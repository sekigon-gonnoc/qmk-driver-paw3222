/* Copyright 2021 Gompa (@Gompa)
 * Modifications Copyright 2025 sekigon-gonnoc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// https://github.com/shinoaliceKabocha/choco60_track/tree/master/keymaps/default

#include "pointing_device.h"

#include "paw3222.h"
#include "wait.h"
#include "debug.h"
#include "gpio.h"
#include "pointing_device_internal.h"

#define REG_PID1 0x00
#define REG_PID2 0x01
#define REG_STAT 0x02
#define REG_X 0x03
#define REG_Y 0x04
#define REG_CONFIGURATION 0x06
#define REG_CPI_X 0x0D
#define REG_CPI_Y 0x0E

#define REG_PROTECT 0x09

#define VAL_PROTECT_DISABLE 0x5A
#define VAL_PROTECT_ENABLE 0x00

#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

// CPI values
enum cpi_values {
    CPI400,  // 0b000
    CPI500,  // 0b001
    CPI600,  // 0b010
    CPI800,  // 0b011
    CPI1000, // 0b100
    CPI1200, // 0b101
    CPI1600, // 0b110
};

#define CPI_STEP 38
#define CPI_MIN (16 * CPI_STEP)
#define CPI_MAX (127 * CPI_STEP)

uint8_t paw3222_serial_read(void);
void    paw3222_serial_write(uint8_t reg_addr);
uint8_t paw3222_read_reg(uint8_t reg_addr);
void    paw3222_write_reg(uint8_t reg_addr, uint8_t data);

const pointing_device_driver_t paw3222_pointing_device_driver = {
    .init       = paw3222_init,
    .get_report = paw3222_get_report,
    .set_cpi    = paw3222_set_cpi,
    .get_cpi    = paw3222_get_cpi,
};

void paw3222_init(void) {
    gpio_write_pin_high(PAW3222_CS_PIN);       // set cs pin high
    gpio_set_pin_output(PAW3222_CS_PIN);       // set cs pin to output
    gpio_write_pin_high(PAW3222_SCLK_PIN);     // set clock pin high
    gpio_set_pin_output(PAW3222_SCLK_PIN);     // setclockpin to output
    gpio_set_pin_input_high(PAW3222_SDIO_PIN); // set datapin input high

    paw3222_write_reg(REG_CONFIGURATION, 0x80); // reset sensor
    wait_ms(2);

    paw3222_read_reg(0x00); // read id
    paw3222_read_reg(0x01); // read id2
    paw3222_read_reg(0x02);
    paw3222_read_reg(0x03);
    paw3222_read_reg(0x04);
    paw3222_read_reg(0x12);
}

uint8_t paw3222_serial_read(void) {
    gpio_set_pin_input(PAW3222_SDIO_PIN);
    uint8_t byte = 0;

    for (uint8_t i = 0; i < 8; ++i) {
        gpio_write_pin_low(PAW3222_SCLK_PIN);
        wait_us(1);

        byte = (byte << 1) | gpio_read_pin(PAW3222_SDIO_PIN);

        gpio_write_pin_high(PAW3222_SCLK_PIN);
        wait_us(1);
    }

    return byte;
}

void paw3222_serial_write(uint8_t data) {
    gpio_write_pin_low(PAW3222_SDIO_PIN);
    gpio_set_pin_output(PAW3222_SDIO_PIN);

    for (int8_t b = 7; b >= 0; b--) {
        gpio_write_pin_low(PAW3222_SCLK_PIN);
        if (data & (1 << b)) {
            gpio_write_pin_high(PAW3222_SDIO_PIN);
        } else {
            gpio_write_pin_low(PAW3222_SDIO_PIN);
        }
        gpio_write_pin_high(PAW3222_SCLK_PIN);
    }

    wait_us(4);
}

report_paw3222_t paw3222_read(void) {
    report_paw3222_t data = {0};

    data.isMotion = paw3222_read_reg(REG_STAT) & (1 << 7); // check for motion only (bit 7 in field)
    data.x        = (int8_t)paw3222_read_reg(REG_X);
    data.y        = (int8_t)paw3222_read_reg(REG_Y);

    return data;
}

void paw3222_write_reg(uint8_t reg_addr, uint8_t data) {
    gpio_write_pin_low(PAW3222_CS_PIN); // set cs pin low
    paw3222_serial_write(0b10000000 | reg_addr);
    paw3222_serial_write(data);
    gpio_write_pin_high(PAW3222_CS_PIN); // set cs pin high
}

uint8_t paw3222_read_reg(uint8_t reg_addr) {
    gpio_write_pin_low(PAW3222_CS_PIN); // set cs pin low
    paw3222_serial_write(reg_addr);
    wait_us(5);
    uint8_t reg = paw3222_serial_read();
    gpio_write_pin_high(PAW3222_CS_PIN); // set cs pin high

    return reg;
}

void paw3222_set_cpi(uint16_t cpi) {
    if (cpi < CPI_MIN) {
        cpi = CPI_MIN;
    } else if (cpi > CPI_MAX) {
        cpi = CPI_MAX;
    }
    uint8_t cpival = (cpi + (CPI_STEP >> 1)) / CPI_STEP;

    paw3222_write_reg(REG_PROTECT, VAL_PROTECT_DISABLE);
    paw3222_write_reg(REG_CPI_X, cpival);
    paw3222_write_reg(REG_CPI_Y, cpival);
    paw3222_write_reg(REG_PROTECT, VAL_PROTECT_ENABLE);
}

uint16_t paw3222_get_cpi(void) {
    return paw3222_read_reg(REG_CPI_X) * CPI_STEP;
}

uint8_t read_pid_paw3222(void) {
    return paw3222_read_reg(REG_PID1);
}

report_mouse_t paw3222_get_report(report_mouse_t mouse_report) {
    report_paw3222_t data = paw3222_read();
    if (data.isMotion) {
        pd_dprintf("Raw ] X: %d, Y: %d\n", data.x, data.y);

        mouse_report.x = data.x;
        mouse_report.y = data.y;
    }

    return mouse_report;
}

void pointing_device_driver_init(void) {
    paw3222_init();
}

report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {
    return paw3222_get_report(mouse_report);
}

uint16_t pointing_device_driver_get_cpi(void) {
    return paw3222_get_cpi();
}

void pointing_device_driver_set_cpi(uint16_t cpi) {
    paw3222_set_cpi(cpi);
}