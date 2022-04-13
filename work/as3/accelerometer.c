#include "accelerometer.h"
#include "audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x1c

#define REG_CTRL1 0x2A
#define REG_STATUS 0x00
#define REG_OUT_X_MSB 0x01
#define REG_OUT_X_LSB 0x02
#define REG_OUT_Y_MSB 0x03
#define REG_OUT_Y_LSB 0x04
#define REG_OUT_Z_MSB 0x05
#define REG_OUT_Z_LSB 0x06

static DIRECTION movement = DIRECTION_NONE; // init movement direction
static int i2cFileDesc;
static char *buf;

static int thresh = 600;
static int up_thresh = 0;
static int down_thresh = 0;
static int z_count = 0;

static int x1_thresh = 0;
static int x2_thresh = 0;
static int x_count = 0;

static int y1_thresh = 0;
static int y2_thresh = 0;
static int y_count = 0;

static int first_thresh_x = 0;
static int first_thresh_y = 0;
static int first_thresh_z = 0;

static int release_counter = 0;

// helper functions from brians code
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2) {
        perror("Unable to write i2c register");
        exit(-1);
    }
}


static void readI2cReg(int i2cFileDesc, unsigned char regAddr, char* buf, int count)
{
    // To read a register, must first write the address
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    if (res != sizeof(regAddr)) {
        perror("Unable to write i2c register.");
        exit(-1);
    }

    // Now read the value and return it
    res = read(i2cFileDesc, buf, count);
    if (res != count) {
        perror("Unable to read i2c register");
        exit(-1);
    }
}


static int initI2cBus(char* bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);

    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}


static void sleep_ms(unsigned int delayMs)
{
    const unsigned int NS_PER_MS = 1000 * 1000;
    const unsigned int NS_PER_SECOND = 1000000000;

    unsigned long long delayNs = delayMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;

    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}


void Accelerometer_initialize() 
{
    i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    writeI2cReg(i2cFileDesc, REG_CTRL1, 0x01); // set to active
    buf = malloc(sizeof(char)*7);
}


DIRECTION Accelerometer_getDirection() 
{
    return movement;
}


void Accelerometer_sampler()
{  
    sleep_ms(10);
    readI2cReg(i2cFileDesc, REG_STATUS, buf, 7);

    int16_t x = ((buf[1] << 8) | (buf[2]))>>4;
    if (x > 2047) x -= 4096;

    int16_t y = ((buf[3] << 8) | (buf[4]))>>4;
    if (y > 2047) y -= 4096;

    int16_t z = ((buf[5] << 8) | (buf[6]))>>4;
    z -= 1024;

    if (y > 2047) y -= 4096;

    if (first_thresh_y == 0 && first_thresh_x == 0 && first_thresh_z == 0) {
        movement = DIRECTION_NONE;
    }

    if (first_thresh_y == 0 && first_thresh_x == 0) {
        // detect up/down
        if (z > thresh) {
            up_thresh = 1;
            first_thresh_z = 1;
        }
        if (z < -thresh) {
            down_thresh = 1;
            first_thresh_z = 1;
        }
        if (z > -200 && z < 200) {
            if (up_thresh == 1 && down_thresh == 1) {
                z_count++;
                up_thresh = 0;
                down_thresh = 0;
                first_thresh_z = 0;
                movement = DIRECTION_Z;
                Instrument_Sound sound = DRUM_CYN_HARD;
                queue_sound(sound);
                release_counter = 0;
            }
        }
    }

    if (first_thresh_y == 0 && first_thresh_z == 0) {
        // detect x
        if (x > thresh) {
            x1_thresh = 1;
            first_thresh_x = 1;
        }
        if (x < -thresh) {
            x2_thresh = 1;
            first_thresh_x = 1;
        }
        if (x > -200 && x < 200) {
            if (x1_thresh == 1 && x2_thresh == 1) {
                x_count++;
                x1_thresh = 0;
                x2_thresh = 0;
                first_thresh_x = 0;
                movement = DIRECTION_X;
                Instrument_Sound sound = DRUM_TOM_HI_HARD;
                queue_sound(sound);
                release_counter = 0;
            }
        }
    }

    if (first_thresh_z == 0 && first_thresh_x == 0) {
        // detect y
        if (y > thresh) {
            y1_thresh = 1;
            first_thresh_y = 1;
        }
        if (y < -thresh) {
            y2_thresh = 1;
            first_thresh_y = 1;
        }
        if (y > -200 && y < 200) {
            if (y1_thresh == 1 && y2_thresh == 1) {
                y_count++;
                y1_thresh = 0;
                y2_thresh = 0;
                first_thresh_y = 0;
                movement = DIRECTION_Y;
                Instrument_Sound sound = DRUM_SNARE_HARD;
                queue_sound(sound);
                release_counter = 0;
            }
        }
    }
    if (first_thresh_x == 1 || first_thresh_y == 1 || first_thresh_z ==1) {
        release_counter++;
    }
    if (release_counter > 25) {
        first_thresh_x = 0;
        first_thresh_y = 0;
        first_thresh_z = 0;
    }
    if (movement == DIRECTION_X) {
        movement = DIRECTION_X;
    }
}


void Accelerometer_stopSampling(void) {
    close(i2cFileDesc);
    free(buf);
    buf = NULL;
}