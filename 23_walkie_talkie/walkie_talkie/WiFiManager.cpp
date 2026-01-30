#include <LittleFS.h>
#include "WiFiManager.h"
#include "WiFiManagerAssets.h"

/*
Tasks
- unit tests for EEPROM creds
- validation of ssid, pwd for type and length
*/

// Logging macros
#define LOGF_ERROR(fmt, ...)    if (_serialLog) { Serial.print("[E] "); Serial.printf(fmt, __VA_ARGS__); Serial.println(); }
#define LOGF_DEBUG(fmt, ...)    if (_serialLog) { Serial.print("[D] "); Serial.printf(fmt, __VA_ARGS__); Serial.println(); }
#define LOG_DEBUG(s)            if (_serialLog) { Serial.print("[D] "); Serial.println(s); }

// DNS server
DNSServer dnsServer;

// Web server
WebServer server(80);

static const char *_LOG_PREFIX = "[WiFiManager] ";
static uint32_t crc32_table[256];
#define POLYNOMIAL 0xEDB88320

// AccessPoint network config
const IPAddress WiFiManager::AP_IP = IPAddress(192,168,42,1);

WiFiManager::WiFiManager(const char *apName, const char *apPassword, bool serialLog) :
    _wlanSSID(""),
    _wlanPassword(""),
    _serialLog(serialLog),
    _setupConfigPortal(false),
    _accessPointRunning(false),
    _connectionAttempts(0),
    _currentState(State_t::START),
    _previousState(State_t::STOP)
{
    strncpy(_apSSID, apName, 32);
    strncpy(_apPassword, apPassword, 32);

    _hero_img = ::hero_img;
    _title = "PicoW";

    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (uint32_t j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ POLYNOMIAL;
            else
                crc >>= 1;
        }
        crc32_table[i] = crc;
    }

// FIXME: Is this in the correct location...
//    EEPROM.begin(512);
//call begin in sketch
}

WiFiManager::~WiFiManager()
{
    // stop WiFiManager servers so other servers downstream (e.g. aWOT) can use the same ports
    dnsServer.stop();
    server.stop();
}

bool WiFiManager::autoConnect()
{
    // iterate through state machine until we have configured WiFi
    while (_currentState != State_t::STOP)
        cycleStateMachine();

    return true;
}

// update html main image and title content
void WiFiManager::setContentText(String& title, String& name, String& shortname, String& maker, String& version)
{
    _title = title;
    _name = name;
    _shortname = shortname;
    _maker = maker;
    _version = version;
}

void WiFiManager::setContentMainImage(const char* img)
{
    _hero_img = img;
}

void WiFiManager::cycleStateMachine()
{
    if (_previousState != _currentState)
        LOGF_DEBUG("Current state %d", _currentState);

    switch(_currentState) {
        case START:
            _currentState = LOAD_CREDENTIALS;               // move to LOAD_CREDENTIALS, initialise state variables
            _accessPointRunning = false;
            _setupConfigPortal = false;
            _connectionAttempts = 0;
            break;

        case LOAD_CREDENTIALS:                              // attempt to load WiFi credentials from EEPROM
            if (loadCredentials()) {
                LOG_DEBUG(F("Credentials exist"));
                _currentState = CONNECT_TO_WLAN;
            }
            else {
                LOG_DEBUG(F("Credential CRC match fail"));
                _currentState = SHOW_PORTAL;                // CRC match failure so show captive portal
            }
            break;

        case SHOW_PORTAL:                                   // spin up captive portal
            startConfigPortal();                            // portal wifi page form submit will move the state to SAVE_CREDENTIALS
            break;

        case SAVE_CREDENTIALS:
            saveCredentials();
            _connectionAttempts = 0;
            _currentState = CONNECT_TO_WLAN;
            break;

        case CONNECT_TO_WLAN:                               // attempt to connect and retry
            if (connectWifi() != WL_CONNECTED) {
                _connectionAttempts++;
                delay(500);                                 // give it some time
            }
            else {
                _currentState = STOP;
            }

            if (_connectionAttempts > MAX_CONNECTION_ATTEMPTS-1) {
                _accessPointRunning = false;                // reinitialise AP
                _currentState = SHOW_PORTAL;
            }
            break;

        case STOP:                                          // done: WiFi creds configured and connected to
            break;

        default:
            LOGF_ERROR("Invalid state %d", _currentState);
            break;
    }

    // if the config portal has been setup, handle dns and web requests
    if (_setupConfigPortal) {
        dnsServer.processNextRequest();
        server.handleClient();
    }

    if (_previousState != _currentState)
        LOGF_DEBUG("Transitioned to state %d", _currentState);

    _previousState = _currentState;
}

// Is str an IP address
boolean WiFiManager::isIp(const String& str) const
{
    for (size_t i = 0; i < str.length(); i++)
    {
        int c = str.charAt(i);
        if (c != '.' && (c < '0' || c > '9'))
        {
            return false;
        }
    }
    return true;
}

/**
 * Convert WL_STATUS into String for logging.
 */
const char* WiFiManager::getWLStatus(const int8_t wlStatus) const
{
    switch (wlStatus)
    {
        case WL_IDLE_STATUS:
            return "WL_IDLE_STATUS";
        case WL_NO_SSID_AVAIL:
            return "WL_NO_SSID_AVAIL";
        case WL_SCAN_COMPLETED:
            return "WL_SCAN_COMPLETED";
        case WL_CONNECTED:
            return "WL_CONNECTED";
        case WL_CONNECT_FAILED:
            return "WL_CONNECT_FAILED";
        case WL_CONNECTION_LOST:
            return "WL_CONNECTION_LOST";
        case WL_DISCONNECTED:
            return "WL_DISCONNECTED";
        default:
            return "Unknown";
    }
}

/**
 * Connect to configured WLAN WiFi
 *
 * @return int8_t connection result
 */
int8_t WiFiManager::connectWifi() const
{
    LOGF_DEBUG("Connecting as wifi client to SSID %s", _wlanSSID);

    WiFi.disconnect();
    WiFi.begin(_wlanSSID, _wlanPassword);
    int8_t res = WiFi.waitForConnectResult(MAX_CONNECTION_TIMEOUT_MS);

    LOGF_DEBUG("Connection result %s", getWLStatus(res));
    return res;
}

/**
 * Create access point and register and serve web content for
 * the captive portal.
 */
bool WiFiManager::startConfigPortal()
{
    if (!_accessPointRunning) {
        // Go into AP mode, configure IP (so DNS can use it)
        WiFi.config(AP_IP);
        WiFi.beginAP(_apSSID, _apPassword);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            LOG_DEBUG(".");  // waiting
        }

        delay(500);             // wait for AP to stabalise

        String ip = WiFi.localIP().toString();
        LOGF_DEBUG("AccessPoint IP address %s", ip.c_str());
        _accessPointRunning = true;
    }

    // only setup config portal once
    if (!_setupConfigPortal)
    {
        LOG_DEBUG(F("Starting config portal"));

        // Setup the DNS server redirecting all domains to the AP_IP
        dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        dnsServer.start(DNS_PORT, "*", AP_IP);

        // Setup web pages: root, wifi config and not found
        server.on("/", std::bind(&WiFiManager::handleRoot, this));
        server.on("/wifi", std::bind(&WiFiManager::handleWifi, this));
        server.on("/wifisave", std::bind(&WiFiManager::handleWifiSave, this));
        server.onNotFound(std::bind(&WiFiManager::handleNotFound, this));
        server.begin();             // Web server start

        LOG_DEBUG(F("HTTP server started"));
        _setupConfigPortal = true;
    }

    return _setupConfigPortal;
}

/** 
 * Redirect to captive portal if we got a request for another domain. 
 * Return true in that case so the page handler does not try to handle the request again.
 */
bool WiFiManager::redirectToPortal() const
{
    if (!isIp(server.hostHeader()) && server.hostHeader() != (String(HOSTNAME) + ".local"))
    {
        LOG_DEBUG(F("Request redirected to captive portal"));

        IPAddress ip = server.client().localIP();
        server.sendHeader("Location", String("http://") + ip.toString(), true);
        server.send(302, "text/plain", "");     // Empty content inhibits Content-length header so we have to close the socket ourselves.
        server.client().stop();                 // Stop is needed because we sent no content length

        return true;
    }
    return false;
}

/**
 * Send standard HTTP headers
 */
void WiFiManager::sendStandardHeaders() const
{
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
}

/** 
 * Handle root or redirect to captive portal
 */
void WiFiManager::handleRoot() const
{
    // If captive portal then redirect instead of displaying the page
    if (redirectToPortal()) { 
        return;
    }

    sendStandardHeaders();
    String html(root_template);
    html.replace("${hero.img}",           hero_img);
    html.replace("${firmware.name}",      _name);
    html.replace("${firmware.shortname}", _shortname);
    html.replace("${firmware.maker}",     _maker);
    html.replace("${firmware.version}",   _version);
    server.send(200, "text/html", html);
}

/**
 * Converts RSSI into a CSS style to indicate WiFi network signal strength.
 */
void WiFiManager::getSignalStrength(String& cssStyle, const int32_t rssi) const
{
    if (rssi < STRONG_RSSI_THRESHOLD)
        cssStyle = "strong";
    else if(rssi < MEDIUM_RSSI_THRESHOLD)
        cssStyle = "medium";
    else
        cssStyle = "weak";
}

/**
 * Show available wifi networks.
 */
void WiFiManager::handleWifi() const
{
    sendStandardHeaders();
    String html(wifi_template);
    html.replace("${hero.img}", hero_img);
    html.replace("${img.lock}", lock_img);

    String htmlNetworks;
    LOG_DEBUG(F("Network scan start"));
    int n = WiFi.scanNetworks();
    LOG_DEBUG(F("Network scan complete"));

    if (n > 0) {
        for (int i = 0; i < n; i++) {
            if ( strlen(WiFi.SSID(i)) > 0 )     // ignore hidden SSIDs
            {
                String htmlNetwork(wifi_network_template);
                htmlNetwork.replace("${name}", WiFi.SSID(i));

                String signal;
                getSignalStrength(signal, WiFi.RSSI(i));
                htmlNetwork.replace("${signal}", signal);

                htmlNetwork.replace("${network.security}", WiFi.encryptionType(i) == ENC_TYPE_NONE ? "unlock" : "lock");
                htmlNetworks += htmlNetwork;
            }
        }
    }
    else {
        htmlNetworks = F("<tr><td>No networks found</td></tr>");
    }

    WiFi.scanDelete();                      // clean-up

    html.replace("${networks}", htmlNetworks);
    server.send(200, "text/html", html);
    server.client().stop();                 // Stop is needed because we sent no content length
}

/**
 * Process the WiFi save form, save credentials and redirect to WiFi config page again. 
 */
void WiFiManager::handleWifiSave()
{
    LOG_DEBUG(F("Handle WiFi save"));

    server.arg("n").toCharArray(_wlanSSID, SSID_OR_PWD_MAXLEN);
    server.arg("p").toCharArray(_wlanPassword, SSID_OR_PWD_MAXLEN);
    server.sendHeader("Location", "wifi", true);
    sendStandardHeaders();
    server.send(302, "text/plain", "");     // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop();                 // Stop is needed because we sent no content length
    
    _currentState = State_t::SAVE_CREDENTIALS;      // force next step in state machine
}

void WiFiManager::handleNotFound() const
{
    // If captive portal then redirect instead of displaying the error page
    if (redirectToPortal())
    {
        return;
    }

    String message = F("File Not Found\n\n");
    message += F("URI: ");
    message += server.uri();
    message += F("\nMethod: ");
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += F("\nArguments: ");
    message += server.args();
    message += F("\n");

    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += String(F(" ")) + server.argName(i) + F(": ") + server.arg(i) + F("\n");
    }
    sendStandardHeaders();
    server.send(404, "text/plain", message);
//FIXME: button to get back home?
}

bool WiFiManager::loadCredentials()
{
    // load credentials from EEPROM (emulated)
    wifi_credentials_t temp;
    EEPROM.get(0, temp);

    reset_crc32();
    add_crc32((const uint8_t*)temp._ssid, strlen(temp._ssid));
    add_crc32((const uint8_t*)temp._pwd, strlen(temp._pwd));
    uint32_t tempCRC = calc_crc32();

    LOGF_DEBUG("Retrieved credentials for SSID: %s and CRC %d", temp._ssid, temp._crc);

    // compare CRC
    if (tempCRC != temp._crc)
    {
        return false;
    }
    else
    {
        // copy temp to this
        strncpy(_wlanSSID, temp._ssid, sizeof(temp._ssid));
        strncpy(_wlanPassword, temp._pwd, sizeof(temp._pwd));
        return true;
    }
}

void WiFiManager::saveCredentials()
{
    // save to EEPROM (emulated)
    wifi_credentials_t temp;
    strncpy(temp._ssid, _wlanSSID, sizeof(_wlanSSID));
    strncpy(temp._pwd, _wlanPassword, sizeof(_wlanPassword));

    reset_crc32();
    add_crc32((const uint8_t*)_wlanSSID, strlen(_wlanSSID));
    add_crc32((const uint8_t*)_wlanPassword, strlen(_wlanPassword));
    temp._crc = calc_crc32();

    EEPROM.put(0, temp);
    EEPROM.commit();

    LOGF_DEBUG("Stored credentials for SSID: %s and CRC: %d", temp._ssid, temp._crc);
}

void WiFiManager::reset_crc32() {
  crc = 0xFFFFFFFF;
}

void WiFiManager::add_crc32(const uint8_t *data, size_t length) {
  for (size_t i = 0; i < length; i++) {
    uint8_t byte = data[i];
    crc = (crc >> 8) ^ crc32_table[(crc ^ byte) & 0xFF];
  }
}

uint32_t WiFiManager::calc_crc32() {
  return ~crc;
}