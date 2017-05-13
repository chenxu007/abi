#ifndef __BHT_TYPE_H__
#define __BHT_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef signed   char                   s8;                     /* 有符号8位整数别名定义                            */
typedef signed   short                  s16;                    /* 有符号16位整数别名定义                           */
typedef signed   int                    s32;                    /* 有符号32位整数别名定义                           */
typedef unsigned char                   u8;                     /* 无符号8位整数别名定义                            */
typedef unsigned short                  u16;                    /* 无符号16位整数别名定义                           */
typedef unsigned int                    u32;                    /* 无符号32位整数别名定义                           */
typedef unsigned long long int          u64;
typedef signed   int					status_t;
typedef volatile signed char            vs8;                    /* volatile 有符号8位整数别名定义                   */
typedef volatile signed short           vs16;                   /* volatile 有符号16位整数别名定义                  */
typedef volatile signed int             vs32;                   /* volatile 有符号32位整数别名定义                  */
typedef volatile unsigned char          vu8;                    /* volatile 无符号8位整数别名定义                   */
typedef volatile unsigned short         vu16;                   /* volatile 无符号16位整数别名定义                  */
typedef volatile unsigned int           vu32;                   /* volatile 无符号32位整数别名定义                  */
typedef volatile unsigned char          VBOOL;                  /* 暂态型布尔类型定义                               */

typedef int             (*FUNCPTR) ();     /* ptr to function returning int */
typedef void            (*VOIDFUNCPTR) (); /* ptr to function returning void */
typedef double          (*DBLFUNCPTR) ();  /* ptr to function returning double*/
typedef float           (*FLTFUNCPTR) ();  /* ptr to function returning float */

typedef int             SYMTAB_ID;
typedef int             SYM_TYPE;

#ifdef __cplusplus
}
#endif

#endif