/*
 * Nunchuck functions  -- Talk to a Wii Nunchuck
 *
 * This library is from the Bionic Arduino course : 
 *                          http://todbot.com/blog/bionicarduino/
 *
 * 2007-11 Tod E. Kurt, http://todbot.com/blog/
 *
 * The Wii Nunchuck reading code originally from Windmeadow Labs
 *   http://www.windmeadow.com/node/42
 */

#include "i2cmaster.h"

#define NUNCHUCK_ID 0x52
#define START_WRITE ((NUNCHUCK_ID << 1) | I2C_WRITE)
#define START_READ  ((NUNCHUCK_ID << 1) | I2C_READ)

static uint8_t nunchuck_buf[6];   // array to store nunchuck data,

static void nunchuck_send_request();

// initialize the I2C system, join the I2C bus,
// and tell the nunchuck we're talking to it
static int nunchuck_init()
{ 
    unsigned char ret;

    i2c_init();                  // join i2c bus as master    
    ret = i2c_start(START_WRITE);// transmit to device 0x52
    if (ret) {
      i2c_stop();
      return -1;
    } else {
      ret = i2c_write(0x40); // Address 0x40
      ret |= i2c_write(0x00); // Data zero
    }
    i2c_stop();  // Release bus
    return -ret;
}

// Send a request for data to the nunchuck
// was "send_zero()"
static void nunchuck_send_request()
{
    i2c_start(START_WRITE);// transmit to device 0x52
    i2c_write(0x00); // Send one byte
    i2c_stop();
}

// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
static char nunchuk_decode_byte (char x)
{
    x = (x ^ 0x17) + 0x17;
    return x;
}

// Receive data back from the nunchuck, 
// returns 1 on successful read. returns 0 on failure
static int nunchuck_get_data()
{
    unsigned char ret;
    int cnt=0;
    ret = i2c_start(START_READ);
    if (ret) return 0;
    nunchuck_buf[cnt++] = nunchuk_decode_byte(i2c_readAck());
    nunchuck_buf[cnt++] = nunchuk_decode_byte(i2c_readAck());
    nunchuck_buf[cnt++] = nunchuk_decode_byte(i2c_readAck());
    nunchuck_buf[cnt++] = nunchuk_decode_byte(i2c_readAck());
    nunchuck_buf[cnt++] = nunchuk_decode_byte(i2c_readAck());
    nunchuck_buf[cnt++] = nunchuk_decode_byte(i2c_readNak());
    i2c_stop();
    nunchuck_send_request();
    return 1;
}

// returns zbutton state: 1=pressed, 0=notpressed
static int nunchuck_zbutton()
{
    return ((nunchuck_buf[5] >> 0) & 1) ? 0 : 1;  // voodoo
}

// returns zbutton state: 1=pressed, 0=notpressed
static int nunchuck_cbutton()
{
    return ((nunchuck_buf[5] >> 1) & 1) ? 0 : 1;  // voodoo
}

// returns value of x-axis joystick
static int nunchuck_joyx()
{
    return nunchuck_buf[0]; 
}

// returns value of y-axis joystick
static int nunchuck_joyy()
{
    return nunchuck_buf[1];
}

// returns value of x-axis accelerometer
static int nunchuck_accelx()
{
    return nunchuck_buf[2];   // FIXME: this leaves out 2-bits of the data
}

// returns value of y-axis accelerometer
static int nunchuck_accely()
{
    return nunchuck_buf[3];   // FIXME: this leaves out 2-bits of the data
}

// returns value of z-axis accelerometer
static int nunchuck_accelz()
{
    return nunchuck_buf[4];   // FIXME: this leaves out 2-bits of the data
}

