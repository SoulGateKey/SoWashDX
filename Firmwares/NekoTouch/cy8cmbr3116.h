#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include "Arduino.h"

/* CY8CMBR3116 Register Map Offset Address */
#define REGMAP_ORIGIN 0x00
#define SENSOR_PATTERN 0x00
#define FSS_EN 0x02
#define TOGGLE_EN 0x04
#define LED_ON_EN 0x06
#define SENSITIVITY0 0x08
#define SENSITIVITY1 0x09
#define SENSITIVITY2 0x0A
#define SENSITIVITY3 0x0B
#define BASE_THRESHOLD0 0x0C
#define BASE_THRESHOLD1 0x0D
#define FINGER_THRESHOLD2 0x0E
#define FINGER_THRESHOLD3 0x0F
#define FINGER_THRESHOLD4 0x10
#define FINGER_THRESHOLD5 0x11
#define FINGER_THRESHOLD6 0x12
#define FINGER_THRESHOLD7 0x13
#define FINGER_THRESHOLD8 0x14
#define FINGER_THRESHOLD9 0x15
#define FINGER_THRESHOLD10 0x16
#define FINGER_THRESHOLD11 0x17
#define FINGER_THRESHOLD12 0x18
#define FINGER_THRESHOLD13 0x19
#define FINGER_THRESHOLD14 0x1A
#define FINGER_THRESHOLD15 0x1B
#define SENSOR_DEBOUNCE 0x1C
#define BUTTON_HYS 0x1D
#define BUTTON_BUT 0x1E
#define BUTTON_LBR 0x1F
#define BUTTON_NNT 0x20
#define BUTTON_NT 0x21
#define PROX_EN 0x26
#define PROX_CFG 0x27
#define PROX_CFG2 0x28
#define PROX_TOUCH_TH0 0x2A
#define PROX_TOUCH_TH1 0x2C
#define PROX_HYS 0x30
#define PROX_BUT 0x31
#define PROX_LBR 0x32
#define PROX_NNT 0x33
#define PROX_NT 0x34
#define PROX_POSITIVE_TH0 0x35
#define PROX_POSITIVE_TH1 0x36
#define PROX_NEGATIVE_TH0 0x39
#define PROX_NEGATIVE_TH1 0x3A
#define LED_ON_TIME 0x3D
#define BUZZER_CFG 0x3E
#define BUZZER_ON_TIME 0x3F
#define GPO_CFG 0x40
#define PWM_DUTYCYCLE_CFG0 0x41
#define PWM_DUTYCYCLE_CFG1 0x42
#define PWM_DUTYCYCLE_CFG2 0x43
#define PWM_DUTYCYCLE_CFG3 0x44
#define PWM_DUTYCYCLE_CFG4 0x45
#define PWM_DUTYCYCLE_CFG5 0x46
#define PWM_DUTYCYCLE_CFG6 0x47
#define PWM_DUTYCYCLE_CFG7 0x48
#define SPO_CFG 0x4C
#define DEVICE_CFG0 0x4D
#define DEVICE_CFG1 0x4E
#define DEVICE_CFG2 0x4F
#define I2C_ADDR 0x51
#define REFRESH_CTRL 0x52
#define STATE_TIMEOUT 0x55
#define SLIDER_CFG 0x5D
#define SLIDER1_CFG 0x61
#define SLIDER1_RESOLUTION 0x62
#define SLIDER1_THRESHOLD 0x63
#define SLIDER2_CFG 0x67
#define SLIDER2_RESOLUTION 0x68
#define SLIDER2_THRESHOLD 0x69
#define SLIDER_DEBOUNCE 0x6F
#define SLIDER_BUT 0x70
#define SLIDER_LBR 0x71
#define SLIDER_NNT 0x72
#define SLIDER_NT 0x73
#define CONFIG_CRC 0x7E
#define GPO_OUTPUT_STATE 0x80
#define SENSOR_ID 0x82
#define CTRL_CMD 0x86
#define CTRL_CMD_STATUS 0x88
#define BUTTON_STATUS 0xAA
#define PROX_STAT 0xAE

#define DIFFERENCE_COUNT_SENSOR0 0xBA
#define DIFFERENCE_COUNT_SENSOR1 0xBC
#define DIFFERENCE_COUNT_SENSOR2 0xBE
#define DIFFERENCE_COUNT_SENSOR3 0xC0
#define DIFFERENCE_COUNT_SENSOR4 0xC2
#define DIFFERENCE_COUNT_SENSOR5 0xC4
#define DIFFERENCE_COUNT_SENSOR6 0xC6
#define DIFFERENCE_COUNT_SENSOR7 0xC8
#define DIFFERENCE_COUNT_SENSOR8 0xCA
#define DIFFERENCE_COUNT_SENSOR9 0xCC
#define DIFFERENCE_COUNT_SENSOR10 0xCE
#define DIFFERENCE_COUNT_SENSOR11 0xD0
#define DIFFERENCE_COUNT_SENSOR12 0xD2
#define DIFFERENCE_COUNT_SENSOR13 0xD4
#define DIFFERENCE_COUNT_SENSOR14 0xD6
#define DIFFERENCE_COUNT_SENSOR15 0xD8

/* Command Codes */
#define CMD_NULL 0x00
#define SAVE_CHECK_CRC 0x02
#define CALC_CRC 0x03
#define LOAD_FACTORY 0x04
#define LOAD_PRIMARY 0x05
#define LOAD_SECONDARY 0x06
#define SLEEP 0x07
#define CLEAR_LATCHED_STATUS 0x08
#define CMD_RESET_PROX0_FILTER 0x09
#define CMD_RESET_PROX1_FILTER 0x0A
#define ENTER_CONFIG_MODE 0x0B
#define EXIT_CONTROL_RUN 0xFE
#define SW_RESET 0xFF

/* Total number of configuration registers */
#define TOTAL_CONFIG_REG_COUNT 0x80

/* Length of Register Map */
#define REG_MAP_LEN 256

/* Slave Address (Default) */
#define SLAVE_ADDR 0x37
#define NO_OF_KIT_BUTTONS 4
#define BUZ_ON_1_33kHz 0x85
#define BUZ_OFF_1_33kHz 0x05
#define SPO_ENABLE 0x03
#define SPO_DISABLE 0x00

/* API Constants */
#define CY8CMBR3xxx_CONFIG_DATA_LENGTH (126)
#define CY8CMBR3xxx_CRC_BIT_WIDTH (sizeof(uint16_t) * 8)
#define CY8CMBR3xxx_CRC_BIT4_MASK (0x0F)
#define CY8CMBR3xxx_CRC_BIT4_SHIFT (0x04)
#define CY8CMBR3xxx_CCITT16_DEFAULT_SEED ((uint16_t)0xffff)
#define CY8CMBR3xxx_CCITT16_POLYNOM ((uint16_t)0x1021)

class CY8CMBR3116 {
   public:
    // Hardware I2C
    CY8CMBR3116();

    /* Generated By EZ-Click */
    /* 20ms Scan Period */
    unsigned char _config_data[128] = {
    0xFFu, 0xFFu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0x80u, 0x80u, 0x80u, 0x80u,
    0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u,
    0x80u, 0x80u, 0x80u, 0x80u, 0x03u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u,
    0x05u, 0x00u, 0x00u, 0x02u, 0x00u, 0x02u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x1Eu, 0x1Eu, 0x00u,
    0x00u, 0x1Eu, 0x1Eu, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u,
    0x00u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu,
    0xFFu, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x01u, 0x48u,
    0x00u, 0x37u, 0x01u, 0x00u, 0x00u, 0x0Au, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x9Bu, 0x4Du
    };  // 最后两位为CRC校验位，需每次重新计算

    bool begin(uint8_t i2caddr = SLAVE_ADDR, TwoWire* theWire = &Wire);

    uint8_t readRegister8(uint8_t reg);
    uint16_t readRegister16(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t value);
    bool writeAllRegister();
    uint16_t touched(void);

    uint16_t Calc4BitsCRC(uint8_t value, uint16_t remainder);

    uint16_t calculate_crc(unsigned char config_data[]);

    bool set_i2c_address(unsigned char addr);

    uint16_t read_touched_data(uint8_t id);

   private:
    Adafruit_I2CDevice* i2c_dev = NULL;
    TwoWire* _theWire = &Wire;
    uint8_t _i2c_addr = SLAVE_ADDR;
    uint8_t _touched_data_addr[16] = {0xBA, 0xBC, 0xBE, 0xC0, 0xC2, 0xC4,
                                      0xC6, 0xC8, 0xCA, 0xCC, 0xCE, 0xD0,
                                      0xD2, 0xD4, 0xD6, 0xD8};
};
