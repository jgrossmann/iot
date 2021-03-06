/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : L3GD20.c
* Author             : MSH Application Team
* Author             : Andrea Labombarda
* Version            : $Revision:$
* Date               : $Date:$
* Description        : L3GD20 driver file
*                      
* HISTORY:
* Date               |	Modification                    |	Author
* 25/01/2012         |	Initial Revision                |	Andrea Labombarda
* 27/08/2012		 |  Modified to support multiple drivers in the same program                |	Antonio Micali
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOFTWARE IS SPECIFICALLY DESIGNED FOR EXCLUSIVE USE WITH ST PARTS.
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "l3gd20_driver.h"
#include "i2cm.h"
#include "cfa.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// Read operation to the lower level driver is 0.
#define I2C_SLAVE_OPERATION_READ                    (0)

// Write operation to the lower level driver is 1.
#define I2C_SLAVE_OPERATION_WRITE                   (1)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/*******************************************************************************
* Function Name		: L3GD20_ReadReg
* Description		: Generic Reading function. It must be fullfilled with either
*					: I2C or SPI reading functions					
* Input				: Register Address
* Output			: Data REad
* Return			: None
*******************************************************************************/
u8_t L3GD20_ReadReg(u8_t Reg, u8_t* Data) {

	CFA_BSC_STATUS read_status;

	read_status = cfa_bsc_OpExtended(Data, sizeof(u8_t), &Reg, sizeof(u8_t), L3GD20_MEMS_I2C_ADDRESS, I2C_SLAVE_OPERATION_READ);

    switch(read_status)
    {
        case CFA_BSC_STATUS_INCOMPLETE:
            // Transaction did not go through. ERROR. Handle this case.
        	return MEMS_ERROR;
            break;
        case CFA_BSC_STATUS_SUCCESS:
            // The read was successful.
        	return MEMS_SUCCESS;
            break;
        case CFA_BSC_STATUS_NO_ACK:
            // No slave device with this address exists on the I2C bus. ERROR. Handle this.
        default:
            break;
    }

    return MEMS_ERROR;
}

/*******************************************************************************
* Function Name		: L3GD20_WriteReg
* Description		: Generic Writing function. It must be fullfilled with either
*					: I2C or SPI writing function
* Input				: Register Address, Data to be written
* Output			: None
* Return			: None
*******************************************************************************/
u8_t L3GD20_WriteReg(u8_t Reg, u8_t Data) {
	CFA_BSC_STATUS read_status;
    UINT8 reg_data_bytes[2];

    reg_data_bytes[0]= Reg;
    reg_data_bytes[1] = Data;

    read_status = cfa_bsc_OpExtended(reg_data_bytes, sizeof(reg_data_bytes), NULL, 0, L3GD20_MEMS_I2C_ADDRESS, I2C_SLAVE_OPERATION_WRITE);

    switch(read_status)
    {
        case CFA_BSC_STATUS_INCOMPLETE:
            // Transaction did not go through. ERROR. Handle this case.
        	return MEMS_ERROR;
            break;
        case CFA_BSC_STATUS_SUCCESS:
            // The read was successful.
        	return MEMS_SUCCESS;
            break;
        case CFA_BSC_STATUS_NO_ACK:
            // No slave device with this address exists on the I2C bus. ERROR. Handle this.
        default:
            break;
    }

    return MEMS_ERROR;
}
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : L3GD20_SetODR
* Description    : Sets L3GD20 Output Data Rate
* Input          : Output Data Rate
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetODR(L3GD20_ODR_t ov){
  u8_t value;

  if( !L3GD20_ReadReg(L3GD20_CTRL_REG1, &value) )
    return MEMS_ERROR;

  value &= 0x0f;
  value |= ov<<4;

  if( !L3GD20_WriteReg(L3GD20_CTRL_REG1, value) )
    return MEMS_ERROR;

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetMode
* Description    : Sets L3GD20 Operating Mode
* Input          : Modality (NORMAL, SLEEP, POWER_DOWN)
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetMode(L3GD20_Mode_t md) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG1, &value) )
    return MEMS_ERROR;
                  
  switch(md) {
  
  case L3GD20_POWER_DOWN:		
    value &= 0xF7;
    value |= (MEMS_RESET<<L3GD20_PD);
    break;
          
  case L3GD20_NORMAL:
    value &= 0xF7;
    value |= (MEMS_SET<<L3GD20_PD);
    break;
          
  case L3GD20_SLEEP:		
    value &= 0xF0;
    value |= ( (MEMS_SET<<L3GD20_PD) | (MEMS_RESET<<L3GD20_ZEN) | (MEMS_RESET<<L3GD20_YEN) | (MEMS_RESET<<L3GD20_XEN) );
    break;
          
  default:
    return MEMS_ERROR;
  }
  
  if( !L3GD20_WriteReg(L3GD20_CTRL_REG1, value) )
    return MEMS_ERROR;
                  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetAxis
* Description    : Enable/Disable L3GD20 Axis
* Input          : X_ENABLE/X_DISABLE | Y_ENABLE/Y_DISABLE | Z_ENABLE/Z_DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetAxis(L3GD20_Axis_t axis) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG1, &value) )
    return MEMS_ERROR;
    
  value &= 0xf8;
  value |= axis;
  
  if( !L3GD20_WriteReg(L3GD20_CTRL_REG1, value) )
    return MEMS_ERROR;  
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetFullScale
* Description    : Sets the L3GD20 FullScale
* Input          : FS_250/FS_500/FS_2000
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetFullScale(L3GD20_Fullscale_t fs) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG4, &value) )
    return MEMS_ERROR;
                  
  value &= 0xCF;	
  value |= (fs<<L3GD20_FS);
  
  if( !L3GD20_WriteReg(L3GD20_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetBDU
* Description    : Enable/Disable Block Data Update Functionality
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetBDU(State_t bdu) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG4, &value) )
    return MEMS_ERROR;
 
  value &= 0x7F;
  value |= (bdu<<L3GD20_BDU);

  if( !L3GD20_WriteReg(L3GD20_CTRL_REG4, value) )
    return MEMS_ERROR;

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetBLE
* Description    : Set Endianess (MSB/LSB)
* Input          : BLE_LSB / BLE_MSB
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetBLE(L3GD20_Endianess_t ble) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG4, &value) )
    return MEMS_ERROR;
                  
  value &= 0xBF;	
  value |= (ble<<L3GD20_BLE);
  
  if( !L3GD20_WriteReg(L3GD20_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_HPFEnable
* Description    : Enable/Disable High Pass Filter
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_HPFEnable(State_t hpf) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG5, &value) )
    return MEMS_ERROR;
                  
  value &= 0xEF;
  value |= (hpf<<L3GD20_HPEN);
  
  if( !L3GD20_WriteReg(L3GD20_CTRL_REG5, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}

/*******************************************************************************
* Function Name  : L3GD20_HPFEnable
* Description    : Enable/Disable High Pass Filter
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_BootMemEnable(State_t hpf) {
  u8_t value;

  if( !L3GD20_ReadReg(L3GD20_CTRL_REG5, &value) )
    return MEMS_ERROR;

  value &= (1 << 7);
  value |= (hpf<<L3GD20_BOOT_MEM);

  if( !L3GD20_WriteReg(L3GD20_CTRL_REG5, value) )
    return MEMS_ERROR;

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetHPFMode
* Description    : Set High Pass Filter Modality
* Input          : HPM_NORMAL_MODE_RES/HPM_REF_SIGNAL/HPM_NORMAL_MODE/HPM_AUTORESET_INT
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetHPFMode(L3GD20_HPFMode_t hpf) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG2, &value) )
    return MEMS_ERROR;
                  
  value &= 0xCF;
  value |= (hpf<<L3GD20_HPM);
  
  if( !L3GD20_WriteReg(L3GD20_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetHPFCutOFF
* Description    : Set High Pass CUT OFF Freq
* Input          : HPFCF_0,HPFCF_1,HPFCF_2... See Table 27 of the datasheet
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetHPFCutOFF(L3GD20_HPFCutOffFreq_t hpf) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG2, &value) )
    return MEMS_ERROR;
                  
  value &= 0xF0;
  value |= (hpf<<L3GD20_HPFC0);
  
  if( !L3GD20_WriteReg(L3GD20_CTRL_REG2, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : L3GD20_SetIntPinMode
* Description    : Set Interrupt Pin Modality (push pull or Open drain)
* Input          : PUSH_PULL/OPEN_DRAIN
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetIntPinMode(L3GD20_IntPinMode_t pm) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG3, &value) )
    return MEMS_ERROR;
                  
  value &= 0xEF;
  value |= (pm<<L3GD20_PP_OD);
  
  if( !L3GD20_WriteReg(L3GD20_CTRL_REG3, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetInt1Pin
* Description    : Set Interrupt1 pin Function
* Input          : L3GD20_I1_ON_PIN_INT1_ENABLE | L3GD20_I1_BOOT_ON_INT1 | L3GD20_INT1_ACTIVE_HIGH
* example        : L3GD20_SetInt1Pin(L3GD20_I1_ON_PIN_INT1_ENABLE | L3GD20_I1_BOOT_ON_INT1_ENABLE | L3GD20_INT1_ACTIVE_LOW) 
* to enable Interrupt 1 or Bootsatus on interrupt 1 pin
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetInt1Pin(L3GD20_Int1PinConf_t pinConf) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG3, &value) )
    return MEMS_ERROR;
                  
  value &= 0x1F;
  value |= pinConf;
  
  if( !L3GD20_WriteReg(L3GD20_CTRL_REG3, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetInt2Pin
* Description    : Set Interrupt2 pin Function
* Input          : L3GD20_I2_DRDY_ON_INT2_ENABLE/DISABLE | 
                   L3GD20_WTM_ON_INT2_ENABLE/DISABLE | 
                   L3GD20_OVERRUN_ON_INT2_ENABLE/DISABLE | 
                   L3GD20_EMPTY_ON_INT2_ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetInt2Pin(L3GD20_Int2PinConf_t pinConf) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG3, &value) )
    return MEMS_ERROR;
                  
  value &= 0xF0;
  value |= pinConf;
  
  if( !L3GD20_WriteReg(L3GD20_CTRL_REG3, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : L3GD20_Int1LatchEnable
* Description    : Enable Interrupt 1 Latching function
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_Int1LatchEnable(State_t latch) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_INT1_CFG, &value) )
    return MEMS_ERROR;
                  
  value &= 0xBF;
  value |= latch<<L3GD20_LIR;
  
  if( !L3GD20_WriteReg(L3GD20_INT1_CFG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_ResetInt1Latch
* Description    : Reset Interrupt 1 Latching function
* Input          : None
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_ResetInt1Latch(void) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_INT1_SRC, &value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetIntConfiguration
* Description    : Interrupt 1 Configuration
* Input          : AND/OR, INT1_LIR ZHIE_ENABLE/DISABLE | INT1_ZLIE_ENABLE/DISABLE...
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetIntConfiguration(L3GD20_Int1Conf_t ic) {
  u8_t value;
  
  value = ic;

  if( !L3GD20_WriteReg(L3GD20_INT1_CFG, value) )
    return MEMS_ERROR;

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetInt1Threshold
* Description    : Sets Interrupt 1 Threshold
* Input          : Threshold = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetInt1Threshold(L3GD20_IntThsAxis axis, u16_t ths) {
  u8_t value;
  
  switch (axis) {
    
    case L3GD20_THS_X:
      //write the threshold LSB
      value = (u8_t)( ths & 0x00ff); 
      if( !L3GD20_WriteReg(L3GD20_INT1_THS_XL, value) )
        return MEMS_ERROR;
      
      //write the threshold LSB
      value = (u8_t)( ths >> 8); 
      if( !L3GD20_WriteReg(L3GD20_INT1_THS_XH, value) )
        return MEMS_ERROR;
      
      break;
      
    case L3GD20_THS_Y:
      //write the threshold LSB
      value = (u8_t)( ths & 0x00ff); 
      if( !L3GD20_WriteReg(L3GD20_INT1_THS_YL, value) )
        return MEMS_ERROR;
      
      //write the threshold LSB
      value = (u8_t)( ths >> 8); 
      if( !L3GD20_WriteReg(L3GD20_INT1_THS_YH, value) )
        return MEMS_ERROR;
      
      break;
      
    case L3GD20_THS_Z:
      //write the threshold LSB
      value = (u8_t)( ths & 0x00ff); 
      if( !L3GD20_WriteReg(L3GD20_INT1_THS_ZL, value) )
        return MEMS_ERROR;
      
      //write the threshold LSB
      value = (u8_t)( ths >> 8); 
      if( !L3GD20_WriteReg(L3GD20_INT1_THS_ZH, value) )
        return MEMS_ERROR;
      
      break;     

        
  }

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetInt1Duration
* Description    : Sets Interrupt 1 Duration
* Input          : Duration value
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetInt1Duration(L3GD20_Int1Conf_t id) {
 
  if (id > 127)
    return MEMS_ERROR;

  if( !L3GD20_WriteReg(L3GD20_INT1_DURATION, id) )
    return MEMS_ERROR;

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_FIFOModeEnable
* Description    : Sets Fifo Modality
* Input          : 
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_FIFOModeEnable(L3GD20_FifoMode_t fm) {
  u8_t value;  
  
  if(fm == L3GD20_FIFO_DISABLE) {
    
    if( !L3GD20_ReadReg(L3GD20_CTRL_REG5, &value) )
      return MEMS_ERROR;
                    
    value &= 0xBF;    
    
    if( !L3GD20_WriteReg(L3GD20_CTRL_REG5, value) )
      return MEMS_ERROR;
    
  }
  else {
    
    if( !L3GD20_ReadReg(L3GD20_CTRL_REG5, &value) )
      return MEMS_ERROR;
                    
    value &= 0xBF;
    value |= MEMS_SET<<L3GD20_FIFO_EN;
    
    if( !L3GD20_WriteReg(L3GD20_CTRL_REG5, value) )
      return MEMS_ERROR;
    
    
    if( !L3GD20_ReadReg(L3GD20_FIFO_CTRL_REG, &value) )
      return MEMS_ERROR;
    
    value &= 0x1f;
    value |= (fm<<L3GD20_FM0);
    
    if( !L3GD20_WriteReg(L3GD20_FIFO_CTRL_REG, value) )
      return MEMS_ERROR;
  }

  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetWaterMark
* Description    : Sets Watermark Value
* Input          : Watermark = [0,31]
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetWaterMark(u8_t wtm) {
  u8_t value;
  
  if(wtm > 31)
    return MEMS_ERROR;  
  
  if( !L3GD20_ReadReg(L3GD20_FIFO_CTRL_REG, &value) )
    return MEMS_ERROR;
                  
  value &= 0xE0;
  value |= wtm; 
  
  if( !L3GD20_WriteReg(L3GD20_FIFO_CTRL_REG, value) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_GetSatusReg
* Description    : Read the status register
* Input          : None
* Output         : status register buffer
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_GetSatusReg(u8_t* buff) {
  if( !L3GD20_ReadReg(L3GD20_STATUS_REG, buff) )
      return MEMS_ERROR;
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : L3GD20_GetAngRateRaw
* Description    : Read the Angular Rate Registers
* Input          : None
* Output         : Angular Rate Registers buffer
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_GetAngRateRaw(AxesRaw_t* buff) {
  u8_t valueL;
  u8_t valueH;
  

  if( !L3GD20_ReadReg(L3GD20_OUT_X_L, &valueL) )
      return MEMS_ERROR;
  
  if( !L3GD20_ReadReg(L3GD20_OUT_X_H, &valueH) )
      return MEMS_ERROR;
  
  buff->AXIS_X = (i16_t)( (valueH << 8) | valueL );
  
  if( !L3GD20_ReadReg(L3GD20_OUT_Y_L, &valueL) )
      return MEMS_ERROR;
  
  if( !L3GD20_ReadReg(L3GD20_OUT_Y_H, &valueH) )
      return MEMS_ERROR;
  
  buff->AXIS_Y = (i16_t)( (valueH << 8) | valueL );
  
   if( !L3GD20_ReadReg(L3GD20_OUT_Z_L, &valueL) )
      return MEMS_ERROR;
  
  if( !L3GD20_ReadReg(L3GD20_OUT_Z_H, &valueH) )
      return MEMS_ERROR;
  
  buff->AXIS_Z = (i16_t)( (valueH << 8) | valueL );
  
  return MEMS_SUCCESS;  
}


/*******************************************************************************
* Function Name  : L3GD20_GetInt1Src
* Description    : Reset Interrupt 1 Latching function
* Input          : None
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_GetInt1Src(u8_t* buff) {
  
  if( !L3GD20_ReadReg(L3GD20_INT1_SRC, buff) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_GetFifoSourceReg
* Description    : Read Fifo source Register
* Input          : None
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_GetFifoSourceReg(u8_t* buff) {
  
  if( !L3GD20_ReadReg(L3GD20_FIFO_SRC_REG, buff) )
    return MEMS_ERROR;
  
  return MEMS_SUCCESS;
}


/*******************************************************************************
* Function Name  : L3GD20_SetOutputDataAndFifoFilters
* Description    : ENABLE/DISABLE HIGH PASS and LOW PASS filters applied to output and fifo registers
*                : See Table 8 of AN3393 for more details
* Input          : L3GD20_NONE, L3GD20_HPH, L3GD20_LPF2, L3GD20_HPFLPF2
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetOutputDataAndFifoFilters(L3GD20_HPF_LPF2_Enable hpf){
  u8_t value;
  
  //HPF
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG5, &value) )
    return MEMS_ERROR;
  
  switch(hpf) {
    
  case L3GD20_NONE:
    value &= 0xfc;
    value |= 0x00; //hpen = x, Out_sel_1 = 0, Out_sel_0 = 0
    break;
    
  case L3GD20_HPF:
    value &= 0xfc;
    value |= 0x01; //hpen = x, Out_sel_1 = 0, Out_sel_0 = 1
    break;

  case L3GD20_LPF2:
    value &= 0xed;
    value |= 0x02; //hpen = 0, Out_sel_1 = 1, Out_sel_0 = x
    break;    
   
  case L3GD20_HPFLPF2:
    value &= 0xed;
    value |= 0x12; //hpen = 1, Out_sel_1 = 1, Out_sel_0 = x
    break;    
  }
  
  if( !L3GD20_WriteReg(L3GD20_CTRL_REG5, value) )
    return MEMS_ERROR;
  
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : L3GD20_SetInt1Filters
* Description    : ENABLE/DISABLE HIGH PASS and LOW PASS filters applied to Int1 circuitery
*                : See Table 9 of AN3393 for more details
* Input          : NONE, HPH, LPF2, HPFLPF2
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetInt1Filters(L3GD20_HPF_LPF2_Enable hpf){
  u8_t value;
  
  //HPF
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG5, &value) )
    return MEMS_ERROR;
  
  switch(hpf) {
    
  case L3GD20_NONE:
    value &= 0xf3;
    value |= 0x00<<L3GD20_INT1_SEL0; //hpen = x, Int1_sel_1 = 0, Int1_sel_0 = 0
    break;
    
  case L3GD20_HPF:
    value &= 0xf3;
    value |= 0x01<<L3GD20_INT1_SEL0; //hpen = x, Int1_sel_1 = 0, Int1_sel_0 = 1
    break;

  case L3GD20_LPF2:
    value &= 0xe7;
    value |= 0x02<<L3GD20_INT1_SEL0; //hpen = 0, Int1_sel_1 = 1, Int1_sel_0 = x
    break;    
   
  case L3GD20_HPFLPF2:
    value &= 0xe7;
    value |= 0x01<<L3GD20_HPEN;
    value |= 0x02<<L3GD20_INT1_SEL0; //hpen = 1, Int1_sel_1 = 1, Int1_sel_0 = x
    break;    
  }
  
  if( !L3GD20_WriteReg(L3GD20_CTRL_REG5, value) )
    return MEMS_ERROR;
  
  
  return MEMS_SUCCESS;
  
}


/*******************************************************************************
* Function Name  : L3GD20_SetSPIInterface
* Description    : Set SPI mode: 3 Wire Interface OR 4 Wire Interface
* Input          : L3GD20_SPI_3_WIRE, L3GD20_SPI_4_WIRE
* Output         : None
* Return         : Status [MEMS_ERROR, MEMS_SUCCESS]
*******************************************************************************/
status_t L3GD20_SetSPIInterface(L3GD20_SPIMode_t spi) {
  u8_t value;
  
  if( !L3GD20_ReadReg(L3GD20_CTRL_REG4, &value) )
    return MEMS_ERROR;
                  
  value &= 0xFE;
  value |= spi<<L3GD20_SIM;
  
  if( !L3GD20_WriteReg(L3GD20_CTRL_REG4, value) )
    return MEMS_ERROR;
  
  
  return MEMS_SUCCESS;
}
