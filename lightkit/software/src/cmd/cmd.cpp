/**
  * @file   cmd.cpp
  * @brief  General API for serial interface
  * @author David DEVANT
  * @date   12/08/2018
  */

#include "cmd.hpp"
#include "buzzer/buzzer.hpp"
#include "flash/flash.hpp"
#include "global.hpp"
#include "relay/relay.hpp"
#include "script/script.hpp"
#include "status_led/status_led.hpp"
#include "stripled/stripled.hpp"
#include "wifi/wifi.hpp"

void cmd_reset_module(void)
{
	// Resetting with new configuration
	script_delayed_reset(1000);
}

/**
 * @brief Set the module name used for mDNS
 *
 * @param newName The new name to use (len < MODULE_NAME_SIZE_MAX)
 * @return OK: 0, Too short: -1
 */
int cmd_set_module_name(String newName)
{
	// Copy the new name
	strncpy(flashSettings.moduleName, newName.c_str(), MODULE_NAME_SIZE_MAX);

	// Always terminate the string
	flashSettings.moduleName[MODULE_NAME_SIZE_MAX - 1] = '\0';

	flash_write();

	return 0;
}

/**
 * @brief Get the current module name
 *
 * @return The module name as a String
 */
String cmd_get_module_name(void)
{
	return String(flashSettings.moduleName);
}

void cmd_print_status(void)
{
	log_raw("APPLI:   0x%02X\n", STATUS_APPLI);
	log_raw("WIFI:    0x%02X\n", STATUS_WIFI);
	wifi_print();
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

#ifdef MODULE_BUZZER
/**
 * Enable/Disable the status leds
 */
int cmd_set_buzzer(uint8_t buzzerId, uint8_t melody, bool repeat)
{
	return buzzer_set_melody(buzzerId, melody, repeat);
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
		stripled_brightness_set(((uint16_t) (newValue * 255)) / 100);
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
	return ((uint16_t) (flashSettings.stripledParams.brightness * 100)) / 255;
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
		stripled_nb_led_set(newValue);
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
	return flashSettings.stripledParams.nbLed;
}
#endif

/**
 * Set the color for some LED animations
 * @param newValue ]0x0; 0xFFFFFF]
 */
#ifdef MODULE_STRIPLED
void cmd_set_color(uint32_t newValue)
{
	rgba_u color;

	// We do not accept black color
	if ((newValue & 0xFFFFFF) > 0) {
		// Handle conversion
		color.u32 = newValue;

		stripled_color_set(&color);
		cmd_set_state(true);
		cmd_set_demo_mode(false);
		cmd_set_animation(0); // 0: Static
	}
}
#endif

/**
 * Get the color currently configured
 * @return nbLed [0x0; 0xFFFFFFFF]
 */
#ifdef MODULE_STRIPLED
uint32_t cmd_get_color(void)
{
	return flashSettings.stripledParams.color.u32;
}
#endif

/**
 * Get the LED State
 * @return  0: LED are OFF, 1: LED are ON
 */
#ifdef MODULE_STRIPLED
bool cmd_get_state(void)
{
	return flashSettings.stripledParams.isOn;
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
	return flashSettings.stripledParams.isInDemoMode;
}
#endif

/**
 * Set the demo mode of LED
 * @param  isInDemoMode    bool
 * @return          [description]
 */
#ifdef MODULE_STRIPLED
int32_t cmd_set_demo_mode(bool isInDemoMode)
{
	stripled_set_demo_mode(isInDemoMode);
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
	return flashSettings.stripledParams.animID;
}
#endif

/**
 * Call stripled_set_animation()
 * @param  animID The animation ID to use
 * @return          See stripled_set_animation()
 */
#ifdef MODULE_STRIPLED
int32_t cmd_set_animation(uint8_t animID)
{
	return stripled_set_animation(animID);
}
#endif

/**
 * @brief Call flash_use_default()
 */
int32_t cmd_flash_setting_reset(void)
{
	return flash_use_default();
}
