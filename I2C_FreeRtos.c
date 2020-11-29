#include "I2C_FreeRtos.h"

typedef struct
{
	uint8_t is_init;
	i2c_master_handle_t fsl_i2c_master_handle;
	SemaphoreHandle_t mutex; /** Same mutex to transfer and receive*/
	SemaphoreHandle_t semphr; /** Same semphr to transfer and receive*/
} freertos_i2c_hanlde_t;

static freertos_i2c_hanlde_t freertos_i2c_handles[NUMBER_OF_SERIAL_PORTS] = {0};

/** Enables I2C clock */
static inline void freertos_enable_i2c_clock(freertos_i2c_number_t i2c_number);
/**Enables port clock*/
static inline void freertos_i2c_enable_port_clock(freertos_i2c_port_t port);
/**Gets Port to use I2C*/
static inline PORT_Type * freertos_i2c_get_port_base(freertos_i2c_port_t port);
/**Gets the I2C to use*/
static inline I2C_Type * freertos_i2c_get_i2c_base(freertos_i2c_number_t i2c_number );

/**Callback function to determinate when I2C is finished*/
static void fsl_i2c_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData);

freertos_i2c_flag_t freertos_i2c_init(freertos_i2c_config_t config)
{
	freertos_i2c_flag_t retval = freertos_i2c_fail;
	i2c_master_config_t  fsl_i2c_config;

	if(config.i2c_number < NUMBER_OF_SERIAL_PORTS)
	{
		if(!freertos_i2c_handles[config.i2c_number].is_init)
		{
			freertos_i2c_handles[config.i2c_number].mutex = xSemaphoreCreateMutex();
			freertos_i2c_handles[config.i2c_number].semphr = xSemaphoreCreateBinary();

			/* Clock Enable */
			freertos_i2c_enable_port_clock(config.port);
			freertos_enable_i2c_clock(config.i2c_number);

			/* Port sets */
			PORT_SetPinMux(freertos_i2c_get_port_base(config.port), config.SCL, config.pin_mux);
			PORT_SetPinMux(freertos_i2c_get_port_base(config.port), config.SDA, config.pin_mux);

			I2C_MasterGetDefaultConfig(&fsl_i2c_config);
			fsl_i2c_config.baudRate_Bps = config.baudrate;


			if(freertos_i2c0 == config.i2c_number)
			{
				I2C_MasterInit(freertos_i2c_get_i2c_base(freertos_i2c0), &fsl_i2c_config, CLOCK_GetFreq(I2C0_CLK_SRC));
				NVIC_SetPriority(I2C0_IRQn,5);
			}
			else if(freertos_i2c1 == config.i2c_number)
			{
				I2C_MasterInit(freertos_i2c_get_i2c_base(freertos_i2c1), &fsl_i2c_config, CLOCK_GetFreq(I2C1_CLK_SRC));
				NVIC_SetPriority(I2C1_IRQn,5);
			}
			else if(freertos_i2c2 == config.i2c_number)
			{
				I2C_MasterInit(freertos_i2c_get_i2c_base(freertos_i2c2), &fsl_i2c_config, CLOCK_GetFreq(I2C2_CLK_SRC));
				NVIC_SetPriority(I2C2_IRQn,5);
			}

			I2C_MasterTransferCreateHandle(freertos_i2c_get_i2c_base(config.i2c_number), &freertos_i2c_handles[config.i2c_number].fsl_i2c_master_handle, fsl_i2c_callback, NULL);

			freertos_i2c_handles[config.i2c_number].is_init = 1;

			retval = freertos_i2c_sucess;
		}
	}
	return retval;
}

freertos_i2c_flag_t freertos_i2c_transfer(freertos_i2c_number_t i2c_number, uint8_t * buffer, uint16_t length, uint8_t slaveAddress, uint32_t subaddress, uint8_t subaddressSize)
{
	freertos_i2c_flag_t flag = freertos_i2c_fail;
	i2c_master_transfer_t transfer;

	if(freertos_i2c_handles[i2c_number].is_init)
	{
		transfer.flags = kI2C_TransferDefaultFlag; /** default flag for transference */
		transfer.slaveAddress = slaveAddress;
		transfer.direction = kI2C_Write; /** We want to transfer */
		transfer.subaddress = subaddress;
		transfer.subaddressSize = subaddressSize;
		transfer.data = buffer;
		transfer.dataSize = length;

		xSemaphoreTake(freertos_i2c_handles[i2c_number].mutex, portMAX_DELAY);

		I2C_MasterTransferNonBlocking(freertos_i2c_get_i2c_base(i2c_number), &freertos_i2c_handles[i2c_number].fsl_i2c_master_handle, &transfer);

		xSemaphoreTake(freertos_i2c_handles[i2c_number].semphr, portMAX_DELAY);

		xSemaphoreGive(freertos_i2c_handles[i2c_number].mutex);

		flag = freertos_i2c_sucess;
	}

	return flag;
}

freertos_i2c_flag_t freertos_i2c_receive(freertos_i2c_number_t i2c_number, uint8_t * buffer, uint16_t length, uint8_t slaveAddress, uint32_t subaddress, uint8_t subaddressSize)
{
	freertos_i2c_flag_t flag = freertos_i2c_fail;
	i2c_master_transfer_t transfer;

	if(freertos_i2c_handles[i2c_number].is_init)
	{
		flag = freertos_i2c_sucess;

		/** Se realiza el Read que se buscaba */
		transfer.flags = kI2C_TransferDefaultFlag; /** default flag for transference */
		transfer.slaveAddress = slaveAddress;
		transfer.direction = kI2C_Read; /** We want to recieve */
		transfer.subaddress = subaddress;
		transfer.subaddressSize = subaddressSize;
		transfer.data = buffer;
		transfer.dataSize = length;


		xSemaphoreTake(freertos_i2c_handles[i2c_number].mutex, portMAX_DELAY);

		I2C_MasterTransferNonBlocking(freertos_i2c_get_i2c_base(i2c_number), &freertos_i2c_handles[i2c_number].fsl_i2c_master_handle, &transfer);

		xSemaphoreTake(freertos_i2c_handles[i2c_number].semphr, portMAX_DELAY);

		xSemaphoreGive(freertos_i2c_handles[i2c_number].mutex);

		flag = freertos_i2c_sucess;
	}

	return flag;
}

static inline void freertos_enable_i2c_clock(freertos_i2c_number_t i2c_number)
{
	switch(i2c_number)
	{
	case freertos_i2c0:
		CLOCK_EnableClock(kCLOCK_I2c0);
		break;
		/**I2C 1*/
	case freertos_i2c1:
		CLOCK_EnableClock(kCLOCK_I2c1);
		break;
		/**I2C 2*/
	case freertos_i2c2:
		CLOCK_EnableClock(kCLOCK_I2c2);
		break;
	case freertos_i2c3:
		CLOCK_EnableClock(kCLOCK_I2c3);
		break;
	}
}

static inline void freertos_i2c_enable_port_clock(freertos_i2c_port_t port)
{
	switch(port)
	{
	case freertos_i2c_portA:
		CLOCK_EnableClock(kCLOCK_PortA);
		break;
	case freertos_i2c_portB:
		CLOCK_EnableClock(kCLOCK_PortB);
		break;
	case freertos_i2c_portC:
		CLOCK_EnableClock(kCLOCK_PortC);
		break;
	case freertos_i2c_portD:
		CLOCK_EnableClock(kCLOCK_PortD);
		break;
	case freertos_i2c_portE:
		CLOCK_EnableClock(kCLOCK_PortE);
		break;
	}
}

static inline PORT_Type * freertos_i2c_get_port_base(freertos_i2c_port_t port)
{
	PORT_Type * port_base = PORTA;

	switch(port)
	{
	case freertos_i2c_portA:
		port_base = PORTA;
		break;
	case freertos_i2c_portB:
		port_base = PORTB;
		break;
	case freertos_i2c_portC:
		port_base = PORTC;
		break;
	case freertos_i2c_portD:
		port_base = PORTD;
		break;
	case freertos_i2c_portE:
		port_base = PORTE;
		break;
	}

	return port_base;
}

static inline I2C_Type * freertos_i2c_get_i2c_base(freertos_i2c_number_t i2c_number )
{
	I2C_Type * retval = I2C0;

	switch(i2c_number)
	{
	case freertos_i2c0:
		retval = I2C0;
		break;
	case freertos_i2c1:
		retval = I2C1;
		break;
	case freertos_i2c2:
		retval = I2C2;
		break;
	case freertos_i2c3:
		retval = I2C3;
		break;
	}

	return retval;
}

static void fsl_i2c_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (kStatus_Success  == status)
	{
		if (I2C0 == base)
		{
			xSemaphoreGiveFromISR(freertos_i2c_handles[freertos_i2c0].semphr, &xHigherPriorityTaskWoken);
		}
		else if(I2C1 == base)
		{
			xSemaphoreGiveFromISR(freertos_i2c_handles[freertos_i2c1].semphr, &xHigherPriorityTaskWoken);
		}
		else if(I2C2 == base)
		{
			xSemaphoreGiveFromISR(freertos_i2c_handles[freertos_i2c2].semphr, &xHigherPriorityTaskWoken);
		}
	}

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}





