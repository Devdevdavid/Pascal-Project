/**
  * @file   web_server.cpp
  * @brief  Handle embedded web server
  * @author David DEVANT
  * @date   12/08/2018
  */

#ifdef ESP32
#include <ESP32WebServer.h>
#include <Update.h>
#else
#include <ESP8266WebServer.h>
#endif
#include <ArduinoJson.h>
#include <StreamString.h>
#include <string>

#include "cmd/cmd.hpp"
#include "file_sys/file_sys.hpp"
#include "global.hpp"
#include "io/inputs.hpp"
#include "script/script.hpp"
#include "stripled/stripled.hpp"
#include "web_server.hpp"
#include "wifi/wifi.hpp"

#ifdef MODULE_WEBSERVER

// Main handle for webserver
G_WebServer server(WEB_SERVER_HTTP_PORT);

// Internals
static String   updaterError;
static uint32_t updateBufferMaxSpace = 0;

static String getContentType(String filename)
{
	if (server.hasArg("download"))
		return "application/octet-stream";
	else if (filename.endsWith(".htm"))
		return "text/html";
	else if (filename.endsWith(".html"))
		return "text/html";
	else if (filename.endsWith(".css"))
		return "text/css";
	else if (filename.endsWith(".js"))
		return "application/javascript";
	else if (filename.endsWith(".png"))
		return "image/png";
	else if (filename.endsWith(".gif"))
		return "image/gif";
	else if (filename.endsWith(".jpg"))
		return "image/jpeg";
	else if (filename.endsWith(".ico"))
		return "image/x-icon";
	else if (filename.endsWith(".xml"))
		return "text/xml";
	else if (filename.endsWith(".pdf"))
		return "application/x-pdf";
	else if (filename.endsWith(".zip"))
		return "application/x-zip";
	else if (filename.endsWith(".gz"))
		return "application/x-gzip";
	return "text/plain";
}

static bool handle_file_read(String path)
{
	log_info("Received : %s", path.c_str());
	if (path.endsWith("/")) {
		path += "index.html";
	}
	String contentType = getContentType(path);
	if (file_sys_exist(path)) {
		File file = file_sys_open(path, "r");
		server.streamFile(file, contentType);
		file.close();
		return true;
	}
	return false;
}

static void handle_bad_parameter(void)
{
	server.send(200, "text/plain", "Bad parameter");
}

static void handle_update_done(void)
{
	// if (!_authenticated)
	//     return server.requestAuthentication();
	server.client().setNoDelay(true);
	String msg = String(Update.getError());
	server.sendHeader("Connection", "close");
	server.send(200, "text/plain", msg);

	if (!Update.hasError()) {
		delay(100);
		server.client().stop();

		// Reset to apply in 1s to let time to send HTTP response
		script_delayed_reset(1000);
	}
}

/**
 * @brief Show usage information on buffer usage
 * @details We don't have the actual file size
 * information so we display usage instead
 *
 * @param progress 	Size received
 * @param size 		Max size of the buffer
 */
static void handle_update_data_progress(uint32_t progress, uint32_t size)
{
	log_info("Buffer usage: %d%% (%d kB)", (progress * 100) / size, progress / 1000);
}

static void handle_update_error()
{
	StreamString str;
	Update.printError(str);
	updaterError = str.c_str();
}

static void handle_update_data(void)
{
	// handler for the file upload, get's the sketch bytes, and writes
	// them through the Update object
	HTTPUpload & upload = server.upload();

	if (upload.status == UPLOAD_FILE_START) {
		updaterError.clear();

		log_info("Update: %s (%s)\n", upload.filename.c_str(), upload.name.c_str());

		if (upload.name == "filesystem") {
			updateBufferMaxSpace = file_sys_get_max_size();

			if (!Update.begin(updateBufferMaxSpace, U_CMD_FS)) {
				handle_update_error();
			}
		} else {
			updateBufferMaxSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;

			if (!Update.begin(updateBufferMaxSpace, U_FLASH)) {
				handle_update_error();
			}
		}
	} else if (upload.status == UPLOAD_FILE_WRITE && !updaterError.length()) {
		handle_update_data_progress(upload.totalSize, updateBufferMaxSpace);

		if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
			handle_update_error();
		}
	} else if (upload.status == UPLOAD_FILE_END && !updaterError.length()) {
		if (Update.end(true)) { //true to set the size to the current progress
			log_info("Update Success: %ukB\n", upload.totalSize / 1000);
		} else {
			handle_update_error();
		}
	} else if (upload.status == UPLOAD_FILE_ABORTED) {
		Update.end();
		log_warn("Update was aborted");
	}
	delay(0);
}

/**
 * Send 0 if LED are disabled and 1 if LED are enabled
 */
static void handle_get_version(void)
{
	server.send(200, "text/plain", FIRMWARE_VERSION);
}

/**
 * Send 0 if LED are disabled and 1 if LED are enabled
 */
static void handle_get_animation(void)
{
	server.send(200, "text/plain", String(cmd_get_animation()));
}

/**
 * Set the current animation
 */
static void handle_set_animation(void)
{
	int32_t  ret;
	uint16_t animID;

	if (!server.hasArg("v")) {
		handle_bad_parameter();
		return;
	}

	animID = server.arg("v").toInt();

	ret = cmd_set_animation(animID);
	if (ret != 0) {
		log_error("cmd_set_animation() failed: ret = %d", ret);
	}

	server.send(200, "text/plain", "");
}

/**
 * Send 0 if LED are disabled and 1 if LED are enabled
 */
static void handle_get_demo_mode(void)
{
	server.send(200, "text/plain", String(cmd_get_demo_mode() ? 1 : 0));
}

/**
 * If arg "state" is 1, turn LED on, if not, turn led off
 */
static void handle_set_demo_mode(void)
{
	if (!server.hasArg("v")) {
		handle_bad_parameter();
		return;
	}

	cmd_set_demo_mode(server.arg("v").toInt() == 1);

	handle_get_demo_mode();
}

/**
 * Send 0 if LED are disabled and 1 if LED are enabled
 */
static void handle_get_state(void)
{
	server.send(200, "text/plain", String(cmd_get_state() ? 1 : 0));
}

/**
 * If arg "state" is 1, turn LED on, if not, turn led off
 */
static void handle_set_state(void)
{
	if (!server.hasArg("v")) {
		handle_bad_parameter();
		return;
	}

	cmd_set_state(server.arg("v").toInt() == 1);

	handle_get_state();
}

/**
 * Send the brightness value [0; 100]
 */
static void handle_get_brightness(void)
{
	server.send(200, "text/plain", String(cmd_get_brightness()));
}

/**
 * Get the arg "v" and set the current brightness level [0; 100]
 */
static void handle_set_brightness(void)
{
	if (!server.hasArg("v")) {
		handle_bad_parameter();
		return;
	}

	uint8_t level = server.arg("v").toInt();

	cmd_set_brightness(level);

	handle_get_brightness();
}

/**
 * Send the nb of LED value [1; STRIP_LED_MAX_NB_PIXELS]
 */
static void handle_get_nb_led(void)
{
	server.send(200, "text/plain", String(cmd_get_nb_led()));
}

/**
 * Get the arg "v" and set the number of LED [0; STRIP_LED_MAX_NB_PIXELS]
 */
static void handle_set_nb_led(void)
{
	if (!server.hasArg("v")) {
		handle_bad_parameter();
		return;
	}

	uint8_t nbLed = server.arg("v").toInt();

	cmd_set_nb_led(nbLed);
	handle_get_nb_led();
}

/**
 * Send the color currently configured
 */
static void handle_get_color(void)
{
	uint32_t color = cmd_get_color();
	String   hexColor;

	// Remove alpha channel
	color &= ~(0xFF000000);

	hexColor = String(color, HEX);

	// Add leading zeros
	while (hexColor.length() < 6) {
		hexColor = "0" + hexColor;
	}

	server.send(200, "text/plain", hexColor);
}

/**
 * Get the arg "v" and set the color of some LED animation
 */
static void handle_set_color(void)
{
	if (!server.hasArg("v")) {
		handle_bad_parameter();
		return;
	}

	uint32_t color = (uint32_t) strtol(server.arg("v").c_str(), 0, 16);

	// Append alpha channel with max value
	color |= 0xFF000000;

	cmd_set_color(color);
	handle_get_color();
}

/**
 * Send the information about display
 */
static void handle_get_display_info(void)
{
	/**
   *  0x01: Hide all but keep color configuration (used by BOARD_RING)
   *  0x02 to 0x80: unused
   **/
	uint8_t displayInfo = 0x00;

#if defined(BOARD_RING)
	if (is_input_low(INPUTS_OPT_WEB_SERVER_DISPLAY)) {
		_set(displayInfo, 0x01);
	}
#endif

	server.send(200, "text/plain", String(displayInfo));
}

/**
 * @brief Send all wifi settings
 */
static void handle_get_wifi_settings(void)
{
	wifi_handle_t *     wifiHandle;
	DynamicJsonDocument json(1024);
	String              jsonString = "";

	wifiHandle = wifi_get_handle();

	json["userMode"]                          = wifiHandle->userMode;
	json["ap"]["ssid"]                        = wifiHandle->ap.ssid;
	json["ap"]["password"]                    = wifiHandle->ap.password;
	json["ap"]["channel"]                     = wifiHandle->ap.channel;
	json["ap"]["maxConnection"]               = wifiHandle->ap.maxConnection;
	json["ap"]["isHidden"]                    = wifiHandle->ap.isHidden == 1;
	IPAddress ip                              = IPAddress(wifiHandle->ap.ip);
	json["ap"]["ip"]                          = ip.toString();
	IPAddress gateway                         = IPAddress(wifiHandle->ap.gateway);
	json["ap"]["gateway"]                     = gateway.toString();
	IPAddress subnet                          = IPAddress(wifiHandle->ap.subnet);
	json["ap"]["subnet"]                      = subnet.toString();
	json["client"]["ssid"]                    = wifiHandle->client.ssid;
	json["client"]["password"]                = wifiHandle->client.password;
	json["client"]["delayBeforeAPFallbackMs"] = wifiHandle->client.delayBeforeAPFallbackMs;
	IPAddress lastIp                          = IPAddress(wifiHandle->client.lastIp);
	json["client"]["lastIp"]                  = lastIp.toString();

	serializeJson(json, jsonString);
	server.send(200, "text/plain", jsonString);
}

/**
 * @brief Receive new wifi settings
 */
static void handle_set_wifi_settings(void)
{
	String              reason = "";
	int32_t             ret    = 0;
	wifi_handle_t       wifiHandleTmp;
	DynamicJsonDocument json(1024);
	IPAddress           ip;
	IPAddress           gateway;
	IPAddress           subnet;

	if (!server.hasArg("v")) {
		handle_bad_parameter();
		return;
	}

	deserializeJson(json, server.arg("v"));

	if (json["use_default"] == true) {
		ret = wifi_use_default_settings();
	} else {

		// Copy data
		wifiHandleTmp.userMode = json["userMode"];
		strncpy(wifiHandleTmp.ap.ssid, json["ap"]["ssid"].as<char *>(), WIFI_SSID_MAX_LEN);
		strncpy(wifiHandleTmp.ap.password, json["ap"]["password"].as<char *>(), WIFI_PASSWORD_MAX_LEN);
		wifiHandleTmp.ap.channel       = json["ap"]["channel"];
		wifiHandleTmp.ap.maxConnection = json["ap"]["maxConnection"];
		wifiHandleTmp.ap.isHidden      = json["ap"]["isHidden"].as<bool>();

		ip.fromString(json["ap"]["ip"].as<char *>());
		wifiHandleTmp.ap.ip = (uint32_t) ip;
		gateway.fromString(json["ap"]["gateway"].as<char *>());
		wifiHandleTmp.ap.gateway = (uint32_t) gateway;
		subnet.fromString(json["ap"]["subnet"].as<char *>());
		wifiHandleTmp.ap.subnet = (uint32_t) subnet;

		strncpy(wifiHandleTmp.client.ssid, json["client"]["ssid"].as<char *>(), WIFI_PASSWORD_MAX_LEN);
		strncpy(wifiHandleTmp.client.password, json["client"]["password"].as<char *>(), WIFI_SSID_MAX_LEN);
		wifiHandleTmp.client.delayBeforeAPFallbackMs = json["client"]["delayBeforeAPFallbackMs"];

		ret = wifi_use_new_settings(&wifiHandleTmp, reason);
	}

	if (ret == 0) {
		log_info("Using new wifi settings");
		server.send(200, "text/plain", "ok");
	} else {
		log_info("Failed to update wifi settings: %s", reason.c_str());
		server.send(200, "text/plain", reason);
	}
}

/**
 * @brief Receive new wifi settings
 */
static void handle_get_wifi_scans(void)
{
	DynamicJsonDocument json(1024);
	String              jsonString = "";
	int32_t             scanCount, i;
	JsonArray           ssidList, rssiList;

	// Start scan in 1s if no result ready yet to let
	// time to http server to send the reply below
	scanCount = wifi_start_scan_req(1000);

	if (scanCount >= 0) {
		json["scanCount"] = scanCount;

		ssidList = json.createNestedArray("ssid");
		rssiList = json.createNestedArray("rssi");

		for (i = 0; i < scanCount; ++i) {
			ssidList.add(WiFi.SSID(i));
			rssiList.add(WiFi.RSSI(i));
		}
	} else {
		json["scanCount"] = -1;
	}

	serializeJson(json, jsonString);
	server.send(200, "text/plain", jsonString);
}

static void handle_get_module_name(void)
{
	server.send(200, "text/plain", cmd_get_module_name());
}

static void handle_set_module_name(void)
{
	if (!server.hasArg("v")) {
		handle_bad_parameter();
		return;
	}

	log_info("Using \"%s\" as new module name", server.arg("v").c_str());

	cmd_set_module_name(server.arg("v"));
	handle_get_module_name();
}

int web_server_init(void)
{
	server.begin();

	// --- Firmware Upload ---
	server.on(
	"/update", HTTP_POST, []() { handle_update_done(); }, []() { handle_update_data(); });

	// --- Get/Set interface ---
	server.on("/get_version", HTTP_GET, []() {
		handle_get_version();
	});
	server.on("/get_animation", HTTP_GET, []() {
		handle_get_animation();
	});
	server.on("/set_animation", HTTP_GET, []() {
		handle_set_animation();
	});
	server.on("/get_demo_mode", HTTP_GET, []() {
		handle_get_demo_mode();
	});
	server.on("/set_demo_mode", HTTP_GET, []() {
		handle_set_demo_mode();
	});
	server.on("/get_state", HTTP_GET, []() {
		handle_get_state();
	});
	server.on("/set_state", HTTP_GET, []() {
		handle_set_state();
	});
	server.on("/get_brightness", HTTP_GET, []() {
		handle_get_brightness();
	});
	server.on("/set_brightness", HTTP_GET, []() {
		handle_set_brightness();
	});
	server.on("/get_nb_led", HTTP_GET, []() {
		handle_get_nb_led();
	});
	server.on("/set_nb_led", HTTP_GET, []() {
		handle_set_nb_led();
	});
	server.on("/get_color", HTTP_GET, []() {
		handle_get_color();
	});
	server.on("/set_color", HTTP_GET, []() {
		handle_set_color();
	});
	server.on("/get_display_info", HTTP_GET, []() {
		handle_get_display_info();
	});
	server.on("/get_wifi_settings", HTTP_GET, []() {
		handle_get_wifi_settings();
	});
	server.on("/set_wifi_settings", HTTP_GET, []() {
		handle_set_wifi_settings();
	});
	server.on("/get_wifi_scans", HTTP_GET, []() {
		handle_get_wifi_scans();
	});
	server.on("/set_module_name", HTTP_GET, []() {
		handle_set_module_name();
	});
	server.on("/get_module_name", HTTP_GET, []() {
		handle_get_module_name();
	});

	// --- File management ---
	server.onNotFound([]() {
		if (!handle_file_read(server.uri())) {
			server.send(404, "text/plain", "File Not Found");
		}
	});

	log_info("HTTP server started");
	return 0;
}

void web_server_main(void)
{
	server.handleClient();
}

#endif /* MODULE_WEBSERVER */