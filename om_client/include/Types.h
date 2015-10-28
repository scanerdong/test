/***************************************************************************
 *   
 *                                NTS��˾��Ȩ����
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename       :  Types.h
 * @Version        :  1.0.0
 * @Brief            :  �������Ͷ���
 *   
 * @Author         :  zhangx
 * @CreatedDate :  2011��3��22�� 14:05:34
 * @History         : 
 *   
 */

#ifndef TYPES_H
#define TYPES_H
/************************���ò���*****************************/

/************************ǰ����������***********************/

/************************���岿��*****************************/
typedef unsigned char  uint8;       // �������u8  
typedef unsigned short uint16;      // �������u16  
typedef unsigned int   uint32;      // �������u32  
typedef unsigned long  ulint32;     // �������ul  

typedef signed char    int8;        // �������i8  
typedef signed short   int16;       // �������i16  
typedef signed int     int32;       // �������i32  
typedef signed long    lint32;      // �������il  

typedef float          float32;    // �������f32  
typedef double         float64;    // �������f64  

typedef enum
{
    TRUE = 1,
    FALSE = 0
}_enBool;                          // �������b

typedef enum
{
    SUCC = 0,
    FAIL = 1
}_enResult;                       //�����ɹ���ʧ��

#endif 
