/******************************************************************************
 * (c) Copyright 2010-2015, Freescale Semiconductor Inc.
 * ALL RIGHTS RESERVED.
 ***************************************************************************//*!
 * @file      ftfa.c
 * @version   1.0.5.0
 * @date      August-18-2015
 * @brief     Flash Memory Module (FTFA) driver source code.
 ******************************************************************************/
#include "common.h"
#include "ftfa.h"

/******************************************************************************
 * static function prototypes                                                 *
 ******************************************************************************/
#if defined (__ICCARM__)
  static __ramfunc void _FTFA_ExecCmd (void);
#elif defined(__GNUC__) || defined(__CC_ARM)
  static void _FTFA_ExecCmd (void) __attribute__((long_call, section(".fast")));
#endif

/******************************************************************************
 * static function definitions                                                *
 ******************************************************************************/
#if defined (__ICCARM__)
  static __ramfunc void _FTFA_ExecCmd (void)
#elif defined(__GNUC__) || defined(__CC_ARM)
  static void _FTFA_ExecCmd (void)
#endif
{
  FTFA_FSTAT = FTFA_FSTAT_CCIF_MASK;
  while (!(FTFA_FSTAT & FTFA_FSTAT_CCIF_MASK));
}

/******************************************************************************
 * public function definitions                                                *
 ******************************************************************************/
int _FTFA_WriteArray (const void *pDst, void *pSrc, int size)
{
  register int n = 0;
  register uint32 *pUlongDst = (uint32*)pDst;
  register uint32 *pUlongSrc = (uint32*)pSrc;

  while ((size-n) > 0) { n+=_FTFA_WriteW32 (pUlongDst++, *(pUlongSrc++)); }

  return n;
}

int _FTFA_ReadArray (void *pSrc, void *pDst, int size)
{
  register uint8 *src = (uint8*)pSrc;
  register uint8 *dst = (uint8*)pDst;
  register int n = size;

  while (n--) { *dst++ = *src++; }

  return size;
}

int _FTFA_WriteW32 (const uint32 *pDst, uint32 val)
{
  if (FTFA_FSTAT &
     (FTFA_FSTAT_FPVIOL_MASK|FTFA_FSTAT_ACCERR_MASK|FTFA_FSTAT_RDCOLERR_MASK))
  {
    FTFA_FSTAT = FTFA_FSTAT_FPVIOL_MASK|FTFA_FSTAT_ACCERR_MASK|FTFA_FSTAT_RDCOLERR_MASK;
  }
  FTFA_FCCOB0 = 0x06;
  FTFA_FCCOB1 = (uint8)((uint32)pDst>> 16);
  FTFA_FCCOB2 = (uint8)((uint32)pDst>>  8);
  FTFA_FCCOB3 = (uint8)((uint32)pDst>>  0);
  FTFA_FCCOB4 = (uint8)(val >> 24);
  FTFA_FCCOB5 = (uint8)(val >> 16);
  FTFA_FCCOB6 = (uint8)(val >>  8);
  FTFA_FCCOB7 = (uint8)(val >>  0);
  _FTFA_ExecCmd ();

  return sizeof(uint32);
}

void _FTFA_EraseSector (const void *pDst)
{
  if (FTFA_FSTAT &
     (FTFA_FSTAT_FPVIOL_MASK|FTFA_FSTAT_ACCERR_MASK|FTFA_FSTAT_RDCOLERR_MASK))
  {
    FTFA_FSTAT = FTFA_FSTAT_FPVIOL_MASK|FTFA_FSTAT_ACCERR_MASK|FTFA_FSTAT_RDCOLERR_MASK;
  }
  FTFA_FCCOB0 = 0x09;
  FTFA_FCCOB1 = (uint8)((uint32)pDst>> 16);
  FTFA_FCCOB2 = (uint8)((uint32)pDst>>  8);
  FTFA_FCCOB3 = (uint8)((uint32)pDst>>  0);
  _FTFA_ExecCmd ();
}

uint32 _FTFA_ReadIFR (uint8 addr)
{
  if (FTFA_FSTAT &
     (FTFA_FSTAT_FPVIOL_MASK|FTFA_FSTAT_ACCERR_MASK|FTFA_FSTAT_RDCOLERR_MASK))
  {
    FTFA_FSTAT = FTFA_FSTAT_FPVIOL_MASK|FTFA_FSTAT_ACCERR_MASK|FTFA_FSTAT_RDCOLERR_MASK;
  }
  FTFA_FCCOB0 = 0x03;
  FTFA_FCCOB1 = 0x00;
  FTFA_FCCOB2 = 0x00;
  FTFA_FCCOB3 = (addr&(~0x03)); /* force longword alignment of the address    */
  FTFA_FCCOB8 = 0x00;
  _FTFA_ExecCmd ();

  return *((uint32*)&FTFA_FCCOB7);
}

/******************************************************************************
 * interrupt function definitions                                             *
 ******************************************************************************/
weak void ftfa_isr (void) { for (;;) {} }
/******************************************************************************
 * End of module                                                              *
 ******************************************************************************/
