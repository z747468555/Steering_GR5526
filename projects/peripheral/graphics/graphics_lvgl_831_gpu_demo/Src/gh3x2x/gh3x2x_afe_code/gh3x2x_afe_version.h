/**
 * @copyright (c) 2003 - 2022, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh3x2x_afe_version.h
 *
 * @brief   gh3x2x afe driver version and change log
 *
 * @par  history:
 * @verbatim
 * Date            Version     Notes
 * 2021-11-04      v3.2.4.2    1.add calibration code
 * 2021-11-05      v3.2.4.4    1.fix Efuse calibration bug
 * 2021-11-05      v3.2.4.5    1.add Ffuse Crc Process
 *                             2.add Check Chip Id process
 *
 * 2021-11-05      v3.2.4.6    1.update Head file
 * 2021-11-05      v3.2.4.7    1.add ecg lead off det lib
 * @endverbatim  */

#ifndef _GH3X2X_AFE_VERSION_H_
#define _GH3X2X_AFE_VERSION_H_


#define   GH3X2X_DRV_MAJOR_VERSION_NUMBER        3      /**< major version number */
#define   GH3X2X_DRV_MINOR_VERSION_NUMBER        2      /**< minor version number */
#define   GH3X2X_DRV_REVISION_VERSION_NUMBER     4      /**< revision version number */
#define   GH3X2X_DRV_FIXED_VERSION_NUMBER        7      /**< fixed version number */

#define   GH3X2X_TO_STRING(x)       #x                      /**< number to char */
#define   GH3X2X_STR(x)             GH3X2X_TO_STRING(x)     /**< number to char */

/// makeup version string
#define   GH3X2X_VERSION_STRING     "v"GH3X2X_STR(GH3X2X_DRV_MAJOR_VERSION_NUMBER)\
                                    "."GH3X2X_STR(GH3X2X_DRV_MINOR_VERSION_NUMBER)\
                                    "."GH3X2X_STR(GH3X2X_DRV_REVISION_VERSION_NUMBER)\
                                    "."GH3X2X_STR(GH3X2X_DRV_FIXED_VERSION_NUMBER)\
                                    " (build:"__DATE__"_"__TIME__")"


#endif /* _GH3X2X_AFE_VERSION_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2022, Goodix Co., Ltd. ********/
