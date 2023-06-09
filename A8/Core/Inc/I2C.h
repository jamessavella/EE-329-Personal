#ifndef INC_I2C_H_
#define INC_I2C_H_


#define EEPROM_ADDRESS 0x50


void I2C_setup();
void I2C_write(uint16_t mem_addr, uint8_t data_write);
uint8_t I2C_read(uint16_t mem_addr);


#endif /* INC_I2C_H_ */
