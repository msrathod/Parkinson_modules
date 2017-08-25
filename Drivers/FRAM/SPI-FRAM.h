/************************** FRAM Memory Driver ***********************************

   Filename:    SPI-FRAM.h
   Description: Library routines for the SPI-FRAM Memory.

   Version:     1.0
   Author:      Mohit Rathod, ETH Zurich

   Copyright (c) 2017 SMS-Lab, ETH Zurich

********************************************************************************
Parkinson v 2.0, MSR
********************************************************************************
  The following functions are available in this library:

	ReturnType FRAM_ReadStatusRegister(NMX_uint8 *ucpStatusRegister);
    ReturnType FRAM_WriteStatusRegister(NMX_uint8 *ucpStatusRegister);
	ReturnType FRAM_WriteEnable(void);
	ReturnType FRAM_WriteDisable(void);
	ReturnType FRAM_Read(uAddrType udAddr, NMX_uint8 *pArray);
	ReturnType FRAM_Write(uAddrType udAddr, NMX_uint8 *pArray);
	ReturnType FRAM_ReadDeviceIdentification(NMX_uint16 *uwpDeviceIdentification);
	ReturnType FRAM_Unlock(ProtectedRows pr);
	ReturnType FRAM_UnlockAll(void);
	ReturnType FRAM_Sleep(void);
    
********************************************************************************/

#ifndef _FM25V20A_
#define _FM25V20A_

/* Basic Data-type */
typedef unsigned char        NMX_uint8;
typedef signed char          NMX_sint8;
typedef unsigned short       NMX_uint16;
typedef signed short         NMX_sint16;
typedef unsigned int         NMX_uint32;
typedef signed int           NMX_sint32;

typedef NMX_uint32 uAddrType;

/* List of supported device */
#define FM25V02ADGTR

#ifdef FM25V20ADGTR
	/* device details */
	typedef NMX_uint8 dataWidth;					/* FRAM data type */
	#define FRAM_WIDTH				8				/* FRAM data width */
	#define FRAM_SIZE				0x40000		    /* FRAM size in bytes */
    #define FRAM_DEV_ID_LENGTH      ((uint8)0x09)   /* Device ID length */
#endif

#define TRUE 1
#define FALSE 0

/* Functions Return Codes */
typedef enum {
	FRAM_Success,
	FRAM_AddressInvalid,
	FRAM_RegAddressInvalid,
	FRAM_MemoryOverflow,
	FRAM_NoInformationAvailable,
	FRAM_OperationOngoing,
	FRAM_OperationTimeOut,
	FRAM_WriteFailed,
	FRAM_SectorProtected,
	FRAM_SectorUnprotected,
	FRAM_SectorProtectFailed,
	FRAM_SectorUnprotectFailed,
	FRAM_SectorLocked,
	FRAM_SectorUnlocked,
	FRAM_SectorLockDownFailed,
	FRAM_WrongType
} ReturnType;

/*
 * SPI FRAM Command Set Definitions (see Datasheet)
 */
enum
{
    SPI_FRAM_WRITE_STATUS_INS 			= 0x01,
    SPI_FRAM_READ_STATUS_INS 			= 0x05,
    SPI_FRAM_WRITE_INS   				= 0x02,
    SPI_FRAM_READ_INS        			= 0x03,
    SPI_FRAM_READ_ID 					= 0x9F,
    SPI_FRAM_SLEEP 						= 0xB9,
    SPI_FRAM_WRITE_DISABLE 				= 0x04,
    SPI_FRAM_WRITE_ENABLE 				= 0x06
};

/*
 * FRAM Status Register Definitions (see Datasheet)
 */

/*
 * Status register description:
 *
 * Bit7 - WPEN Write Protect Enable
 * Bit6 - reserved (1)
 * Bit5 - reserved (0)
 * Bit4 - reserved (0)
 * Bit3 - BP0 Block Protect bit '1'
 * Bit2 - BP1 Block Protect bit '0'
 * Bit1 - WEL Write enable
 * Bit0 - reserved (0)
 *
 * Block Protect bits (Bit3,Bit2) should be interpreted as follow:
 *
 * Tables of Protected Rows
 *
 * (BP1, BP0)
 *   0    0    ->  None (all unlocked)
 *   0    1    ->  Upper 1/4 locked
 *   1    0    ->  Upper 1/2 locked
 *   1    1    ->  All locked
 *
 */
 /*
 * Values for status register (see Datasheet)
 */

 enum
{
    SPI_FRAM_WPEN   = 0x80, /* write protection */
	SPI_FRAM_BP1 	= 0x08,
	SPI_FRAM_BP0 	= 0x04,
    SPI_FRAM_WE   	= 0x02, /* write enable */
};

typedef enum
{
	SPI_FRAM_PROTECTED_ALL_UNLOCKED		= 0x00, // None�all unlocked
	SPI_FRAM_PROTECTED_1_4_UPPER		= 0x01, // Upper 1/4 locked
	SPI_FRAM_PROTECTED_1_2_UPPER		= 0x02, // Upper 1/2 locked
	SPI_FRAM_PROTECTED_ALL_LOCKED		= 0x03, // All locked (default)
} ProtectedRows;

/*
 * Hardware independent function
 */
ReturnType FRAM_ReadStatusRegister(NMX_uint8 *ucpStatusRegister);
ReturnType FRAM_WriteStatusRegister(NMX_uint8 ucpStatusRegister);
ReturnType FRAM_WriteEnable(void);
ReturnType FRAM_WriteDisable(void);
ReturnType FRAM_Read(uAddrType udAddr, NMX_uint8 *pArray, NMX_uint32 udNrOfElementsInArray);
ReturnType FRAM_Write(uAddrType udAddr, NMX_uint8 *pArray, NMX_uint32 udNrOfElementsInArray);
ReturnType FRAM_ReadDeviceIdentification(NMX_uint8 *devArray);
ReturnType FRAM_Unlock(ProtectedRows pr);
ReturnType FRAM_UnlockAll(void);
ReturnType FRAM_Sleep(void);

#endif /* _FM25V20A_ */