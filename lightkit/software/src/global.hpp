/**
  * @file   global.hpp
  * @brief  Main configuration of boards
  * @author David DEVANT
  * @date   12/08/2019
  */

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

// clang-format off

#include <Arduino.h>
#include "tools/tools.hpp"
#include "tools/log.hpp"
#include "status/status.hpp"
#include "io/io.hpp"

// If you get an error on this file, copy "private_tmpl.hpp" to "private.hpp"
// and fill the blanked #define
// This template system avoid leaking password on Github
#include "private.hpp"

#define APP_VERSION           "LightKit v2.1.0"
#define BUILD_DATE            __DATE__ " " __TIME__
#define LIGHTKIT_VERSION      APP_VERSION " build " BUILD_DATE


/* ===========================
 * COMMON TO ALL BOARDS
 * ===========================
 */
#ifdef ESP32
    #define FS_IS_SPIFFS
    #define G_WebServer  ESP32WebServer
    #define G_FileSystem SPIFFS
#else
    #define FS_IS_LITTLEFS
    #define G_WebServer  ESP8266WebServer
    #define G_FileSystem LittleFS
#endif

#define G_LANG_EN                                   0
#define G_LANG_FR                                   1
#define G_LANG                                      G_LANG_FR

/** Types of Led status */
#define STATUS_LED_TYPE_CLASSIC                     0                       /** Constant for STATUS_LED_TYPE */
#define STATUS_LED_TYPE_NEOPIXEL                    1                       /** Constant for STATUS_LED_TYPE */

/** Modes for inputs pins */
#define I_N                                         0                       /** Input no-pull-up */
#define I_U                                         1                       /** Input pull-up */
#define I_A                                         2                       /** Input analog */

/** Maximum size of module name ('\0' included) */
#define MODULE_NAME_SIZE_MAX 16

/** The name used by default to find the module without IP */
#define MODULE_NAME_DEFAULT  "LightKit"

/**
 * This is the configuration for the board
 * TEMP_DOMOTICZ
 * with temperature, domoticz, some inputs, and a relay
 */
#ifdef BOARD_TEMP_DOMOTICZ

    #if defined(BOARD_TEMP_DOMOTICZ_RELAY)
    #define FIRMWARE_VERSION      LIGHTKIT_VERSION" - TEMP. DOMOTICZ RELAY"
    #elif defined(BOARD_TEMP_DOMOTICZ_BUZZER)
    #define FIRMWARE_VERSION      LIGHTKIT_VERSION" - TEMP. DOMOTICZ BUZZER"
    #else
    #define FIRMWARE_VERSION      LIGHTKIT_VERSION" - TEMP. DOMOTICZ"
    #endif

    /* MODULE DEFINITION (Use #define or #undef) */
    #define MODULE_TEMPERATURE
    #define MODULE_DOMOTICZ
    #define MODULE_STATUS_LED
    #define MODULE_INPUTS
    // 2 versions of this BOARD_TEMP_DOMOTICZ
    // One with a buzzer
    #if defined(BOARD_TEMP_DOMOTICZ_BUZZER) || defined(BOARD_TEMP_DOMOTICZ_RELAY)
        #define MODULE_OUTPUTS
    #else
        #undef MODULE_OUTPUTS
    #endif
    // The other with a Relay
    #ifdef BOARD_TEMP_DOMOTICZ_RELAY
        #define MODULE_RELAY
    #else
        #undef MODULE_RELAY
    #endif
    #undef MODULE_WEBSERVER
    #undef MODULE_STRIPLED
    #define MODULE_SERIAL
    #define MODULE_TELNET
    #define MODULE_TERM

    /* MODULE TEMPERATURE */
    #define TEMP_1_WIRE_PIN                         2
    #define TEMP_SENSOR_RESOLUTION                  10
    #define TEMP_MAX_SENSOR_SUPPORTED               2
    #define TEMP_POLLING_PERIOD_MS                  1*60*1000

    /* MODULE_DOMOTICZ */
    #define DOMOTICZ_HOST                           "192.168.0.32"
    #define DOMOTICZ_PORT                           8080
    #define DOMOTICZ_TIMEOUT_MS                     3*1000
    #define DOMOTICZ_SENSOR_ID_INSIDE               3
    #define DOMOTICZ_SENSOR_ID_OUTSIDE              4

    /* MODULE_STATUS_LED */
    #define STATUS_LED_TYPE                         STATUS_LED_TYPE_NEOPIXEL  /** LED Type, see constant below */
    #define STATUS_LED_NEOPIXEL_PIN                 14                    /** If Neopixel, indicates the NeoPixel Pin */
    #define STATUS_LED_NB_LED                       1                     /** Number of led in the neopixel chain */
    #define STATUS_LED_NEOPIXEL_BRIGHTNESS          100                   /** Brightness of the neopixel LED [0-255] */
    #define STATUS_LED_NEOPIXEL_RED                 255                   /** Brightness of red led */
    #define STATUS_LED_NEOPIXEL_GREEN               255                   /** Brightness of green led */
    #define STATUS_LED_NEOPIXEL_BLUE                255                   /** Brightness of blue led */

    /* MODULE_INPUTS */
    #define INPUTS_COUNT                           1                     /** Number of inputs managed by the module */
    #define INPUTS_PINS                            {0}                   /** Define the pin of the input with following format: {x, y, z} */
    #define INPUTS_MODES                           {I_U}                 /** Define the init mode of the input pin (N, U or A) */
    #define INPUTS_LONG_HOLD_TIME                  3*1000                /** Hold time in ms for long press on input */
    // Aliases
    #define INPUTS_OPT_ALARM_IMPULSION_MODE_EN     0                     /** Option jumper JP2 define alarm method used for relay or buzzer (Impulsion of constant) */

    /* MODULE_OUTPUTS */
    #define OUTPUTS_COUNT                           2                    /** Number of outputs managed by the module */
    #define OUTPUTS_PINS                            {12}                 /** Define the pin of the output with following format: {x, y, z} */
    // Aliases
    #define OUTPUTS_BUZZER                          0                    /** Output for the buzzer is the first output declared above */
    #define OUTPUTS_RELAIS_CMD                      0                     /** Output for the buzzer is the first output declared above */

    /* MODULE_RELAY */
    #define RELAY_FEEDBACK_PIN                      16                    /** Define the input to use to get the feedback information */
    #define RELAY_CHECK_PERIOD                      60*1000               /** Delays between 2 relay checks in ms */
    #define RELAY_CHECK_BEFORE_ERROR                3                     /** Try n times to resend commmand before going to error */
    #undef RELAY_IS_BISTABLE                                              /** Define this if the relay is bistable, undef it for monostable */
    #define RELAY_CMD_ALIAS                         OUTPUTS_RELAIS_CMD    /** Define the output to use for monostable relay command */

    /* MODULE_BUZZER */
    #define BUZZER_PIN                              12                    /** Pin of the connected buzzer */

    /** SCRIPT */
    #define SCRIPT_DOMOTICZ_UPT_PERIOD              10*60*1000            /** Period of time between two domoticz transactions */
    #define SCRIPT_TEMP_CHECK_PERIOD                10*1000               /** Period between two sensor mesures */

    #define METHOD_THRESHOLD                        0                     /** Alert is set active when sensor value is above a specified threshold */
    #define METHOD_DIFFERENTIAL                     1                     /** Alert is set active when the mathematical value difference is above a specified threshold */
    #define SCRIPT_TEMP_ALERT_METHOD                METHOD_THRESHOLD      /** Select the method of comparaison to use to trigger temp alert */

    // For METHOD_THRESHOLD
    #define SCRIPT_TEMP_ALERT_SENSOR_0              27                    /** High level in degrees */
    #define SCRIPT_TEMP_ALERT_SENSOR_1              30                    /** Low level in degrees */

    // For METHOD_DIFFERENTIAL
    #define SCRIPT_TEMP_ALERT_DIFF_THRESHOLD        1.0                   /** Threshold  in degrees to overpass before setting alert active (Don't forget to add SCRIPT_TEMP_ALERT_HYSTERESIS when choosing this value) */

    // For both methods
    #define SCRIPT_TEMP_ALERT_HYSTERESIS            1.0                   /** Hysteresis in degrees (In both ways -1/+1°C) */

    #define SCRIPT_RELAY_IMPULSION_DURATION         3*1000                /** Duration of the impulsion when the alert is triggered */
    #define SCRIPT_RELAY_MS_BEFORE_2ND_IMPULSION    2*1000                /** Duration in ms before sending a second impulsion when alert is turning on (Set to 0 to disable 2nd impulsion) */
    #define SCRIPT_BUZZER_PULSE_ON_MS               300                   /** When using pulsed buzzer, this is the ON duration in ms */
    #define SCRIPT_BUZZER_PULSE_OFF_MS              1700                  /** When using pulsed buzzer, this is the OFF duration in ms */

    /** Compilation checks */
    #if defined(BOARD_TEMP_DOMOTICZ_RELAIS) && defined(BOARD_TEMP_DOMOTICZ_BUZZER)
        #error BOARD_TEMP_DOMOTICZ_RELAIS cannot be defined with BOARD_TEMP_DOMOTICZ_BUZZER
    #endif
#endif /* BOARD_TEMP_DOMOTICZ */

/**
 * This is the configuration for the board
 * TEMP_TELEGRAM
 * with temperature, domoticz, some inputs, and a relay
 */
#ifdef BOARD_TEMP_TELEGRAM

    #if defined(BOARD_TEMP_TELEGRAM_RELAY)
    #define FIRMWARE_VERSION      LIGHTKIT_VERSION" - TEMP. TELEGRAM RELAY"
    #elif defined(BOARD_TEMP_TELEGRAM_BUZZER)
    #define FIRMWARE_VERSION      LIGHTKIT_VERSION" - TEMP. TELEGRAM BUZZER"
    #else
    #define FIRMWARE_VERSION      LIGHTKIT_VERSION" - TEMP. TELEGRAM"
    #endif

    /* MODULE DEFINITION (Use #define or #undef) */
    #define MODULE_TEMPERATURE
    #undef MODULE_DOMOTICZ
    #define MODULE_TELEGRAM
    #define MODULE_STATUS_LED
    #define MODULE_INPUTS
    #define MODULE_OUTPUTS
    // Relay is not needed when using buzzer
    #ifdef BOARD_TEMP_TELEGRAM_RELAY
        #define MODULE_RELAY
    #else
        #undef MODULE_RELAY
    #endif
    #undef MODULE_WEBSERVER
    #undef MODULE_STRIPLED
    #define MODULE_SERIAL
    #define MODULE_TELNET
    #define MODULE_TERM

    /* MODULE TEMPERATURE */
    #define TEMP_1_WIRE_PIN                         D4
    #define TEMP_SENSOR_RESOLUTION                  10
    #define TEMP_MAX_SENSOR_SUPPORTED               2
    #define TEMP_POLLING_PERIOD_MS                  1*60*1000

    /* MODULE_TELEGRAM */
    #define TELEGRAM_CONV_TOKEN						P_TELEGRAM_CONV_TOKEN   /** Private token use to identify the bot */

    /* MODULE_STATUS_LED */
    #define STATUS_LED_TYPE                         STATUS_LED_TYPE_NEOPIXEL  /** LED Type, see constant below */
    #define STATUS_LED_NEOPIXEL_PIN                 D5                    /** If Neopixel, indicates the NeoPixel Pin */
    #define STATUS_LED_NB_LED                       1                     /** Number of led in the neopixel chain */
    #define STATUS_LED_NEOPIXEL_BRIGHTNESS          50                    /** Brightness of the neopixel LED [0-255] */
    #define STATUS_LED_NEOPIXEL_RED                 255                   /** Brightness of red led */
    #define STATUS_LED_NEOPIXEL_GREEN               255                   /** Brightness of green led */
    #define STATUS_LED_NEOPIXEL_BLUE                255                   /** Brightness of blue led */

    /* MODULE_INPUTS */
    #define INPUTS_COUNT                            2                     /** Number of inputs managed by the module */
    #define INPUTS_PINS                             {D3, D1}              /** Define the pin of the input with following format: {x, y, z} */
    #define INPUTS_MODES                            {I_U, I_U}            /** Define the init mode of the input pin (N, U or A) */
    #define INPUTS_LONG_HOLD_TIME                   3*1000                /** Hold time in ms for long press on input */
    // Aliases
    #define INPUTS_OPT_TEMP_ALARM_EN                0                     /** Option jumper JP1 define State 0: Alarm is forced off, State 1: Alarm is triggered by temperature */
    #define INPUTS_OPT_ALARM_IMPULSION_MODE_EN      1                     /** Option jumper JP3 define alarm method used for relay or buzzer (Impulsion of constant) */

    /* MODULE_OUTPUTS */
    #define OUTPUTS_COUNT                           1                     /** Number of outputs managed by the module */
    #define OUTPUTS_PINS                            {D6}                  /** Define the pin of the output with following format: {x, y, z} */
    // Aliases
    #define OUTPUTS_BUZZER                          0                     /** Output for the buzzer is the first output declared above */
    #define OUTPUTS_RELAIS_CMD                      0                     /** Output for the buzzer is the first output declared above */

    /* MODULE_RELAY */
    #define RELAY_FEEDBACK_PIN                      D0                    /** Define the input to use to get the feedback information */
    #define RELAY_CHECK_PERIOD                      60*1000               /** Delays between 2 relay checks in ms */
    #define RELAY_CHECK_BEFORE_ERROR                3                     /** Try n times to resend commmand before going to error */
    #undef RELAY_IS_BISTABLE                                              /** Define this if the relay is bistable, undef it for monostable */
    #define RELAY_CMD_ALIAS                         OUTPUTS_RELAIS_CMD    /** Define the output to use for monostable relay command */

    /** SCRIPT */
    #define SCRIPT_TELEGRAM_UPT_PERIOD              60*60*1000			  /** Period of time between two telegram message when auto send is enabled */
    #define SCRIPT_TELEGRAM_CONN_OK_NOTIFY_PERIOD   12*60*60*1000         /** Period of time between two telegram message indicating connexion is OK (Put 0 to disable) */
    #define SCRIPT_TEMP_CHECK_PERIOD                4*1000				  /** Period between two sensor mesures */

    #define METHOD_THRESHOLD                        0                     /** Alert is set active when sensor value is above a specified threshold */
    #define METHOD_DIFFERENTIAL                     1                     /** Alert is set active when the mathematical value difference is above a specified threshold */
    #define SCRIPT_TEMP_ALERT_METHOD                METHOD_THRESHOLD      /** Select the method of comparaison to use to trigger temp alert */

    // For METHOD_THRESHOLD
    #define SCRIPT_TEMP_ALERT_SENSOR_0              28.0			      /** High level in degrees */
    #define SCRIPT_TEMP_ALERT_SENSOR_1              35.0			      /** Low level in degrees */

    // For METHOD_DIFFERENTIAL
    #define SCRIPT_TEMP_ALERT_DIFF_THRESHOLD        1.0                   /** Threshold  in degrees to overpass before setting alert active (Don't forget to add SCRIPT_TEMP_ALERT_HYSTERESIS when choosing this value) */

    // For both methods
    #define SCRIPT_TEMP_ALERT_HYSTERESIS            1.0					  /** Hysteresis in degrees (In both ways -1/+1°C) */

    #define SCRIPT_RELAY_IMPULSION_DURATION         3*1000                /** Duration of the impulsion when the alert is triggered */
    #define SCRIPT_RELAY_MS_BEFORE_2ND_IMPULSION    0	                  /** Duration in ms before sending a second impulsion when alert is turning on (Set to 0 to disable 2nd impulsion) */
    #define SCRIPT_BUZZER_PULSE_ON_MS               300                   /** When using pulsed buzzer, this is the ON duration in ms */
    #define SCRIPT_BUZZER_PULSE_OFF_MS              1700                  /** When using pulsed buzzer, this is the OFF duration in ms */

    /** Compilation checks */
    #if defined(BOARD_TEMP_TELEGRAM_RELAIS) && defined(BOARD_TEMP_TELEGRAM_BUZZER)
        #error BOARD_TEMP_TELEGRAM_RELAIS cannot be defined with BOARD_TEMP_TELEGRAM_BUZZER
    #endif
#endif /* BOARD_TEMP_TELEGRAM */

/**
 * This is the configuration for the board
 * RING
 * with neopixel support and some options pin
 */
#ifdef BOARD_RING

    #define FIRMWARE_VERSION      LIGHTKIT_VERSION" - RING"

    /* MODULE DEFINITION (Use #define or #undef) */
    #undef MODULE_TEMPERATURE
    #undef MODULE_DOMOTICZ
    #undef MODULE_STATUS_LED
    #define MODULE_INPUTS
    #undef MODULE_OUTPUTS
    #undef MODULE_RELAY
    #define MODULE_WEBSERVER
    #define MODULE_STRIPLED
    #define MODULE_SERIAL
    #define MODULE_TELNET
    #define MODULE_TERM

    /* MODULE_STRIPLED */
    #define STRIPLED_PIN                            2                     /** Output pin for the strip command */
    #define STRIPLED_NB_PIXELS                      12                    /** Number of Pixel on the strip lamp Ikea mini (12) Ikea classic = 24 */
    #define STRIPLED_DEMO_MODE_PERIOD               5000                  /** Delay betwwen two animation in demo mode (in ms) */

    /* MODULE_INPUTS */
    #define INPUTS_COUNT                           4                     /** Number of inputs managed by the module */
    #define INPUTS_PINS                            {32, 4, 25, 26}       /** Define the pin of the input with following format: {x, y, z} */
    #define INPUTS_MODES                           {I_U, I_N, I_N, I_A}  /** Define the init mode of the input pin (N, U or A) */
    #define INPUTS_LONG_HOLD_TIME                  3*1000                /** Hold time in ms for long press on input */
    // Aliases
    #define INPUTS_OPT_WEB_SERVER_DISPLAY          0                     /** Option jumper JP1 is set as the first button in the module */
    #define INPUTS_PIR_DETECTOR                    1                     /** The PIR detector is set as the second button in the module */
    #define INPUTS_PIR_DETECTOR_ENABLE             2                     /** This Jumper tell if Detector is enabled or not */
    #define INPUTS_PIR_DETECTOR_DELAY              3                     /** Analog input to set the detector duration */

    /** CUSTOM DEFINE for this board */
    #define DETECTOR_MAX_DURATION_MIN              20                    /** Maximum duration of the ON State after detector found someone (in minute) */
    #define DETECTOR_INVERSE_POLARITY              0                     /** Inverse the polarity of the detector input: 0 -> goes HIGH on trigger, 1 -> goes LOW on trigger */

    /** Compilation checks */
    #if !defined(MODULE_INPUTS)
        #error BOARD_RING require MODULE_INPUTS
    #endif
#endif /* BOARD_RING */

/**
 * This is the configuration for the board
 * NEOPIXEL_WEB_THIERRY
 * with neopixel support
 */
#ifdef BOARD_NEOPIXEL_WEB_THIERRY

    #define FIRMWARE_VERSION      LIGHTKIT_VERSION" - NEOPIXEL_WEB_THIERRY"

    /* MODULE DEFINITION (Use #define or #undef) */
    #undef MODULE_TEMPERATURE
    #undef MODULE_DOMOTICZ
    #undef MODULE_STATUS_LED
    #undef MODULE_INPUTS
    #undef MODULE_OUTPUTS
    #undef MODULE_RELAY
    #define MODULE_WEBSERVER
    #define MODULE_STRIPLED
    #define MODULE_SERIAL
    #define MODULE_TELNET
    #define MODULE_TERM

    /* MODULE_STRIPLED */
    #define STRIPLED_PIN                            2                     /** Output pin for the strip command */
    #define STRIPLED_NB_PIXELS                      140                   /** Number of Pixel on the strip */
    #define STRIPLED_DEFAULT_BRIGHTNESS_VALUE       32                    /** Brightness level used at startup [0-255] */
    #define STRIPLED_DEMO_MODE_PERIOD               5000                  /** Delay betwwen two animation in demo mode (in ms) */

#endif /* BOARD_NEOPIXEL_WEB_THIERRY */

/**
 * This is the configuration for the board
 * NEOPIXEL_WEB_GENERIC
 * with neopixel support
 */
#ifdef BOARD_NEOPIXEL_WEB

    #define FIRMWARE_VERSION      LIGHTKIT_VERSION" - NEOPIXEL_WEB"

    /* MODULE DEFINITION (Use #define or #undef) */
    #undef MODULE_TEMPERATURE
    #undef MODULE_DOMOTICZ
    #undef MODULE_STATUS_LED
    #undef MODULE_INPUTS
    #undef MODULE_OUTPUTS
    #undef MODULE_RELAY
    #define MODULE_WEBSERVER
    #define MODULE_STRIPLED
    #define MODULE_SERIAL
    #define MODULE_TELNET
    #define MODULE_TERM

    /* MODULE_STRIPLED */
    #define STRIPLED_PIN                            2                     /** Output pin for the strip command */
    #define STRIPLED_NB_PIXELS                      140                   /** Number of Pixel on the strip */
    #define STRIPLED_DEMO_MODE_PERIOD               5000                  /** Delay betwwen two animation in demo mode (in ms) */

#endif /* BOARD_NEOPIXEL_WEB */


/**
 * This is the configuration for the board
 * BOARD_LIGHTKITV2_DAVID
 *
 * Just an ESP8266 connected to a LED Strip
 * with a web server to control it with
 * HTTP requests using Homebridge
 */
#ifdef BOARD_LIGHTKITV2_DAVID

    #define FIRMWARE_VERSION      LIGHTKIT_VERSION" - LIGHTKITV2 DAVID"

    /* MODULE DEFINITION (Use #define or #undef) */
    #undef MODULE_TEMPERATURE
    #undef MODULE_DOMOTICZ
    #undef MODULE_STATUS_LED
    #undef MODULE_INPUTS
    #undef MODULE_OUTPUTS
    #undef MODULE_RELAY
    #define MODULE_WEBSERVER
    #define MODULE_STRIPLED
    #define MODULE_SERIAL
    #define MODULE_TELNET
    #define MODULE_TERM

    /* MODULE_STRIPLED */
    #define STRIPLED_PIN                            D2                    /** Output pin for the strip command */
    #define STRIPLED_NB_PIXELS                      60                    /** Number of Pixel on the strip */
    #define STRIPLED_DEMO_MODE_PERIOD               5000                  /** Delay betwwen two animation in demo mode (in ms) */

#endif /* BOARD_LIGHTKITV2_DAVID */

/**
 * This is the configuration for the board
 * BOARD_FEU_ROUGE
 *
 * Just an ESP8266 connected to an IOI2C board in order to control traffic light
 */
#ifdef BOARD_FEU_ROUGE

    #define FIRMWARE_VERSION      LIGHTKIT_VERSION" - FEU ROUGE"

    /* MODULE DEFINITION (Use #define or #undef) */
    #undef MODULE_TEMPERATURE
    #undef MODULE_DOMOTICZ
    #undef MODULE_STATUS_LED
    #define MODULE_INPUTS
    #define MODULE_OUTPUTS
    #undef MODULE_RELAY
    #undef MODULE_WEBSERVER
    #undef MODULE_STRIPLED
    #define MODULE_SERIAL
    #define MODULE_TELNET
    #define MODULE_TERM
    #define MODULE_FEU_ROUGE
    #define MODULE_BUZZER

    /** IOI2C BOARD */
    #define HAS_IOI2C_BOARD                  // We have an IOI2C board connected to I2C bus
    #define IOI2C_SDA 4                      // I2C interface to use to communicate with IOI2C boards
    #define IOI2C_SCL 5
    #define IOI2C_ADDRESSES { IOI2C_0_ADDR } // Adresses of IOI2C board (if chained, use coma separated list)

    /* MODULE_INPUTS */
    #define INPUTS_COUNT                           2                     /** Number of inputs managed by the module */
    #define INPUTS_PINS                            {io_special_ioi2c(0), io_special_ioi2c(1)}       /** Define the pin of the input with following format: {x, y, z} */
    #define INPUTS_MODES                           {I_U, I_U}            /** Define the init mode of the input pin (N, U or A) */
    #define INPUTS_LONG_HOLD_TIME                  3*1000                /** Hold time in ms for long press on input */
    // Aliases
    #define INPUTS_UNUSED_E1                       0                     /** IOI2C Addr. IOI2C_0_ADDR - Input E1 */
    #define INPUTS_DOOR_SWITCH                     1                     /** IOI2C Addr. IOI2C_0_ADDR - Input E2 */

    /* MODULE_OUTPUTS */
    #define OUTPUTS_COUNT                          7                     /** Number of outputs managed by the module */
    #define OUTPUTS_PINS                           {                        \
                                                       io_special_ioi2c(0), \
                                                       io_special_ioi2c(1), \
                                                       io_special_ioi2c(2), \
                                                       io_special_ioi2c(3), \
                                                       io_special_ioi2c(4), \
                                                       io_special_ioi2c(5), \
                                                       D5                   \
                                                   }
    // Aliases
    #define OUTPUTS_TRAFFIC_LIGHT_GREEN            0                     /** IOI2C Addr. IOI2C_0_ADDR - Output S1 */
    #define OUTPUTS_TRAFFIC_LIGHT_YELLOW           1                     /** IOI2C Addr. IOI2C_0_ADDR - Output S2 */
    #define OUTPUTS_TRAFFIC_LIGHT_RED              2                     /** IOI2C Addr. IOI2C_0_ADDR - Output S2 */
    #define OUTPUTS_UNUSED_S4                      3                     /** IOI2C Addr. IOI2C_0_ADDR - Output S4 */
    #define OUTPUTS_UNUSED_S5                      4                     /** IOI2C Addr. IOI2C_0_ADDR - Output S5 */
    #define OUTPUTS_UNUSED_S6                      5                     /** IOI2C Addr. IOI2C_0_ADDR - Output S6 */
    #define OUTPUTS_BUZZER                         6                     /** Buzzer connected to D5 through a transistor */

    /* MODULE_FEU_ROUGE */
    #define FEU_ROUGE_INIT_FCT_MODE                MODE_FCT_TRAFFIC_LIGHT         /** Look at MODE_FCT_* to define the initial functionning mode */

    /* MODULE_BUZZER */
    #define BUZZERS_COUNT                          1
    #define BUZZERS_OUTPUTS                        { \
                                                       OUTPUTS_BUZZER \
                                                   }

    // Aliases
    #define BUZZERS_ALARM                          0                     /** A buzzer to signal something */

#endif /* BOARD_FEU_ROUGE */

/* ===========================
 * CHECKS
 * ===========================
 */
#if defined(MODULE_RELAY) && !defined(MODULE_OUTPUTS)
    #error MODULE_RELAY needs MODULE_OUTPUTS to work properly
#endif

#if defined(MODULE_TELNET) && !defined(MODULE_TERM)
    #error MODULE_TELNET needs MODULE_TERM to work properly
#endif

#if defined(MODULE_SERIAL) && !defined(MODULE_TERM)
    #error MODULE_SERIAL needs MODULE_TERM to work properly
#endif

#if defined(HAS_IOI2C_BOARD) && (!defined(MODULE_INPUTS) || !defined(MODULE_OUTPUTS))
    #error HAS_IOI2C_BOARD needs MODULE_INPUTS and MODULE_OUTPUTS to work properly
#endif

#endif /** GLOBAL_HPP */