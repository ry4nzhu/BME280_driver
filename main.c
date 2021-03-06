#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include "stdlib.h"
#include "drivers/mss_i2c/mss_i2c.h"
#include "bme280.h"

int delayTime = 1000;

void user_delay_ms(uint32_t period)
{
    /*
     * Return control or wait,
     * for a period amount of milliseconds
     */
	 volatile int i = 0;
	 while (i < 10000 * period) {
	 	i++;
	 }

	return;
}

int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

    /*
     * The parameter dev_id can be used as a variable to store the I2C address of the device
     */
    mss_i2c_status_t status;

    MSS_I2C_write(&g_mss_i2c1, BME280_I2C_ADDR_PRIM, &reg_addr, sizeof(reg_addr), MSS_I2C_RELEASE_BUS);
    MSS_I2C_wait_complete(&g_mss_i2c1, MSS_I2C_NO_TIMEOUT);
    MSS_I2C_read( &g_mss_i2c1, BME280_I2C_ADDR_PRIM, reg_data, len, MSS_I2C_RELEASE_BUS );
    status = MSS_I2C_wait_complete( &g_mss_i2c1, MSS_I2C_NO_TIMEOUT );

    return rslt;
}

int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */
   // uint8_t* arr = malloc((len + 1) * sizeof(uint8_t));
   // arr[0] = reg_addr;
   // while (i < len) {
   // 	arr[i+1] = *(reg_data + i);
   // 	i++;
   // }
    //free(arr);
	uint8_t arr[2] = {reg_addr, *reg_data};

	MSS_I2C_write(&g_mss_i2c1, BME280_I2C_ADDR_PRIM, arr, sizeof(arr), MSS_I2C_RELEASE_BUS);
    MSS_I2C_wait_complete( &g_mss_i2c1, MSS_I2C_NO_TIMEOUT);

    return rslt;
}

void print_sensor_data(struct bme280_data *comp_data)
{
#ifdef BME280_FLOAT_ENABLE
  printf("temp %0.2f, p %0.2f, hum %0.2f\r\n",comp_data->temperature, comp_data->pressure, comp_data->humidity);
#else
  printf("temp %ld, p %ld, hum %ld\r\n",comp_data->temperature, comp_data->pressure, comp_data->humidity);
#endif
}

int main()
{
	struct bme280_dev dev;
	struct bme280_data comp_data;
	int8_t rslt = BME280_OK;
	uint8_t settings_sel;

	dev.dev_id = BME280_I2C_ADDR_PRIM;
	dev.intf = BME280_I2C_INTF;
	dev.read = user_i2c_read;
	dev.write = user_i2c_write;
	dev.delay_ms = user_delay_ms;

    // Initialize MSS I2C peripheral
    MSS_I2C_init( &g_mss_i2c1, dev.dev_id, MSS_I2C_PCLK_DIV_256 );

	rslt = bme280_init(&dev);
	if (rslt == 0) {
		printf("success init!\r\n");
	}

	/* Test for normal mode */
	dev.settings.osr_h = BME280_OVERSAMPLING_1X;
	dev.settings.osr_p = BME280_OVERSAMPLING_1X;
	dev.settings.osr_t = BME280_OVERSAMPLING_1X;
	dev.settings.filter = BME280_FILTER_COEFF_OFF;
	dev.settings.standby_time = BME280_STANDBY_TIME_250_MS;

	settings_sel = BME280_ALL_SETTINGS_SEL;

	rslt = bme280_set_sensor_settings(settings_sel, &dev);

	// set the sensor to normal mode
	rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, &dev);

	printf("Temperature, Pressure, Humidity\r\n");
	/* Continuously stream sensor data */
	while (1) {
		/* Wait for the measurement to complete and print data @25Hz */
		dev.delay_ms(40);
		rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
		print_sensor_data(&comp_data);
	}

	/* Suggested Weather / Climate Monitor mode */
	dev.settings.osr_h = BME280_OVERSAMPLING_1X;
	dev.settings.osr_p = BME280_OVERSAMPLING_1X;
	dev.settings.osr_t = BME280_OVERSAMPLING_1X;
	dev.settings.filter = BME280_FILTER_COEFF_OFF;

	rslt = bme280_set_sensor_settings((uint8_t)0x0F, &dev);

	printf("Temperature, Pressure, Humidity\r\n");
	/* Continuously stream sensor data */
	while (1) {
	rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
	/* Wait for the measurement to complete and print data @25Hz */
	dev.delay_ms(40);
	rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
	print_sensor_data(&comp_data);
	}




//	while (1) {
//		//bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
//		bme280_get_sensor_data(BME280_TEMP, &d, &dev);
//		printf("Temp: %lu\r\n", d.temperature);
//	}

	/*
	uint8_t addr = 0b1110110;
	uint8_t read = 0xD0;
	uint8_t write = 0xD0;
	uint8_t receive[10];
    MSS_I2C_init( &g_mss_i2c1, addr, MSS_I2C_PCLK_DIV_256 );
	MSS_I2C_write
	(
		&g_mss_i2c1,
		addr,
		&write,
		sizeof(write),
		MSS_I2C_RELEASE_BUS
	);
	MSS_I2C_wait_complete(&g_mss_i2c1, MSS_I2C_NO_TIMEOUT);
    //MSS_I2C_init( &g_mss_i2c1, read, MSS_I2C_PCLK_DIV_256 );
	MSS_I2C_read
	(
	    	&g_mss_i2c1,
	    	addr,
	    	receive,
	    	sizeof(receive),
	    	MSS_I2C_RELEASE_BUS
	 );
	MSS_I2C_wait_complete(&g_mss_i2c1, MSS_I2C_NO_TIMEOUT);
	//printf("Temp: %lu\r\n", receive);
	 */
	return 0;
}
