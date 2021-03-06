/**
  * @file   temp.hpp
  * @brief  Get temperature of OneWire sensors
  * @author David DEVANT
  * @date   12/08/2018
  */

#ifndef TEMP_TEMP_HPP
#define TEMP_TEMP_HPP

#include "global.hpp"

#define DEVICE_INDEX_0 0
#define DEVICE_INDEX_1 1

float   temp_get_value(uint8_t deviceIndex);
char *  temp_get_address(char * str, uint8_t deviceIndex);
uint8_t temp_get_nb_sensor(void);
int     temp_init(void);
void    temp_main(void);

#endif /* TEMP_TEMP_HPP */
