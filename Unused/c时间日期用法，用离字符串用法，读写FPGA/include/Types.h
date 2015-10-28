/***************************************************************************
 *   
 *                                NTS公司版权所有
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename       :  Types.h
 * @Version        :  1.0.0
 * @Brief            :  基本类型定义
 *   
 * @Author         :  zhangx
 * @CreatedDate :  2011年3月22日 14:05:34
 * @History         : 
 *   
 */

#ifndef TYPES_H
#define TYPES_H
/************************引用部分*****************************/

/************************前向声明部分***********************/

/************************定义部分*****************************/
typedef unsigned char  uint8;       // 变量简称u8  
typedef unsigned short uint16;      // 变量简称u16  
typedef unsigned int   uint32;      // 变量简称u32  
typedef unsigned long  ulint32;     // 变量简称ul  

typedef signed char    int8;        // 变量简称i8  
typedef signed short   int16;       // 变量简称i16  
typedef signed int     int32;       // 变量简称i32  
typedef signed long    lint32;      // 变量简称il  

typedef float          float32;    // 变量简称f32  
typedef double         float64;    // 变量简称f64  

typedef enum
{
    TRUE = 1,
    FALSE = 0
}_enBool;                          // 变量简称b

typedef enum
{
    SUCC = 0,
    FAIL = 1
}_enResult;                       //操作成功或失败

#endif 
