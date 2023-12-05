/*
 * Chu Controller Board Definitions
 * WHowe <github.com/whowechina>
 */


#define I2C_PORT i2c0
#define I2C_SDA 20
#define I2C_SCL 21
#define I2C_FREQ 733*1000

#define I2C_HUB_EN 19

#define TOF_MUX_LIST { 1, 2, 0, 4, 5 }

#define RGB_PIN 2
#define RGB_ORDER GRB // or RGB

#define NKRO_KEYMAP "1aqz2swx3dec4frv5gtb6hyn7jum8ki90olp,."