#include "global.hpp"

bool relay_get_state(void);
void relay_set_toogle_timeout(uint32_t timeoutMs);
void relay_set_state(bool isClose);
void relay_init(void);
void relay_main(void);