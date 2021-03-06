/**
 * @file wifi.hpp
 * @description Manage all wifi
 * settings (AP and client)
 */

#ifndef WIFI_WIFI_HPP
#define WIFI_WIFI_HPP

#include "global.hpp"
#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#include <mDNS.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#endif

// Constants
#define WIFI_CHECK_PERIOD          1000
#define WIFI_SSID_MAX_LEN          30
#define WIFI_PASSWORD_MAX_LEN      30
#define WIFI_DELAYED_CONFIG_MS     100
#define WIFI_SCAN_MIN_INTERVAL_MS  (24 * 60 * 60 * 1000)
#define WIFI_MAX_CO_MIN            1
#define WIFI_MAX_CO_MAX            3
#define WIFI_CHANNEL_MIN           1
#define WIFI_CHANNEL_MAX           13
#define WIFI_DELAY_AP_FALLBACK_MIN 5000

#define IP_TO_U32(a, b, c, d) ((uint32_t)((d << 24) | (c << 16) | (b << 8) | a))

#define WIFI_DEFAULT_MODE              MODE_AP
#define WIFI_DEFAULT_AP_SSID           "LightKit_SSID"
#define WIFI_DEFAULT_AP_PASSWORD       "bravo42beta"
#define WIFI_DEFAULT_AP_CHANNEL        1
#define WIFI_DEFAULT_AP_MAXCO          1
#define WIFI_DEFAULT_AP_IP             IP_TO_U32(192, 168, 4, 1)
#define WIFI_DEFAULT_AP_GATEWAY        IP_TO_U32(192, 168, 4, 254)
#define WIFI_DEFAULT_AP_SUBNET         IP_TO_U32(255, 255, 255, 0)
#define WIFI_DEFAULT_CLIENT_SSID       P_WIFI_CLIENT_SSID
#define WIFI_DEFAULT_CLIENT_PASSWORD   P_WIFI_CLIENT_PWD
#define WIFI_DEFAULT_DELAY_AP_FALLBACK 20000

// Structures and enums
typedef enum
{
	MODE_NONE = 0,
	MODE_AP,
	MODE_CLIENT,
	MODE_SCAN,

	MODE_MAX
} WIFI_MODE_E;

#ifdef WIFI_CPP
static const char outOfRangeStr[]  = "<unknown>";
static const char wifiModeStr[][7] = {
	"NONE",
	"AP",
	"CLIENT",
	"SCAN",
	"MAX"
};
#endif

/**
 * Store all wifi settings for AP and Client mode
 * @warning : Don't forget to increment FLASH_STRUCT_VERSION
 * if this structure is modified !
 */
typedef struct {
	WIFI_MODE_E mode;       // The actual mode in use
	WIFI_MODE_E userMode;   // The mode the user configured
	WIFI_MODE_E forcedMode; // The mode defined by the module itself effective after 1 reset only

	// WIFI_MODE_AP
	struct {
		char     ssid[WIFI_SSID_MAX_LEN];
		char     password[WIFI_PASSWORD_MAX_LEN];
		uint8_t  channel;       // Wifi Channel [WIFI_CHANNEL_MIN; WIFI_CHANNEL_MAX] (Default = WIFI_CHANNEL_MIN)
		uint8_t  maxConnection; // Max connection supported by AP
		uint8_t  isHidden : 1;  // Tell if SSID is hidden
		uint32_t ip;            // Local ip address
		uint32_t gateway;       // Gateway address
		uint32_t subnet;        // Netmask of the network
	} ap;

	// WIFI_MODE_CLIENT
	struct {
		char ssid[WIFI_SSID_MAX_LEN];
		char password[WIFI_PASSWORD_MAX_LEN];

		uint32_t delayBeforeAPFallbackMs; // Number of ms when cannot connect as client after boot before rebooting in AP mode
		uint32_t lastIp;                  // Last valid IP address used last time we got a successful connection as client
	} client;
} wifi_handle_t;

typedef struct {
	uint8_t     isValid;
	WIFI_MODE_E mode;
	uint8_t     channel;
	uint8_t     bssid[6];
} wifi_fast_reconnect_t;

// PROTOTYPES
void            wifi_print(void);
void            wifi_print_config(wifi_handle_t * pHandle);
wifi_handle_t * wifi_get_handle(void);
int32_t         wifi_use_new_settings(wifi_handle_t * pWifiHandle, String & reason);
int32_t         wifi_use_default_settings(void);
int32_t         wifi_start_scan_req(uint32_t delay);
int             wifi_init(void);
void            wifi_main(void);

#endif /* WIFI_WIFI_HPP */