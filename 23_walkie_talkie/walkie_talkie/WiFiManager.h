#pragma once

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>    // yes this is correct
#include <EEPROM.h>         // simulated EEPROM: https://arduino-pico.readthedocs.io/en/latest/eeprom.html

/**
 * @brief WiFiManager provides captive portal and network configuration management to allow devices
 * to connect to a local network SSID and password.
 *
 * Derived from the excellent https://www.smartlab.at/implement-an-esp32-hot-spot-that-runs-a-captive-portal/ and
 * https://stackoverflow.com/questions/54583818/esp-auto-login-accept-message-by-os-with-redirect-to-page-like-public-wifi-port
 */

#define SSID_OR_PWD_MAXLEN   32

class WiFiManager
{
public:
    WiFiManager(char const *apName, char const *apPassword, bool serialLog = false);
    ~WiFiManager();

    bool autoConnect();
    void setContentText(String& title, String& name, String& shortname, String& maker, String& version);
    void setContentMainImage(const char* img);

    inline static const char *HOSTNAME = "picow";    // hostname for mDNS to enable http://picow.local

private:
    // Used to load/save credentials and CRC32 as struct in EEPROM.
    typedef struct wifi_credentials_t {
        char      _ssid[SSID_OR_PWD_MAXLEN + 1];
        char      _pwd[SSID_OR_PWD_MAXLEN + 1];
        uint32_t  _crc;                             // CRC32 check
    } wifi_credentials_t;

    // state machine states
    typedef enum State {    // _currentState, _previousState;
        START,
        LOAD_CREDENTIALS,   // -> (check for WiFi credentials in EEPROM) ? CONNECT_TO_WLAN : SHOW_PORTAL    // _connectionAttempts reset; setupConfigPortal false; APrunning false;
        CONNECT_TO_WLAN,    // -> (connection successful) ? STOP : RETRY_CONNECTION                         //  exceeded retries, then _accessPointRunning false to reinitalise AP
        RETRY_CONNECTION,   // -> (num connection attempts > 3) ? SHOW_PORTAL : CONNECT_TO_WLAN             // _connectionAttempts++
        SHOW_PORTAL,        // -> (WLAN configured) ? SAVE_CREDS : SHOW_PORTAL
        SAVE_CREDENTIALS,   // -> (save credentials) then CONNECT_TO_WLAN                                   // _connectionAttempts reset
        STOP
    } State_t;

    // heart of the machine
    void cycleStateMachine();

    // wifi connection methods
    int8_t connectWifi() const;
    const char* getWLStatus(const int8_t wlStatus) const;

    // captive portal methods
    bool startConfigPortal();
    bool redirectToPortal() const;
    boolean isIp(const String& str) const;

    // credential EEPROM storage
    bool loadCredentials();
    void saveCredentials();

    // webserver handler callbacks
    void handleRoot() const;
    void handleWifi() const;
    void handleWifiSave();
    void handleNotFound() const;

    // html helpers
    void sendStandardHeaders() const;
    void getSignalStrength(String& cssStyle, const int32_t rssi) const;

    uint32_t crc;
    void reset_crc32();
    void add_crc32(const uint8_t *data, size_t length);
    uint32_t calc_crc32();

// members
    State_t _currentState;              // current state of network configuration state machine
    State_t _previousState;             // previous state of network configuration state machine
    uint8_t _connectionAttempts;        // count of connection attempts
    bool    _setupConfigPortal;         // true if web/dns servers have already been setup for captive portal
    bool    _accessPointRunning;        // true if the AP WiFi is running to support captive portal

    bool    _serialLog;                 // true if Serial log output is required for debugging

    // Portal content related
    const char* _hero_img;              // base64 image reference
    String _title;
    String _name;
    String _shortname;
    String _maker;
    String _version;

    // RSSI thresholds for displaying strength indicators
    inline static const int32_t STRONG_RSSI_THRESHOLD = -55;
    inline static const int32_t MEDIUM_RSSI_THRESHOLD = -80;

    // Access Point network parameters
    static const IPAddress AP_IP;

    // DNS related
    inline static const byte DNS_PORT = 53;

    // WiFi connection consts
    inline static const uint8_t  MAX_CONNECTION_ATTEMPTS   = 3;       // max number of attempts to connect to WiFi
    inline static const uint16_t MAX_CONNECTION_TIMEOUT_MS = 10000;   // num of ms to wait for successful WiFi connection

    // Target WLAN SSID and password
    char _wlanSSID[SSID_OR_PWD_MAXLEN + 1];
    char _wlanPassword[SSID_OR_PWD_MAXLEN + 1];

    // Access point SSID and password
    char _apSSID[SSID_OR_PWD_MAXLEN + 1];
    char _apPassword[SSID_OR_PWD_MAXLEN + 1];
};
