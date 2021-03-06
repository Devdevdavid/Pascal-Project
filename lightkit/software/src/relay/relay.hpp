/**
  * @file   relay.hpp
  * @brief  Control a relay
  * @author David DEVANT
  * @date   11/06/2020
  */

#ifndef RELAY_RELAY_HPP
#define RELAY_RELAY_HPP

#include <Arduino.h>

bool relay_get_theoretical_state(void);
bool relay_get_state(void);
void relay_set_toogle_timeout(uint32_t timeoutMs);
void relay_set_state(bool isClose);
bool relay_toogle_state(void);
int  relay_init(void);
void relay_main(void);

#endif /* RELAY_RELAY_HPP */