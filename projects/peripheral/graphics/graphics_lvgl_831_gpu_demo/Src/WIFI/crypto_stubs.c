/**
 * @file crypto_stubs.c
 * @brief 
 * @author ZixuanHung (13246009616@163.com)
 * @version 1.0
 * @date 2025-05-07
 * 
 * @copyright Copyright (c) 2025  ZixuanHung
 * 
 * @par LOG:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2025-05-07 <td>1.0     <td>ZixuanHung     <td>加密算法实现
 * </table>
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "crypto.h"
#include "SEGGER_RTT.h"

/* SHA-1算法配置 */
#define HEXCASE 0  /* 16进制输出格式：0-小写，1-大写 */
#define CHRSZ 8    /* 每个输入字符的位数：8-ASCII，16-Unicode */

/* SHA-1算法常量 */
#define SHA1_DIGEST_SIZE 20  /* SHA-1输出是20字节（160位） */

/* SM3算法常量 */
#define SM3_DIGEST_SIZE 32   /* SM3输出是32字节（256位） */
#define SM3_BLOCK_SIZE 64    /* SM3块大小是64字节 */

/* SM4算法常量 */
#define SM4_BLOCK_SIZE 16    /* SM4块大小是16字节 */
#define SM4_KEY_SIZE 16      /* SM4密钥是16字节 */

/* API签名相关变量 */
static char api_key[64] = {0};
static char device_id[64] = {0};
static char secret_key[64] = {0};
static char timestamp_str[16] = "";
static char nonce[64] = {0}; // 添加nonce变量

// 前向声明
static void GenerateRandomString(char *output, uint8_t length);

// SHA-1算法实现
typedef struct {
    unsigned int state[5];  // 哈希状态
    unsigned int count[2];  // 位计数器
    unsigned char buffer[64]; // 输入缓冲区
} SHA1_CTX;

// SM3算法实现
typedef struct {
    uint32_t total[2];     // 消息长度
    uint32_t state[8];     // 哈希状态
    uint8_t buffer[64];    // 输入缓冲区
} SM3_CTX;

// SHA-1算法内部函数
static void SHA1Transform(unsigned int state[5], const unsigned char buffer[64]);
static void SHA1Init(SHA1_CTX *context);
static void SHA1Update(SHA1_CTX *context, const unsigned char *data, unsigned int len);
static void SHA1Final(unsigned char digest[20], SHA1_CTX *context);

// SM3算法内部函数
static void SM3Init(SM3_CTX *ctx);
static void SM3Update(SM3_CTX *ctx, const uint8_t *input, size_t ilen);
static void SM3Final(SM3_CTX *ctx, uint8_t output[32]);
static void SM3Process(SM3_CTX *ctx, const uint8_t data[64]);

// 辅助函数
static void ToHex(const unsigned char *bin, unsigned int len, char *hex);

// 左循环移位
#define ROL(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))
#define ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

// SHA-1基本函数
#define SHA1_F0(b,c,d) (((b) & (c)) | ((~(b)) & (d)))
#define SHA1_F1(b,c,d) ((b) ^ (c) ^ (d))
#define SHA1_F2(b,c,d) (((b) & (c)) | ((b) & (d)) | ((c) & (d)))
#define SHA1_F3(b,c,d) ((b) ^ (c) ^ (d))

// SM3基本常量
#define SM3_T0 0x79CC4519
#define SM3_T1 0x7A879D8A

// SM3基本函数
#define SM3_P0(x) ((x) ^ ROTL32((x), 9) ^ ROTL32((x), 17))
#define SM3_P1(x) ((x) ^ ROTL32((x), 15) ^ ROTL32((x), 23))
#define SM3_FF0(x, y, z) ((x) ^ (y) ^ (z))
#define SM3_FF1(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define SM3_GG0(x, y, z) ((x) ^ (y) ^ (z))
#define SM3_GG1(x, y, z) (((x) & (y)) | ((~(x)) & (z)))

// SM3_GET_UINT32_BE和SM3_PUT_UINT32_BE函数用于处理字节序
#define SM3_GET_UINT32_BE(n, b, i)                \
{                                                  \
    (n) = ((uint32_t)(b)[(i)] << 24)              \
        | ((uint32_t)(b)[(i) + 1] << 16)          \
        | ((uint32_t)(b)[(i) + 2] << 8)           \
        | ((uint32_t)(b)[(i) + 3]);               \
}

#define SM3_PUT_UINT32_BE(n, b, i)                \
{                                                  \
    (b)[(i)] = (uint8_t)((n) >> 24);              \
    (b)[(i) + 1] = (uint8_t)((n) >> 16);          \
    (b)[(i) + 2] = (uint8_t)((n) >> 8);           \
    (b)[(i) + 3] = (uint8_t)((n));                \
}

/**
 * 32位左循环移位
 * 等同于JavaScript中的： function rol(num, cnt)
 */
static uint32_t rol(uint32_t num, uint32_t cnt) {
    return ((num << cnt) | (num >> (32 - cnt)));
}

/**
 * 安全加法，处理32位溢出
 * 等同于JavaScript中的： function safe_add(x, y)
 */
static uint32_t safe_add(uint32_t x, uint32_t y) {
    uint32_t lsw = (x & 0xFFFF) + (y & 0xFFFF);
    uint32_t msw = (x >> 16) + (y >> 16) + (lsw >> 16);
    return (msw << 16) | (lsw & 0xFFFF);
}

/**
 * SHA-1选择函数
 * 等同于JavaScript中的： function sha1_ft(t, b, c, d)
 */
static uint32_t sha1_ft(int t, uint32_t b, uint32_t c, uint32_t d) {
    if (t < 20) return (b & c) | ((~b) & d);
    if (t < 40) return b ^ c ^ d;
    if (t < 60) return (b & c) | (b & d) | (c & d);
    return b ^ c ^ d;
}

/**
 * SHA-1常量函数
 * 等同于JavaScript中的： function sha1_kt(t)
 */
static uint32_t sha1_kt(int t) {
    if (t < 20) return 0x5A827999;
    if (t < 40) return 0x6ED9EBA1;
    if (t < 60) return 0x8F1BBCDC;
    return 0xCA62C1D6;
}

/**
 * 将字符串转换为大端序的字节数组
 * 简化版的str2binb函数，假定输入是ASCII字符串
 */
static void str2binb(const char *str, uint32_t *bin, size_t bin_size) {
    size_t str_len = strlen(str);
    memset(bin, 0, bin_size * sizeof(uint32_t));
    
    for (size_t i = 0; i < str_len; i++) {
        // 按照SHA-1.js中的逻辑，每个字符占8位
        bin[i >> 2] |= (str[i] & 0xFF) << (24 - (i % 4) * 8);
    }
}

/**
 * SHA-1核心算法
 * 等同于JavaScript中的： function core_sha1(x, len)
 */
static void core_sha1(uint32_t *x, size_t len, uint32_t *digest) {
    // 添加填充
    size_t word_len = (len + 8) >> 6; // (len + 8) / 64 * 16
    word_len = (word_len + 1) * 16;   // 确保有足够空间

    // 添加1位
    x[len >> 5] |= 0x80 << (24 - len % 32);
    
    // 添加长度（以位为单位）
    x[(((len + 64) >> 9) << 4) + 15] = len;
    
    uint32_t w[80];
    uint32_t a = 0x67452301;  // 1732584193
    uint32_t b = 0xEFCDAB89;  // -271733879
    uint32_t c = 0x98BADCFE;  // -1732584194
    uint32_t d = 0x10325476;  // 271733878
    uint32_t e = 0xC3D2E1F0;  // -1009589776
    
    // 主循环
    for (size_t i = 0; i < word_len; i += 16) {
        uint32_t olda = a;
        uint32_t oldb = b;
        uint32_t oldc = c;
        uint32_t oldd = d;
        uint32_t olde = e;
        
        // 80轮迭代
        for (int j = 0; j < 80; j++) {
            if (j < 16) {
                w[j] = x[i + j];
            } else {
                w[j] = rol(w[j-3] ^ w[j-8] ^ w[j-14] ^ w[j-16], 1);
            }
            
            uint32_t t = safe_add(safe_add(rol(a, 5), sha1_ft(j, b, c, d)), 
                                safe_add(safe_add(e, w[j]), sha1_kt(j)));
            e = d;
            d = c;
            c = rol(b, 30);
            b = a;
            a = t;
        }
        
        // 更新状态
        a = safe_add(a, olda);
        b = safe_add(b, oldb);
        c = safe_add(c, oldc);
        d = safe_add(d, oldd);
        e = safe_add(e, olde);
    }
    
    // 保存结果
    digest[0] = a;
    digest[1] = b;
    digest[2] = c;
    digest[3] = d;
    digest[4] = e;
}

/**
 * 将32位字数组转换为16进制字符串
 * 等同于JavaScript中的： function binb2hex(binarray)
 */
static void binb2hex(const uint32_t *binarray, size_t len, char *hex_str) {
    static const char hex_tab[] = "0123456789abcdef";
    
    for (size_t i = 0; i < len * 4; i++) {
        // 获取字节的高4位和低4位
        uint8_t byte = (binarray[i >> 2] >> ((3 - (i % 4)) * 8)) & 0xFF;
        hex_str[i * 2] = hex_tab[(byte >> 4) & 0xF];
        hex_str[i * 2 + 1] = hex_tab[byte & 0xF];
    }
    hex_str[len * 8] = '\0';
}

/**
 * SHA-1哈希函数
 * 等同于JavaScript中的： function hex_sha1(s)
 * @param input 输入数据
 * @param output 输出哈希值（至少41字节的缓冲区，40字节哈希值+1字节结束符）
 * @return 1成功，0失败
 */
int SHA1_Hash(const char *data, unsigned int length, char *output) {
    SHA1_CTX context;
    unsigned char digest[20];
    
    SHA1Init(&context);
    SHA1Update(&context, (unsigned char*)data, length);
    SHA1Final(digest, &context);
    
    ToHex(digest, 20, output);
    return 1;
}

/**
 * 字符串比较函数 (用于qsort)
 */
int string_compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

/**
 * 设置API签名参数
 * @param api_key_param API密钥
 * @param device_id_param 设备ID
 * @param secret_key_param 密钥
 * @return 1成功，0失败
 */
int SetAPISignParams(const char *api_key_param, const char *device_id_param, const char *secret_key_param) {
    if (!api_key_param || !device_id_param || !secret_key_param) {
        return 0;
    }
    
    strncpy(api_key, api_key_param, sizeof(api_key) - 1);
    api_key[sizeof(api_key) - 1] = '\0';
    
    strncpy(device_id, device_id_param, sizeof(device_id) - 1);
    device_id[sizeof(device_id) - 1] = '\0';
    
    strncpy(secret_key, secret_key_param, sizeof(secret_key) - 1);
    secret_key[sizeof(secret_key) - 1] = '\0';
    
    // 生成随机nonce
    GenerateRandomString(nonce, 16);
		SEGGER_RTT_printf(0, "nonce:%s\r\n",nonce);
    
    return 1;
}

/**
 * 生成随机字符串
 * @param output 输出缓冲区
 * @param length 生成的字符串长度
 */
static void GenerateRandomString(char *output, uint8_t length) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    
    for (uint8_t i = 0; i < length; i++) {
        uint8_t index = rand() % (sizeof(charset) - 1);
        output[i] = charset[index];
    }
    output[length] = '\0';
}

/**
 * 获取当前随机nonce
 * @return nonce字符串指针
 */
const char* GetNonce(void) {
    return nonce;
}

/**
 * 生成API签名 
 * 主要用于测试目的，与esp8266_api.c中的GenerateSignature函数功能类似但参数不同
 * @param timestamp 时间戳
 * @param output 输出签名 (至少41字节的缓冲区)
 * @return 1成功，0失败
 */
int GenerateAPISignature(uint32_t timestamp, char *output) {
    char signature_str[256] = {0};
    char* parts[5]; // 存储需要排序的5个部分
    char url_with_api[128] = {0};
    char timestamp_str[32] = {0};
    
    // 检查参数
    if (!output) {
        return 0;
    }
    
    // 转换时间戳为字符串
    snprintf(timestamp_str, sizeof(timestamp_str), "%u", timestamp);
    
    // 准备排序的数组
    // 完全按照postData.js的实现: ['/api/' + options.url, uinfo.now_sn, uinfo.nonce, now_time, now_token]
    snprintf(url_with_api, sizeof(url_with_api), "/api/%s", api_key);
    
    parts[0] = url_with_api;      // "/api/路径"
    parts[1] = device_id;         // 设备SN
    parts[2] = nonce;             // 随机数nonce
    parts[3] = timestamp_str;     // 时间戳
    parts[4] = secret_key;        // token
    
    // 排序数组 (使用字符串比较函数)
    qsort(parts, 5, sizeof(char*), string_compare);
    
    // 连接排序后的字符串
    signature_str[0] = '\0';
    for (int i = 0; i < 5; i++) {
        strcat(signature_str, parts[i]);
    }
    
    // 计算SHA-1哈希
    return SHA1_Hash(signature_str, strlen(signature_str), output);
}

/**
 * 获取当前的时间戳字符串
 * @return 时间戳字符串指针
 */
const char* GetTimeStampStr(void) {
    return timestamp_str;
}

/**
 * 获取当前时间戳
 * @return 当前UNIX时间戳（秒）
 */
uint32_t GetTimeStamp(void) {
    
    // 简单返回一个固定值(仅用于测试)
    return 1746884339; // 2023-09-01 00:00:00 UTC
}

/**
 * SM3哈希算法
 * 基于中国国家密码管理局发布的标准实现
 * @param input 输入数据
 * @param output 输出哈希值（至少64字节）
 * @return 1成功，0失败
 */
uint8_t SM3_Hash(const char *input, char *output) {
    if (!input || !output) {
        return 0;
    }
    
    SM3_CTX ctx;
    unsigned char digest[32]; // SM3输出是32字节
    
    // 初始化SM3上下文
    SM3Init(&ctx);
    
    // 处理输入数据
    SM3Update(&ctx, (const uint8_t*)input, strlen(input));
    
    // 完成哈希计算
    SM3Final(&ctx, digest);
    
    // 转换为十六进制字符串
    ToHex(digest, 32, output);
    
    return 1;
}

/**
 * 初始化SM3上下文
 */
static void SM3Init(SM3_CTX *ctx) {
    ctx->total[0] = 0;
    ctx->total[1] = 0;
    
    // 初始化哈希值（IV）
    ctx->state[0] = 0x7380166F;
    ctx->state[1] = 0x4914B2B9;
    ctx->state[2] = 0x172442D7;
    ctx->state[3] = 0xDA8A0600;
    ctx->state[4] = 0xA96F30BC;
    ctx->state[5] = 0x163138AA;
    ctx->state[6] = 0xE38DEE4D;
    ctx->state[7] = 0xB0FB0E4E;
    
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

/**
 * 更新SM3上下文
 */
static void SM3Update(SM3_CTX *ctx, const uint8_t *input, size_t ilen) {
    size_t fill;
    uint32_t left;
    
    if (ilen == 0) {
        return;
    }
    
    left = ctx->total[0] & 0x3F;
    fill = SM3_BLOCK_SIZE - left;
    
    ctx->total[0] += (uint32_t)ilen;
    ctx->total[0] &= 0xFFFFFFFF;
    
    if (ctx->total[0] < (uint32_t)ilen) {
        ctx->total[1]++;
    }
    
    if (left && ilen >= fill) {
        memcpy((void *)(ctx->buffer + left), input, fill);
        SM3Process(ctx, ctx->buffer);
        input += fill;
        ilen -= fill;
        left = 0;
    }
    
    while (ilen >= SM3_BLOCK_SIZE) {
        SM3Process(ctx, input);
        input += SM3_BLOCK_SIZE;
        ilen -= SM3_BLOCK_SIZE;
    }
    
    if (ilen > 0) {
        memcpy((void *)(ctx->buffer + left), input, ilen);
    }
}

/**
 * SM3处理块
 */
static void SM3Process(SM3_CTX *ctx, const uint8_t data[64]) {
    uint32_t W[68];
    uint32_t W1[64];
    uint32_t A, B, C, D, E, F, G, H;
    uint32_t SS1, SS2, TT1, TT2;
    uint32_t T;
    uint32_t j;
    
    // 填充消息
    for (j = 0; j < 16; j++) {
        SM3_GET_UINT32_BE(W[j], data, j * 4);
    }
    
    // 消息扩展
    for (j = 16; j < 68; j++) {
        W[j] = SM3_P1(W[j-16] ^ W[j-9] ^ ROTL32(W[j-3], 15)) ^ ROTL32(W[j-13], 7) ^ W[j-6];
    }
    
    for (j = 0; j < 64; j++) {
        W1[j] = W[j] ^ W[j+4];
    }
    
    // 初始化工作变量
    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];
    F = ctx->state[5];
    G = ctx->state[6];
    H = ctx->state[7];
    
    // 压缩函数主循环
    for (j = 0; j < 64; j++) {
        T = (j < 16) ? SM3_T0 : SM3_T1;
        SS1 = ROTL32((ROTL32(A, 12) + E + ROTL32(T, j)), 7);
        SS2 = SS1 ^ ROTL32(A, 12);
        
        if (j < 16) {
            TT1 = SM3_FF0(A, B, C) + D + SS2 + W1[j];
            TT2 = SM3_GG0(E, F, G) + H + SS1 + W[j];
        } else {
            TT1 = SM3_FF1(A, B, C) + D + SS2 + W1[j];
            TT2 = SM3_GG1(E, F, G) + H + SS1 + W[j];
        }
        
        D = C;
        C = ROTL32(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTL32(F, 19);
        F = E;
        E = SM3_P0(TT2);
    }
    
    // 更新上下文状态
    ctx->state[0] ^= A;
    ctx->state[1] ^= B;
    ctx->state[2] ^= C;
    ctx->state[3] ^= D;
    ctx->state[4] ^= E;
    ctx->state[5] ^= F;
    ctx->state[6] ^= G;
    ctx->state[7] ^= H;
}

/**
 * 完成SM3计算
 */
static void SM3Final(SM3_CTX *ctx, uint8_t output[32]) {
    uint32_t i;
    uint8_t tmp[64];
    uint32_t last, padn;
    uint32_t high, low;
    
    high = (ctx->total[0] >> 29) | (ctx->total[1] << 3);
    low = (ctx->total[0] << 3);
    
    SM3_PUT_UINT32_BE(high, tmp, 0);
    SM3_PUT_UINT32_BE(low, tmp, 4);
    
    last = ctx->total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);
    
    // 填充消息
    tmp[8] = 0x80; // 添加1位
    memset(tmp + 9, 0, padn - 1); // 添加0位
    
    SM3Update(ctx, tmp, padn + 8);
    
    // 输出哈希值
    for (i = 0; i < 8; i++) {
        SM3_PUT_UINT32_BE(ctx->state[i], output, i * 4);
    }
}

// SM4 T变换
static uint32_t SM4_T(uint32_t x, const uint8_t SBOX[256]) {
    uint8_t a[4];
    a[0] = (uint8_t)(x >> 24);
    a[1] = (uint8_t)(x >> 16);
    a[2] = (uint8_t)(x >> 8);
    a[3] = (uint8_t)x;
    
    // S盒替换
    a[0] = SBOX[a[0]];
    a[1] = SBOX[a[1]];
    a[2] = SBOX[a[2]];
    a[3] = SBOX[a[3]];
    
    // 合并
    x = ((uint32_t)a[0] << 24) | ((uint32_t)a[1] << 16) | ((uint32_t)a[2] << 8) | a[3];
    
    // 线性变换L
    return x ^ ROTL32(x, 2) ^ ROTL32(x, 10) ^ ROTL32(x, 18) ^ ROTL32(x, 24);
}

// SM4 T'变换
static uint32_t SM4_T_Prime(uint32_t x, const uint8_t SBOX[256]) {
    uint8_t a[4];
    a[0] = (uint8_t)(x >> 24);
    a[1] = (uint8_t)(x >> 16);
    a[2] = (uint8_t)(x >> 8);
    a[3] = (uint8_t)x;
    
    // S盒替换
    a[0] = SBOX[a[0]];
    a[1] = SBOX[a[1]];
    a[2] = SBOX[a[2]];
    a[3] = SBOX[a[3]];
    
    // 合并
    x = ((uint32_t)a[0] << 24) | ((uint32_t)a[1] << 16) | ((uint32_t)a[2] << 8) | a[3];
    
    // 线性变换L'
    return x ^ ROTL32(x, 13) ^ ROTL32(x, 23);
}

/**
 * SM4加密算法
 * 基于中国国家密码管理局发布的标准实现
 * 兼容原有接口：输入明文字符串，输出兼容原接口格式
 * @param input 输入明文
 * @param key 密钥（32字节十六进制字符串，对应16字节二进制）
 * @param output 输出密文（十六进制字符串）
 * @return 1成功，0失败
 */
uint8_t SM4_Encrypt(const char *input, const char *key, char *output) {
    if (!input || !key || !output) {
        return 0;
    }
    
    // SM4 S盒
    static const uint8_t SBOX[256] = {
        0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7, 0x16, 0xb6, 0x14, 0xc2, 0x28, 0xfb, 0x2c, 0x05,
        0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3, 0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99,
        0x9c, 0x42, 0x50, 0xf4, 0x91, 0xef, 0x98, 0x7a, 0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62,
        0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95, 0x80, 0xdf, 0x94, 0xfa, 0x75, 0x8f, 0x3f, 0xa6,
        0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba, 0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8,
        0x68, 0x6b, 0x81, 0xb2, 0x71, 0x64, 0xda, 0x8b, 0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35,
        0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2, 0x25, 0x22, 0x7c, 0x3b, 0x01, 0x21, 0x78, 0x87,
        0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52, 0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e,
        0xea, 0xbf, 0x8a, 0xd2, 0x40, 0xc7, 0x38, 0xb5, 0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1,
        0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55, 0xad, 0x93, 0x32, 0x30, 0xf5, 0x8c, 0xb1, 0xe3,
        0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60, 0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f,
        0xd5, 0xdb, 0x37, 0x45, 0xde, 0xfd, 0x8e, 0x2f, 0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51,
        0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f, 0x11, 0xd9, 0x5c, 0x41, 0x1f, 0x10, 0x5a, 0xd8,
        0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd, 0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0,
        0x89, 0x69, 0x97, 0x4a, 0x0c, 0x96, 0x77, 0x7e, 0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84,
        0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20, 0x79, 0xee, 0x5f, 0x3e, 0xd7, 0xcb, 0x39, 0x48
    };
    
    // SM4系统参数
    static const uint32_t FK[4] = {
        0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc
    };
    
    // SM4固定参数
    static const uint32_t CK[32] = {
        0x00070e15, 0x1c232a31, 0x383f464d, 0x545b6269,
        0x70777e85, 0x8c939aa1, 0xa8afb6bd, 0xc4cbd2d9,
        0xe0e7eef5, 0xfc030a11, 0x181f262d, 0x343b4249,
        0x50575e65, 0x6c737a81, 0x888f969d, 0xa4abb2b9,
        0xc0c7ced5, 0xdce3eaf1, 0xf8ff060d, 0x141b2229,
        0x30373e45, 0x4c535a61, 0x686f767d, 0x848b9299,
        0xa0a7aeb5, 0xbcc3cad1, 0xd8dfe6ed, 0xf4fb0209,
        0x10171e25, 0x2c333a41, 0x484f565d, 0x646b7279
    };
    
    // 将16进制字符串转换为二进制密钥
    uint8_t binary_key[16] = {0};
    for (int i = 0; i < 16; i++) {
        unsigned int value;
        sscanf(key + i*2, "%2x", &value);
        binary_key[i] = (uint8_t)value;
    }
    
    // 轮密钥扩展
    uint32_t rk[32];
    uint32_t MK[4];
    uint32_t K[36];
    
    for (int i = 0; i < 4; i++) {
        MK[i] = ((uint32_t)binary_key[i*4] << 24) | 
                ((uint32_t)binary_key[i*4+1] << 16) | 
                ((uint32_t)binary_key[i*4+2] << 8) | 
                binary_key[i*4+3];
        K[i] = MK[i] ^ FK[i];
    }
    
    for (int i = 0; i < 32; i++) {
        K[i+4] = K[i] ^ SM4_T_Prime(K[i+1] ^ K[i+2] ^ K[i+3] ^ CK[i], SBOX);
        rk[i] = K[i+4];
    }
    
    // 明文长度和填充
    size_t input_len = strlen(input);
    size_t padding_len = 16 - (input_len % 16);
    size_t total_len = input_len + padding_len;
    
    uint8_t *padded_input = (uint8_t *)malloc(total_len);
    if (!padded_input) {
        return 0;
    }
    
    memcpy(padded_input, input, input_len);
    memset(padded_input + input_len, padding_len, padding_len); // PKCS#7填充
    
    // 分组加密
    size_t num_blocks = total_len / 16;
    uint8_t *cipher = (uint8_t *)malloc(total_len);
    if (!cipher) {
        free(padded_input);
        return 0;
    }
    
    for (size_t i = 0; i < num_blocks; i++) {
        uint32_t X[4];
        uint32_t tmp;
        
        // 加载数据块
        for (int j = 0; j < 4; j++) {
            X[j] = ((uint32_t)padded_input[i*16+j*4] << 24) | 
                   ((uint32_t)padded_input[i*16+j*4+1] << 16) | 
                   ((uint32_t)padded_input[i*16+j*4+2] << 8) | 
                   padded_input[i*16+j*4+3];
        }
        
        // 32轮加密
        for (int j = 0; j < 32; j++) {
            tmp = X[1] ^ X[2] ^ X[3] ^ rk[j];
            tmp = SM4_T(tmp, SBOX);
            X[0] ^= tmp;
            
            // 旋转
            tmp = X[0];
            X[0] = X[1];
            X[1] = X[2];
            X[2] = X[3];
            X[3] = tmp;
        }
        
        // 反序变换
        for (int j = 0; j < 4; j++) {
            tmp = X[3-j];
            cipher[i*16+j*4] = (uint8_t)(tmp >> 24);
            cipher[i*16+j*4+1] = (uint8_t)(tmp >> 16);
            cipher[i*16+j*4+2] = (uint8_t)(tmp >> 8);
            cipher[i*16+j*4+3] = (uint8_t)tmp;
        }
    }
    
    // 转换结果为十六进制字符串
    for (size_t i = 0; i < total_len; i++) {
        sprintf(output + i*2, "%02x", cipher[i]);
    }
    output[total_len*2] = '\0';
    
    free(padded_input);
    free(cipher);
    return 1;
}

/**
 * SM4解密算法
 * 基于中国国家密码管理局发布的标准实现
 * 兼容原有接口：输入十六进制字符串或JSON字符串，输出明文字符串
 * @param input 输入密文
 * @param key 密钥（32字节十六进制字符串，对应16字节二进制）
 * @param output 输出明文
 * @return 1成功，0失败
 */
uint8_t SM4_Decrypt(const char *input, const char *key, char *output) {
    if (!input || !key || !output) {
        return 0;
    }
    
    // 检查输入是否是JSON（包含大括号）
    uint8_t is_json = (input[0] == '{');
    
    if (is_json) {
        // 对于JSON输入，直接复制到输出，保持兼容性
        // 这里为了与旧代码兼容，直接返回
        strcpy(output, input);
        return 1;
    }
    
    // SM4 S盒
    static const uint8_t SBOX[256] = {
        0xd6, 0x90, 0xe9, 0xfe, 0xcc, 0xe1, 0x3d, 0xb7, 0x16, 0xb6, 0x14, 0xc2, 0x28, 0xfb, 0x2c, 0x05,
        0x2b, 0x67, 0x9a, 0x76, 0x2a, 0xbe, 0x04, 0xc3, 0xaa, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99,
        0x9c, 0x42, 0x50, 0xf4, 0x91, 0xef, 0x98, 0x7a, 0x33, 0x54, 0x0b, 0x43, 0xed, 0xcf, 0xac, 0x62,
        0xe4, 0xb3, 0x1c, 0xa9, 0xc9, 0x08, 0xe8, 0x95, 0x80, 0xdf, 0x94, 0xfa, 0x75, 0x8f, 0x3f, 0xa6,
        0x47, 0x07, 0xa7, 0xfc, 0xf3, 0x73, 0x17, 0xba, 0x83, 0x59, 0x3c, 0x19, 0xe6, 0x85, 0x4f, 0xa8,
        0x68, 0x6b, 0x81, 0xb2, 0x71, 0x64, 0xda, 0x8b, 0xf8, 0xeb, 0x0f, 0x4b, 0x70, 0x56, 0x9d, 0x35,
        0x1e, 0x24, 0x0e, 0x5e, 0x63, 0x58, 0xd1, 0xa2, 0x25, 0x22, 0x7c, 0x3b, 0x01, 0x21, 0x78, 0x87,
        0xd4, 0x00, 0x46, 0x57, 0x9f, 0xd3, 0x27, 0x52, 0x4c, 0x36, 0x02, 0xe7, 0xa0, 0xc4, 0xc8, 0x9e,
        0xea, 0xbf, 0x8a, 0xd2, 0x40, 0xc7, 0x38, 0xb5, 0xa3, 0xf7, 0xf2, 0xce, 0xf9, 0x61, 0x15, 0xa1,
        0xe0, 0xae, 0x5d, 0xa4, 0x9b, 0x34, 0x1a, 0x55, 0xad, 0x93, 0x32, 0x30, 0xf5, 0x8c, 0xb1, 0xe3,
        0x1d, 0xf6, 0xe2, 0x2e, 0x82, 0x66, 0xca, 0x60, 0xc0, 0x29, 0x23, 0xab, 0x0d, 0x53, 0x4e, 0x6f,
        0xd5, 0xdb, 0x37, 0x45, 0xde, 0xfd, 0x8e, 0x2f, 0x03, 0xff, 0x6a, 0x72, 0x6d, 0x6c, 0x5b, 0x51,
        0x8d, 0x1b, 0xaf, 0x92, 0xbb, 0xdd, 0xbc, 0x7f, 0x11, 0xd9, 0x5c, 0x41, 0x1f, 0x10, 0x5a, 0xd8,
        0x0a, 0xc1, 0x31, 0x88, 0xa5, 0xcd, 0x7b, 0xbd, 0x2d, 0x74, 0xd0, 0x12, 0xb8, 0xe5, 0xb4, 0xb0,
        0x89, 0x69, 0x97, 0x4a, 0x0c, 0x96, 0x77, 0x7e, 0x65, 0xb9, 0xf1, 0x09, 0xc5, 0x6e, 0xc6, 0x84,
        0x18, 0xf0, 0x7d, 0xec, 0x3a, 0xdc, 0x4d, 0x20, 0x79, 0xee, 0x5f, 0x3e, 0xd7, 0xcb, 0x39, 0x48
    };
    
    // SM4系统参数
    static const uint32_t FK[4] = {
        0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc
    };
    
    // SM4固定参数
    static const uint32_t CK[32] = {
        0x00070e15, 0x1c232a31, 0x383f464d, 0x545b6269,
        0x70777e85, 0x8c939aa1, 0xa8afb6bd, 0xc4cbd2d9,
        0xe0e7eef5, 0xfc030a11, 0x181f262d, 0x343b4249,
        0x50575e65, 0x6c737a81, 0x888f969d, 0xa4abb2b9,
        0xc0c7ced5, 0xdce3eaf1, 0xf8ff060d, 0x141b2229,
        0x30373e45, 0x4c535a61, 0x686f767d, 0x848b9299,
        0xa0a7aeb5, 0xbcc3cad1, 0xd8dfe6ed, 0xf4fb0209,
        0x10171e25, 0x2c333a41, 0x484f565d, 0x646b7279
    };
    
    // 将16进制字符串转换为二进制密钥
    uint8_t binary_key[16] = {0};
    for (int i = 0; i < 16; i++) {
        unsigned int value;
        sscanf(key + i*2, "%2x", &value);
        binary_key[i] = (uint8_t)value;
    }
    
    // 轮密钥扩展
    uint32_t rk[32];
    uint32_t MK[4];
    uint32_t K[36];
    
    for (int i = 0; i < 4; i++) {
        MK[i] = ((uint32_t)binary_key[i*4] << 24) | 
                ((uint32_t)binary_key[i*4+1] << 16) | 
                ((uint32_t)binary_key[i*4+2] << 8) | 
                binary_key[i*4+3];
        K[i] = MK[i] ^ FK[i];
    }
    
    for (int i = 0; i < 32; i++) {
        K[i+4] = K[i] ^ SM4_T_Prime(K[i+1] ^ K[i+2] ^ K[i+3] ^ CK[i], SBOX);
        rk[i] = K[i+4];
    }
    
    // 计算密文字节长度
    size_t input_len = strlen(input) / 2;
    if (input_len % 16 != 0) {
        return 0; // 密文长度必须是16的倍数
    }
    
    // 将十六进制字符串转换为二进制
    uint8_t *cipher = (uint8_t *)malloc(input_len);
    if (!cipher) {
        return 0;
    }
    
    for (size_t i = 0; i < input_len; i++) {
        unsigned int value;
        sscanf(input + i*2, "%2x", &value);
        cipher[i] = (uint8_t)value;
    }
    
    // 分组解密
    size_t num_blocks = input_len / 16;
    uint8_t *plain = (uint8_t *)malloc(input_len);
    if (!plain) {
        free(cipher);
        return 0;
    }
    
    for (size_t i = 0; i < num_blocks; i++) {
        uint32_t X[4];
        uint32_t tmp;
        
        // 加载数据块
        for (int j = 0; j < 4; j++) {
            X[j] = ((uint32_t)cipher[i*16+j*4] << 24) | 
                   ((uint32_t)cipher[i*16+j*4+1] << 16) | 
                   ((uint32_t)cipher[i*16+j*4+2] << 8) | 
                   cipher[i*16+j*4+3];
        }
        
        // 32轮解密（使用逆序的轮密钥）
        for (int j = 0; j < 32; j++) {
            tmp = X[1] ^ X[2] ^ X[3] ^ rk[31-j];
            tmp = SM4_T(tmp, SBOX);
            X[0] ^= tmp;
            
            // 旋转
            tmp = X[0];
            X[0] = X[1];
            X[1] = X[2];
            X[2] = X[3];
            X[3] = tmp;
        }
        
        // 反序变换
        for (int j = 0; j < 4; j++) {
            tmp = X[3-j];
            plain[i*16+j*4] = (uint8_t)(tmp >> 24);
            plain[i*16+j*4+1] = (uint8_t)(tmp >> 16);
            plain[i*16+j*4+2] = (uint8_t)(tmp >> 8);
            plain[i*16+j*4+3] = (uint8_t)tmp;
        }
    }
    
    // 去除PKCS#7填充
    uint8_t padding_len = plain[input_len - 1];
    if (padding_len > 16 || padding_len == 0) {
        free(cipher);
        free(plain);
        return 0;
    }
    
    // 验证填充
    for (int i = input_len - padding_len; i < input_len; i++) {
        if (plain[i] != padding_len) {
            free(cipher);
            free(plain);
            return 0;
        }
    }
    
    // 复制明文到输出
    memcpy(output, plain, input_len - padding_len);
    output[input_len - padding_len] = '\0';
    
    free(cipher);
    free(plain);
    return 1;
}

// 初始化SHA-1上下文
static void SHA1Init(SHA1_CTX *context) {
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}

// 更新SHA-1上下文
static void SHA1Update(SHA1_CTX *context, const unsigned char *data, unsigned int len) {
    unsigned int i, j;

    j = (context->count[0] >> 3) & 63;
    if ((context->count[0] += len << 3) < (len << 3))
        context->count[1]++;
    context->count[1] += (len >> 29);
    
    if ((j + len) > 63) {
        memcpy(&context->buffer[j], data, (i = 64-j));
        SHA1Transform(context->state, context->buffer);
        for (; i + 63 < len; i += 64) {
            SHA1Transform(context->state, &data[i]);
        }
        j = 0;
    } else {
        i = 0;
    }
    memcpy(&context->buffer[j], &data[i], len - i);
}

// 完成SHA-1计算
static void SHA1Final(unsigned char digest[20], SHA1_CTX *context) {
    unsigned char finalcount[8];
    unsigned int i;

    for (i = 0; i < 8; i++) {
        finalcount[i] = (unsigned char)((context->count[(i >= 4 ? 0 : 1)]
            >> ((3-(i & 3)) * 8) ) & 255);
    }
    
    unsigned char c = 0x80;
    SHA1Update(context, &c, 1);
    
    while ((context->count[0] & 504) != 448) {
        c = 0x00;
        SHA1Update(context, &c, 1);
    }
    
    SHA1Update(context, finalcount, 8);
    
    for (i = 0; i < 20; i++) {
        digest[i] = (unsigned char)((context->state[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
    }
}

// SHA-1的核心转换函数
static void SHA1Transform(unsigned int state[5], const unsigned char buffer[64]) {
    unsigned int a, b, c, d, e;
    unsigned int block[80];
    
    // 将输入块转换为单词
    for (int i = 0; i < 16; i++) {
        block[i] = (buffer[i*4] << 24) | 
                   (buffer[i*4+1] << 16) | 
                   (buffer[i*4+2] << 8) | 
                   (buffer[i*4+3]);
    }
    
    // 扩展16个单词为80个单词
    for (int i = 16; i < 80; i++) {
        block[i] = ROL(block[i-3] ^ block[i-8] ^ block[i-14] ^ block[i-16], 1);
    }
    
    // 初始化哈希值
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    
    // 主循环
    for (int i = 0; i < 80; i++) {
        unsigned int temp;
        
        if (i < 20) {
            temp = ROL(a, 5) + SHA1_F0(b,c,d) + e + block[i] + 0x5A827999;
        } else if (i < 40) {
            temp = ROL(a, 5) + SHA1_F1(b,c,d) + e + block[i] + 0x6ED9EBA1;
        } else if (i < 60) {
            temp = ROL(a, 5) + SHA1_F2(b,c,d) + e + block[i] + 0x8F1BBCDC;
        } else {
            temp = ROL(a, 5) + SHA1_F3(b,c,d) + e + block[i] + 0xCA62C1D6;
        }
        
        e = d;
        d = c;
        c = ROL(b, 30);
        b = a;
        a = temp;
    }
    
    // 更新状态
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

// 将二进制转换为十六进制字符串
static void ToHex(const unsigned char *bin, unsigned int len, char *hex) {
    static const char hexchars[] = "0123456789abcdef";
    for (unsigned int i = 0; i < len; i++) {
        hex[i * 2] = hexchars[(bin[i] >> 4) & 0xF];
        hex[i * 2 + 1] = hexchars[bin[i] & 0xF];
    }
    hex[len * 2] = '\0';
} 

