#include "cy8cmbr3116.h"

CY8CMBR3116::CY8CMBR3116() {}

bool CY8CMBR3116::begin(uint8_t i2caddr, TwoWire* theWire) {
    _theWire = theWire;
    _i2c_addr = i2caddr;
    _config_data[81] = i2caddr;
    // 计算CRC值
    _theWire->setClock(400000);
    _config_data[126] = 0;
    _config_data[127] = 0;
    uint16_t crc_data = calculate_crc(_config_data);
    _config_data[126] = (unsigned char)(crc_data & 0xFF);
    _config_data[127] = (unsigned char)(crc_data >>8 &0xFF);
    // 实例化I2C设备
    if (i2c_dev) {
        delete i2c_dev;
    }
    i2c_dev = new Adafruit_I2CDevice(i2caddr, _theWire);
    if (!i2c_dev->begin()) {
        return false;
    }
    return 1;
}

void CY8CMBR3116::writeRegister(uint8_t reg, uint8_t value) {
    Adafruit_BusIO_Register err_reg =
        Adafruit_BusIO_Register(i2c_dev, CTRL_CMD_STATUS, 1);

    Adafruit_BusIO_Register the_reg = Adafruit_BusIO_Register(i2c_dev, reg, 1);
    the_reg.write(value);
}

bool CY8CMBR3116::writeAllRegister() {
    // 计算CRC值
    _config_data[126] = 0;
    _config_data[127] = 0;
    uint16_t crc_data = calculate_crc(_config_data);
    _config_data[126] = (unsigned char)(crc_data & 0xFF);
    _config_data[127] = (unsigned char)(crc_data >>8 &0xFF);
    // 写入寄存器
    // writeRegister(REGMAP_ORIGIN, 0x00);
    // writeRegister(REGMAP_ORIGIN, 0x00);
    // delay(30);
    uint8_t prefix[1];
    uint8_t data[31];
    for (int i=0; i<31; i++) data[i] = _config_data[i];
    prefix[0] = 0;
    i2c_dev->write(data,  31, 1, prefix, 1);
    for (int i=0; i<31; i++) data[i] = _config_data[i+31];
    prefix[0] = 31;
    i2c_dev->write(data, 31, 1, prefix, 1);
    for (int i=0; i<31; i++) data[i] = _config_data[i+62];
    prefix[0] = 62;
    i2c_dev->write(data, 31, 1, prefix, 1);
    for (int i=0; i<31; i++) data[i] = _config_data[i+93];
    prefix[0] = 93;
    i2c_dev->write(data, 31, 1, prefix, 1);
    for (int i=0; i<4; i++) data[i] = _config_data[i+124];
    for (int i=0; i<27; i++) data[i+4] = 0;
    prefix[0] = 124;
    i2c_dev->write(data, 4, 1, prefix, 1);
    // 保存配置
    Adafruit_BusIO_Register cmd_reg = Adafruit_BusIO_Register(i2c_dev, CTRL_CMD, 1);
    cmd_reg.write(SAVE_CHECK_CRC);
    // 检查是否有错
    delay(440);
    Adafruit_BusIO_Register cmd_status_reg = Adafruit_BusIO_Register(i2c_dev, CTRL_CMD_STATUS, 1);
    if (!cmd_status_reg.read()) {
        cmd_reg.write(SW_RESET);
        // cmd_reg.write(SW_RESET);
        delay(400);
        while(!begin(_i2c_addr, _theWire)) delay(300);
        return 1;
    } else {
        return 0;
    }
}

bool CY8CMBR3116::set_i2c_address(unsigned char addr) {
    if (addr < 119 && addr > 8) {
        _config_data[81] = addr;
        _i2c_addr = addr;
        if (writeAllRegister()) {
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}

uint16_t CY8CMBR3116::read_touched_data(uint8_t id) {
    return readRegister16(_touched_data_addr[id]);
}

uint8_t CY8CMBR3116::readRegister8(uint8_t reg) {
    Adafruit_BusIO_Register thereg = Adafruit_BusIO_Register(i2c_dev, reg, 1);

    return (thereg.read());
}

uint16_t CY8CMBR3116::readRegister16(uint8_t reg) {
    Adafruit_BusIO_Register thereg =
        Adafruit_BusIO_Register(i2c_dev, reg, 2, LSBFIRST);

    return (thereg.read());
}

uint16_t CY8CMBR3116::Calc4BitsCRC(uint8_t value, uint16_t remainder) {
    uint8_t tableIndex;

    /* Divide the value by polynomial, via the CRC polynomial */
    tableIndex = (value & CY8CMBR3xxx_CRC_BIT4_MASK) ^
                 ((remainder) >>
                  (CY8CMBR3xxx_CRC_BIT_WIDTH - CY8CMBR3xxx_CRC_BIT4_SHIFT));
    remainder = (CY8CMBR3xxx_CCITT16_POLYNOM * tableIndex) ^
                (remainder << CY8CMBR3xxx_CRC_BIT4_SHIFT);
    return remainder;
}

uint16_t CY8CMBR3116::calculate_crc(unsigned char config_data[]) {
    uint32_t messageIndex;
    uint8_t byteValue;
    uint16_t seed = CY8CMBR3xxx_CCITT16_DEFAULT_SEED;

    /* don't make count down cycle! CRC will be different! */
    for (messageIndex = 0; messageIndex < CY8CMBR3xxx_CONFIG_DATA_LENGTH;
         messageIndex++) {
        byteValue = config_data[messageIndex];
        seed = Calc4BitsCRC(byteValue >> CY8CMBR3xxx_CRC_BIT4_SHIFT, seed);
        seed = Calc4BitsCRC(byteValue, seed);
    }

    return seed;
}

uint16_t CY8CMBR3116::touched() {
    return readRegister16(BUTTON_STATUS);
}