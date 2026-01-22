/**
 * @file crypto.h
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
 * <tr><td>2025-05-07 <td>1.0     <td>ZixuanHung     <td>加密算法模块头文件,定义了SHA-1哈希算法和API签名相关函数
 * </table>
 */

#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// SHA-1摘要长度定义
#define SHA1_DIGEST_SIZE 20
#define SHA1_DIGEST_HEX_LENGTH 40

/**
 * @brief 计算SHA-1哈希值
 * 
 * @param data 输入数据
 * @param length 输入数据长度
 * @param output 输出缓冲区，至少需要41字节空间（40字节哈希+1字节结束符）
 * @return int 1成功，0失败
 */
int SHA1_Hash(const char *data, unsigned int length, char *output);

/**
 * SM3哈希算法
 * @param input 输入数据
 * @param output 输出哈希值（至少64字节）
 * @return 1成功，0失败
 */
uint8_t SM3_Hash(const char *input, char *output);

/**
 * SM4加密算法
 * @param input 输入明文
 * @param key 密钥（32字节）
 * @param output 输出密文
 * @return 1成功，0失败
 */
uint8_t SM4_Encrypt(const char *input, const char *key, char *output);

/**
 * SM4解密算法
 * @param input 输入密文
 * @param key 密钥（32字节）
 * @param output 输出明文
 * @return 1成功，0失败
 */
uint8_t SM4_Decrypt(const char *input, const char *key, char *output);

/**
 * 获取当前时间戳
 * @return 当前UNIX时间戳（秒）
 */
uint32_t GetTimeStamp(void);

/**
 * 获取当前时间戳字符串
 * @return 时间戳字符串指针
 */
const char* GetTimeStampStr(void);

/**
 * 获取当前随机nonce
 * @return nonce字符串指针
 */
const char* GetNonce(void);

/**
 * @brief 设置API签名参数
 * 
 * @param api_key API密钥
 * @param device_id 设备ID
 * @param secret_key 密钥
 * @return int 1成功，0失败
 */
int SetAPISignParams(const char *api_key, const char *device_id, const char *secret_key);

/**
 * @brief 生成API签名
 * 主要用于测试目的，与esp8266_api.c中的GenerateSignature函数功能类似但参数不同
 * @param unix_timestamp UNIX时间戳
 * @param output 输出缓冲区，至少需要41字节空间
 * @return int 1成功，0失败
 */
int GenerateAPISignature(uint32_t unix_timestamp, char *output);

/**
 * 字符串比较函数 (用于qsort)
 */
int string_compare(const void *a, const void *b);

#ifdef __cplusplus
}
#endif

#endif /* __CRYPTO_H__ */ 

