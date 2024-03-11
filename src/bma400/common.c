/**
 * Copyright (C) 2020 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
// #include <zephyr/zephyr.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include "bma400.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bma400_common);
#define I2C_DEV DT_LABEL(DT_ALIAS(i2c0))
#define FRAM_I2C_ADDR BMA400_I2C_ADDRESS_SDO_LOW
#define READ_WRITE_LENGTH 46

static const struct device *i2c_dev;
static uint8_t dev_addr;

static int write_bytes(const struct device *i2c_dev, uint16_t addr,
                       uint8_t *data, uint32_t num_bytes)
{
    uint8_t wr_addr[1];
    struct i2c_msg msgs[2];

    /* FRAM address */
    wr_addr[0] = (addr >> 8) & 0xFF;
    // wr_addr[1] = addr & 0xFF;

    /* Setup I2C messages */

    /* Send the address to write to */
    msgs[0].buf = wr_addr;
    msgs[0].len = 2U;
    msgs[0].flags = I2C_MSG_WRITE;

    /* Data to be written, and STOP after this. */
    msgs[1].buf = data;
    msgs[1].len = num_bytes;
    msgs[1].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

    return i2c_transfer(i2c_dev, &msgs[0], 2, FRAM_I2C_ADDR);
}

static int read_bytes(const struct device *i2c_dev, uint16_t addr,
                      uint8_t *data, uint32_t num_bytes)
{
    uint8_t wr_addr[1];
    struct i2c_msg msgs[2];

    /* Now try to read back from FRAM */

    /* FRAM address */
    wr_addr[0] = (addr >> 8) & 0xFF;
    // wr_addr[1] = addr & 0xFF;

    /* Setup I2C messages */

    /* Send the address to read from */
    msgs[0].buf = wr_addr;
    msgs[0].len = 2U;
    msgs[0].flags = I2C_MSG_WRITE;

    /* Read from device. STOP after this. */
    msgs[1].buf = data;
    msgs[1].len = num_bytes;
    msgs[1].flags = I2C_MSG_READ | I2C_MSG_STOP;

    return i2c_transfer(i2c_dev, &msgs[0], 2, FRAM_I2C_ADDR);
}

static void i2c_master_init(void)
{
    i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));
    if (!device_is_ready(i2c_dev))
    {
        LOG_ERR("I2C: Device is not ready.");
        // return;
    }
}

BMA400_INTF_RET_TYPE bma400_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    int err = 0;
    // err = read_bytes(i2c_dev, &reg_addr, reg_data, len);
   err =  i2c_burst_read(i2c_dev,BMA400_I2C_ADDRESS_SDO_LOW, reg_addr, reg_data, len);
    return err;
}

BMA400_INTF_RET_TYPE bma400_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    int err = 0;
    // err = write_bytes(i2c_dev, &reg_addr, reg_data, len);
   err =  i2c_burst_write(i2c_dev,BMA400_I2C_ADDRESS_SDO_LOW, reg_addr,reg_data, len);

    return err;
}

void bma400_delay_us(uint32_t period, void *intf_ptr)
{
   k_sleep(K_MSEC(period));
}

void bma400_check_rslt(const char api_name[], int8_t rslt)
{
    switch (rslt)
    {
    case BMA400_OK:

        /* Do nothing */
        break;
    case BMA400_E_NULL_PTR:
        printf("Error [%d] : Null pointer\r\n", rslt);
        break;
    case BMA400_E_COM_FAIL:
        printf("Error [%d] : Communication failure\r\n", rslt);
        break;
    case BMA400_E_INVALID_CONFIG:
        printf("Error [%d] : Invalid configuration\r\n", rslt);
        break;
    case BMA400_E_DEV_NOT_FOUND:
        printf("Error [%d] : Device not found\r\n", rslt);
        break;
    default:
        printf("Error [%d] : Unknown error code\r\n", rslt);
        break;
    }
}

int8_t bma400_interface_init(struct bma400_dev *bma400, uint8_t intf)
{
    int8_t rslt = BMA400_OK;
    i2c_master_init();
    if (bma400 != NULL)
    {
        k_sleep(K_MSEC(100));
        if (intf == BMA400_I2C_INTF)
        {
            printf("I2C Interface \n");

            dev_addr = BMA400_I2C_ADDRESS_SDO_LOW;
            bma400->read = bma400_i2c_read;
            bma400->write = bma400_i2c_write;
            bma400->intf = BMA400_I2C_INTF;
        }

        bma400->intf_ptr = &dev_addr;
        bma400->delay_us = bma400_delay_us;
        bma400->read_write_len = READ_WRITE_LENGTH;
    }
    else
    {
        rslt = BMA400_E_NULL_PTR;
    }
    return rslt;
}
