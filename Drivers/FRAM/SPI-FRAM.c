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

#include "SPI-FRAM.h"

#ifdef FM25V02ADGTR
/*
 *
 *  ADDRESS STRUCTURE for FM25V02ADx devices
 *
 * 		31                                    0
 * 		+--------+----------------------------+
 * 		| ///////|           BYTES            |
 * 		+--------+----------------------------+
 *
 */
/*
 * void Build_Address_Stream(NMX_uint32 udAddr, NMX_uint8 cCMD, NMX_uint8 *chars);
 * (This is not an api function)
 */
static inline void Build_Address_Stream(NMX_uint32 udAddr, NMX_uint8 cCMD, NMX_uint8 *chars)
{
	udAddr &= 0x0003FFFFul; /* row mask */
	chars[0] = (NMX_uint8) cCMD;
	chars[1] = (NMX_uint8) ((udAddr>>16) & 0x3F);
	chars[2] = (NMX_uint8) (udAddr>>8);
	chars[3] = (NMX_uint8) (udAddr);
	return;
}
#endif 

/*******************************************************************************
 *
 * Function:		ReadStatusRegister()
 * Arguments:		NMX_uint8 *ucpStatusRegister
 * Return Value:	FRAM_Success
 * Description:
 *
 * Return the status of the status register
 *
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
 * Pseudo Code:
 *
 *    - Send Command (0x05) get features
 *    - Read the data serially to get status register content
 *
 *******************************************************************************/
ReturnType FRAM_ReadStatusRegister(NMX_uint8 *ucpStatusRegister)
{
    CharStream char_stream_send;
    CharStream char_stream_recv;
	NMX_uint8 chars;

	chars = (NMX_uint8) SPI_FRAM_READ_STATUS_INS;
	
    // Step 1: Initialize the data (i.e. Instruction) packet to be sent serially
    char_stream_send.length  = 1;
    char_stream_send.pChar   = &chars;
    char_stream_recv.length  = 1;
    char_stream_recv.pChar   = ucpStatusRegister;

    // Step 2: Send the packet serially, get the Status Register content
    Serialize_SPI(&char_stream_send, &char_stream_recv, OpsWakeUp, OpsEndTransfer);

    return FRAM_Success;
}

/*******************************************************************************
 *
 * Function:		WriteStatusRegister()
 * Arguments:		NMX_uint8 *ucpStatusRegister
 * Return Value:	FRAM_Success
 * Description:
 *
 * Return the status of the status register
 *
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
 * Pseudo Code:
 *
 *    - Send Command (0x01) get features
 *    - Send the new data to write status register content
 *
 *******************************************************************************/
ReturnType FRAM_WriteStatusRegister(NMX_uint8 *ucpStatusRegister)
{
    CharStream char_stream_send;
	NMX_uint8 chars[2];

	chars[0] = (NMX_uint8) SPI_FRAM_WRITE_STATUS_INS;
    chars[1] = *ucpStatusRegister;
	
    // Step 1: Write Enable
    FRAM_WriteEnable();

    // Step 2: Initialize the data (i.e. Instruction) packet to be sent serially
    char_stream_send.length  = 2;
    char_stream_send.pChar   = chars;

    // Step 3: Send the packet serially, get the Status Register content
    Serialize_SPI(&char_stream_send, NULL_PTR, OpsWakeUp, OpsEndTransfer);

    return FRAM_Success;
}

/*******************************************************************************
 *
 * Function:     FRAM_WriteEnable()
 * Arguments:    none
 * Return Value: FRAM_Success
 * Description:
 *
 * This function sets the Write Enable Latch(WEL) by sending a write
 * enable Instruction.
 *
 * Pseudo Code:
 *
 *    - Initialize the data (i.e. Instruction) packet to be sent serially
 *    - Send the packet serially
 *
 ******************************************************************************/
ReturnType FRAM_WriteEnable(void)
{
    CharStream char_stream_send;
    NMX_uint8  cWREN = SPI_FRAM_WRITE_ENABLE;
    NMX_uint8 ucSR;

    // Step 1: Initialize the data (i.e. Instruction) packet to be sent serially
    char_stream_send.length = 1;
    char_stream_send.pChar  = &cWREN;

    // Step 2: Send the packet serially
    Serialize_SPI(&char_stream_send, NULL_PTR, OpsWakeUp, OpsEndTransfer);

    // Step 3: Read the Status Register.
    do {
        FRAM_ReadStatusRegister(&ucSR);
    } while(~ucSR & SPI_FRAM_WE);
    
    return FRAM_Success;
}

/*******************************************************************************
 * Function:     FRAM_WriteDisable()
 * Arguments:    none
 * Return Value: FRAM_Success
 * Description:
 *
 * This function resets the Write Enable Latch(WEL) by sending a
 * WRDI Instruction.
 *
 * Pseudo Code:
 *
 *   - Initialize the data (i.e. Instruction) packet to be sent serially
 *   - Send the packet serially
 *
 ******************************************************************************/
ReturnType FRAM_WriteDisable(void)
{
    CharStream char_stream_send;
    NMX_uint8  cWRDI = SPI_FRAM_WRITE_DISABLE;
    NMX_uint8 ucSR;
 
    // Step 1: Initialize the data (i.e. Instruction) packet to be sent serially
    char_stream_send.length = 1;
    char_stream_send.pChar  = &cWRDI;

    // Step 2: Send the packet serially
    Serialize_SPI(&char_stream_send, NULL_PTR, OpsWakeUp, OpsEndTransfer);

    // Step 3: Read the Status Register.
    do {
        FRAM_ReadStatusRegister(&ucSR);
    } while(ucSR & SPI_NAND_WE);

    return FRAM_Success;
}
/******************************************************************************
 *
 * Function:		FRAM_Read()
 * Arguments:		uAddrType udAddr, NMX_uint8 *pArray
 * Return Value:	FRAM_AddressInvalid, FRAM_Success
 * Description:
 *
 * The READ (03h) command transfers the data from the FRAM array to the
 * SPI out.
 * The READ command requires a 24-bit address consisting of 6 dummy bits followed
 * by a 18-bit address.  The upper six bits of the address are ignored.
 * After the opcode and address are issued, the device drives out
 * the read data on the next eight clocks. The SI input is ignored
 * during read data bytes. Subsequent bytes are data bytes, which
 * are read out sequentially. Addresses are incremented internally
 * as long as the bus master continues to issue clocks and CS is
 * LOW. If the last address of 3FFFFh is reached, the counter will
 * roll over to 00000h. Data is read MSB first. The rising edge of CS
 * terminates a read operation and tristates the SO pin.
 *
 *
 ******************************************************************************/
ReturnType FRAM_Read(uAddrType udAddr, NMX_uint8 *pArray, NMX_uint32 udNrOfElementsInArray)
{
    CharStream char_stream_send;
    CharStream char_stream_recv;
    NMX_uint8  chars[4];
	
    // Step 1: Validate address input
	if(udAddr >= FRAM_SIZE)
		return FRAM_AddressInvalid;

    // Step 2: Initialize the data (i.e. Instruction) packet to be sent serially
	Build_Address_Stream(udAddr, SPI_FRAM_READ_INS, chars);
    char_stream_send.length   = 4;
    char_stream_send.pChar    = chars;
    char_stream_recv.length   = udNrOfElementsInArray;
    char_stream_recv.pChar    = pArray;

    // Step 3: Send the packet serially, and fill the buffer with the data being returned
    Serialize_SPI(&char_stream_send, &char_stream_recv, OpsWakeUp, OpsEndTransfer);

    return FRAM_Success;
}

/******************************************************************************
 *
 * Function:		FRAM_Write()
 * Arguments:		uAddrType udAddr,
 * 					NMX_uint8 *pArray,
 * 					NMX_uint32 udNrOfElementsInArray
 * Return Value:	FRAM_AddressInvalid,
 * 					FRAM_Success,
 *
 * Description:
 *
 * All writes to the memory begin with a WREN opcode with CS
 * being asserted and deasserted. The next opcode is WRITE. The
 * WRITE opcode is followed by a three-byte address containing
 * the 18-bit address (A17-A0) of the first data byte to be written into
 * the memory. The upper six bits of the three-byte address are
 * ignored. Subsequent bytes are data bytes, which are written
 * sequentially. Addresses are incremented internally as long as
 * the bus master continues to issue clocks and keeps CS LOW. If
 * the last address of 3FFFFh is reached, the counter will roll over
 * to 00000h. Data is written MSB first.
 *
 * Pseudo code:
 *
 *    - Send Write enable Command
 *    - Send Write Command (0x02)
 *    - Send address (18bit)
 *    - Send data 
 *
 ******************************************************************************/
ReturnType FRAM_Write(uAddrType udAddr, NMX_uint8 *pArray, NMX_uint32 udNrOfElementsInArray)
{
    CharStream char_stream_send;
    NMX_uint8 chars[4];
    NMX_uint8 status_reg;

    // Step 1: Validate address input
	if(udAddr >= FRAM_SIZE)
		return FRAM_AddressInvalid;

    // Step 2: Disable Write protection
    FRAM_WriteEnable();

    // Step 3: Initialize the data (Instruction & address only) packet to be sent serially
	Build_Address_Stream(udAddr, SPI_FRAM_WRITE_INS, chars);
    char_stream_send.length   = 4;
    char_stream_send.pChar    = chars;

    // Step 5: Send the packet serially, and fill the buffer with the data being returned
    Serialize_SPI(&char_stream_send, NULL_PTR, OpsWakeUp, OpsInitTransfer);

    // Step 6: Initialize the data (data to be programmed) packet to be sent serially
    char_stream_send.length   = udNrOfElementsInArray;
    char_stream_send.pChar    = pArray;

    // Step 7: Send the packet (data to be programmed) serially
    Serialize_SPI(&char_stream_send, NULL_PTR, OpsWakeUp, OpsEndTransfer);
	    
    return FRAM_Success;
}

/******************************************************************************
 *
 * Function:		FRAM_ReadDeviceIdentification()
 * Arguments:		NMX_uint16 *uwpDeviceIdentification
 * Return Value:	FRAM_Success
 * Description:
 *
 * The RDID opcode 9Fh allows the user to read the manufacturer ID and product ID
 *
 ******************************************************************************/
ReturnType FRAM_ReadDeviceIdentification(NMX_uint8 *devArray)
{
    CharStream char_stream_send;
    CharStream char_stream_recv;
    NMX_uint8  cRead_ID;

	cRead_ID = (NMX_uint8) SPI_FRAM_READ_ID;
	
    // Step 1: Initialize the data (i.e. Instruction) packet to be sent serially
    char_stream_send.length = 1;
    char_stream_send.pChar  = &chars;
    char_stream_recv.length = 9;
    char_stream_recv.pChar  = devArray;

    // Step 2: Send the packet serially
    Serialize_SPI(&char_stream_send, &char_stream_recv, OpsWakeUp, OpsEndTransfer);

    return FRAM_Success;
}

/******************************************************************************
 *
 * Function:		FRAM_Unlock()
 * Arguments:		ProtectedRows pr
 * Return Value:	FRAM_Success
 * Description:
 * The BP1 and BP0 control the software write-protection features and are
 * nonvolatile bits. BP1 and BP0 are memory block write protection bits. They
 * specify portions of memory that are write-protected as shown:
 * 
 * Status register description:
 *
 * Bit7 - WPEN Write Protect Enable
 * Bit6 - reserved (1)
 * Bit5 - reserved (0)
 * Bit4 - reserved (0)
 * Bit3 - BP1 Block Protect bit '1'
 * Bit2 - BP0 Block Protect bit '0'
 * Bit1 - WEL Write enable
 * Bit0 - reserved (0)
 * 
 * (BP1, BP0)
 *   0    0    ->  None (all unlocked)
 *   0    1    ->  Upper 1/4 locked
 *   1    0    ->  Upper 1/2 locked
 *   1    1    ->  All locked
 *
 ******************************************************************************/
ReturnType FRAM_Unlock(ProtectedRows pr)
{
    CharStream char_stream_send;
	NMX_uint8 chars;

	chars = (NMX_uint8) pr << 2;

    // Step 1: Set the new value
    FRAM_WriteStatusRegister(&chars);

    return FRAM_Success;
}

/******************************************************************************
 *
 *  Function:		FRAM_Sleep()
 *  Arguments:		none
 *  Return Value:	FRAM_Success
 *  Description:
 *
 ******************************************************************************/
ReturnType FRAM_Sleep(void)
{
    CharStream char_stream_send;
    NMX_uint8  cSLP = SPI_FRAM_SLEEP;

    // Step 1: Initialize the data (i.e. Instruction) packet to be sent serially
    char_stream_send.length = 1;
    char_stream_send.pChar  = &cSLP;

    // Step 2: Send the packet serially
    Serialize_SPI(&char_stream_send, NULL_PTR, OpsWakeUp, OpsEndTransfer);
    
    return FRAM_Success;
}