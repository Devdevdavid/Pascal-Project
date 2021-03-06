/**
  * @file   script.hpp
  * @brief  Interconnection point of all modules
  * @author David DEVANT
  * @date   12/08/2019
  */

#ifndef SCRIPT_SCRIPT_HPP
#define SCRIPT_SCRIPT_HPP

#include "global.hpp"

void script_main(void);
#if defined(MODULE_RELAY)
void script_relay_feedback_event(void);
void script_relay_set_event(bool isClosed);
#endif
void script_delayed_reset(uint32_t tickCount);

#endif /* SCRIPT_SCRIPT_HPP s*/