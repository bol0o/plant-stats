#include "BME280_Driver.h"

void BME280_Driver::writeReg(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

bool BME280_Driver::readRegs(uint8_t reg, uint8_t* data, uint8_t len) {
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    
    uint8_t error = Wire.endTransmission();
    if (error != 0) {
        return false; 
    }

    uint8_t received = Wire.requestFrom(_addr, len);
    if (received != len) {
        return false;
    }

    for (uint8_t i = 0; i < len; i++) {
        data[i] = Wire.read();
    }

    return true;
}

bool BME280_Driver::begin() {
    uint8_t chipID;
    readRegs(0xD0, &chipID, 1);
    if (chipID != 0x60) {
        return false;
    }

    uint8_t cal[26];
    readRegs(0x88, cal, 26);
    
    _calib.dig_T1 = (cal[1] << 8) | cal[0];
    _calib.dig_T2 = (cal[3] << 8) | cal[2];
    _calib.dig_T3 = (cal[5] << 8) | cal[4];

    _calib.dig_P1 = (cal[7] << 8) | cal[6];
    _calib.dig_P2 = (cal[9] << 8) | cal[8];
    _calib.dig_P3 = (cal[11] << 8) | cal[10];
    _calib.dig_P4 = (cal[13] << 8) | cal[12];
    _calib.dig_P5 = (cal[15] << 8) | cal[14];
    _calib.dig_P6 = (cal[17] << 8) | cal[16];
    _calib.dig_P7 = (cal[19] << 8) | cal[18];
    _calib.dig_P8 = (cal[21] << 8) | cal[20];
    _calib.dig_P9 = (cal[23] << 8) | cal[22];

    readRegs(0xA1, &_calib.dig_H1, 1);

    uint8_t cal_h[7];
    readRegs(0xE1, cal_h, 7);
    
    _calib.dig_H2 = (cal_h[1] << 8) | cal_h[0];
    _calib.dig_H3 = cal_h[2];
    _calib.dig_H4 = ((int8_t)cal_h[3] << 4) | (cal_h[4] & 0x0F);
    _calib.dig_H5 = ((int8_t)cal_h[5] << 4) | (cal_h[4] >> 4);
    _calib.dig_H6 = (int8_t)cal_h[6];

    writeReg(0xF2, 0x01); // Humidity oversampling: x1    
    writeReg(0xF4, 0x27); // Temp x1 (001), Press x1 (001), Mode Normal (11)

    return true;
}

float BME280_Driver::compensateTemp(int32_t adc_T) {
    int32_t var1, var2, T;
    
    var1 = ((((adc_T >> 3) - ((int32_t)_calib.dig_T1 << 1))) * ((int32_t)_calib.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)_calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)_calib.dig_T1))) >> 12) * ((int32_t)_calib.dig_T3)) >> 14;
    
    t_fine = var1 + var2;
    
    T = (t_fine * 5 + 128) >> 8;
    return (float)T / 100.0;
}

float BME280_Driver::compensateHum(int32_t adc_H) {
    int32_t v_x1_u32r;
    
    v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)_calib.dig_H4) << 20) - (((int32_t)_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
                 (((((((v_x1_u32r * ((int32_t)_calib.dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
                   ((int32_t)_calib.dig_H2) + 8192) >> 14));
    
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)_calib.dig_H1)) >> 4));
    
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    
    return (float)(v_x1_u32r >> 12) / 1024.0;
}

BME_Result BME280_Driver::getValues() {
    BME_Result res = {0.0f, 0.0f, false};
    uint8_t raw[8];

    if (!readRegs(0xF7, raw, 8)) {
        Serial.println("BLAD I2C: Nie udalo sie odczytac BME280!");
        return res; 
    }

    int32_t adc_T = (raw[3] << 12) | (raw[4] << 4) | (raw[5] >> 4);
    int32_t adc_H = (raw[6] << 8) | raw[7];

    res.temperature = compensateTemp(adc_T);
    res.humidity = compensateHum(adc_H);
    res.success = true;

    return res;
}