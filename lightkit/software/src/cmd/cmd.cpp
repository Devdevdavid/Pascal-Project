/**
  * @file   cmd.cpp
  * @brief  General API for serial interface
  * @author David DEVANT
  * @date   12/08/2018
  */

#include "cmd.hpp"
#include "global.hpp"
#include "relay/relay.hpp"
#include "status_led/status_led.hpp"
#include "stripled/stripled.hpp"

void cmd_print_help(void)
{
	log_raw("=== " FIRMWARE_VERSION " ===\n");
	log_raw("H                         This help\n");
	log_raw("T                         Print board status\n");
#ifdef MODULE_STATUS_LED
	log_raw("L[State]                  Toggle or set Led Visu Visibility\n");
#endif
#ifdef MODULE_STRIPLED
	log_raw("B[Value]                  Set brightness, no value = Auto\n");
	log_raw("S[1/0: State]             Set the LED ON or OFF\n");
	log_raw("D[1/0: State]             Set the demo mode ON or OFF\n");
	log_raw("A<animID>                 Set animation 0 to 55\n");
#endif
#ifdef MODULE_RELAY
	log_raw("R[1/0: State]             Toggle or Set Relay On or Off\n");
#endif
}

void cmd_print_status(void)
{
	log_raw("APPLI:   0x%02X\n", STATUS_APPLI);
	log_raw("WIFI:    0x%02X\n", STATUS_WIFI);
	log_raw("BRIGHT:  %d%%", (STATUS_BRIGHTNESS * 100) / 255);
	if (_isset(STATUS_APPLI, STATUS_APPLI_AUTOLUM)) {
		log_raw(" AUTO\n");
	} else {
		log_raw("\n");
	}
	log_raw("LEVEL:   %d\n", STATUS_BRIGHT_LVL);
	log_raw("ANIM:    %d\n", STATUS_ANIM);
}

#ifdef MODULE_STATUS_LED
/**
 * Enable/Disable the status leds
 */
void cmd_set_status_led(uint8_t isEnabled)
{
	if (isEnabled) {
		_set(STATUS_APPLI, STATUS_APPLI_STATUS_LED);
	} else {
		_unset(STATUS_APPLI, STATUS_APPLI_STATUS_LED);
		status_led_turnoff();
	}
}
#endif

/**
 * Set/Reset brightness in automayic mode
 * @param newValue [description]
 */
void cmd_set_brightness_auto(bool newValue)
{
	if (newValue) {
		_set(STATUS_APPLI, STATUS_APPLI_AUTOLUM);
	} else {
		_unset(STATUS_APPLI, STATUS_APPLI_AUTOLUM);
	}
}

/**
 * Set the output brightness value and
 * disable auto bright if set
 * @param newValue ]0-100]
 */
#ifdef MODULE_STRIPLED
void cmd_set_brightness(uint8_t newValue)
{
	if (newValue > 0 && newValue <= 100) {
		brightness_set(((uint16_t)(newValue * 255)) / 100);
		cmd_set_brightness_auto(false);
	}
}
#endif

/**
 * Get the output brightness value
 * @param newValue [0-100]
 */
#ifdef MODULE_STRIPLED
uint8_t cmd_get_brightness(void)
{
	return ((uint16_t)(STATUS_BRIGHTNESS * 100)) / 255;
}
#endif

/**
 * Set the number of LED for the strip
 * @param newValue [1; STRIPLED_MAX_NB_PIXELS]
 */
#ifdef MODULE_STRIPLED
void cmd_set_nb_led(uint8_t newValue)
{
	if (newValue >= 1 && newValue <= STRIPLED_MAX_NB_PIXELS) {
		nb_led_set(newValue);
	}
}
#endif

/**
 * Get the number of LED
 * @return nbLed [1; STRIPLED_MAX_NB_PIXELS]
 */
#ifdef MODULE_STRIPLED
uint8_t cmd_get_nb_led(void)
{
	return STATUS_NB_LED;
}
#endif

/**
 * Set the color for some LED animations
 * @param newValue ]0x0; 0xFFFFFF]
 */
#ifdef MODULE_STRIPLED
void cmd_set_color(uint32_t newValue)
{
	if (newValue > 0 && newValue <= 0xFFFFFF) {
		color_set(newValue);
		cmd_set_state(true);
		cmd_set_demo_mode(false);
		cmd_set_animation(0); // 0: Static
	}
}
#endif

/**
 * Get the color currently configured
 * @return nbLed [0x0; 0xFFFFFF]
 */
#ifdef MODULE_STRIPLED
uint32_t cmd_get_color(void)
{
	return (STATUS_COLOR_R << 16) | (STATUS_COLOR_G << 8) | (STATUS_COLOR_B);
}
#endif

/**
 * Get the LED State
 * @return  0: LED are OFF, 1: LED are ON
 */
#ifdef MODULE_STRIPLED
bool cmd_get_state(void)
{
	return _isset(STATUS_APPLI, STATUS_APPLI_LED_IS_ON);
}
#endif

/**
 * Set the state of LED
 * @param  state    [description]
 * @return          [description]
 */
#ifdef MODULE_STRIPLED
int32_t cmd_set_state(bool state)
{
	stripled_set_state(state);
	return 0;
}
#endif

/**
 * Get the demo mode
 * @return  0: Is not in demo, 1: Is in demo
 */
#ifdef MODULE_STRIPLED
bool cmd_get_demo_mode(void)
{
	return _isset(STATUS_APPLI, STATUS_APPLI_DEMO_MODE);
}
#endif

/**
 * Set the demo mode of LED
 * @param  isDemoMode    bool
 * @return          [description]
 */
#ifdef MODULE_STRIPLED
int32_t cmd_set_demo_mode(bool isDemoMode)
{
	stripled_set_demo_mode(isDemoMode);
	return 0;
}
#endif

/**
 * Call get_animation()
 * @return  animID The animation ID used
 */
#ifdef MODULE_STRIPLED
uint8_t cmd_get_animation(void)
{
	return STATUS_ANIM;
}
#endif

/**
 * Call set_animation()
 * @param  animID The animation ID to use
 * @return          See set_animation()
 */
#ifdef MODULE_STRIPLED
int32_t cmd_set_animation(uint8_t animID)
{
	return set_animation(animID);
}
#endif