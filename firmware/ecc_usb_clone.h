#pragma once
#include <Arduino.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <usbhub.h>
#include "EvilCrowLib.h"

#ifndef ECC_CLONE_PROFILE_PATH
#define ECC_CLONE_PROFILE_PATH "/usb_clone.txt"
#endif
#define ECC_CLONE_DIR          "/clone"
#define ECC_CLONE_MAX_IFACES   8
#define ECC_CLONE_MAX_RD       1024

struct EccCloneIface {
  uint8_t  bInterfaceNumber;
  uint8_t  bInterfaceClass;
  uint8_t  bInterfaceSubClass;
  uint8_t  bInterfaceProtocol;
  uint16_t hidReportLen;
};

struct EccCloneInfo {
  bool     valid = false;
  uint8_t  addr  = 0;
  uint16_t idVendor = 0, idProduct = 0, bcdDevice = 0, bcdUSB = 0;
  uint8_t  bDeviceClass = 0, bDeviceSubClass = 0, bDeviceProtocol = 0;
  uint8_t  bNumConfigurations = 0, bNumInterfaces = 0;
  char     manufacturer[64] = {0};
  char     product[64]      = {0};
  char     serial[64]       = {0};
  uint8_t  iManufacturer = 0, iProduct = 0, iSerial = 0;
  uint8_t  nIfaces = 0;
  EccCloneIface ifaces[ECC_CLONE_MAX_IFACES];
};

extern EccCloneInfo g_cloneInfo;

static void eccDecodeUsbString(const uint8_t* buf, uint8_t len, char* out, size_t outsz) {
  size_t o = 0;

  for (uint8_t i = 2; i + 1 < len && o + 1 < outsz; i += 2) {
    uint16_t cp = buf[i] | (buf[i + 1] << 8);
    if (cp == 0) break;
    if (cp < 0x80) out[o++] = (char)cp;
    else if (o + 2 < outsz) {
      out[o++] = (char)(0xC0 | (cp >> 6));
      out[o++] = (char)(0x80 | (cp & 0x3F));
    }
  }
  out[o] = '\0';
}

static void eccReadUsbString(USBHost* usb, uint8_t addr, uint8_t index,
                             uint16_t langid, char* out, size_t outsz) {
  out[0] = '\0';
  if (!index) return;
  uint8_t buf[128];
  uint8_t rc = usb->getStrDescr(addr, 0, sizeof(buf), index, langid, buf);
  if (rc == 0 && buf[0] >= 2) eccDecodeUsbString(buf, buf[0], out, outsz);
}

static bool eccUsbCloneScan(USBHost* usb, uint8_t addr) {
  EccCloneInfo info;
  if (addr == 0) return false;
  info.addr = addr;

  uint8_t dd[18];
  if (usb->getDevDescr(addr, 0, sizeof(dd), dd) != 0) return false;
  info.bcdUSB             = dd[2]  | (dd[3]  << 8);
  info.bDeviceClass       = dd[4];
  info.bDeviceSubClass    = dd[5];
  info.bDeviceProtocol    = dd[6];
  info.idVendor           = dd[8]  | (dd[9]  << 8);
  info.idProduct          = dd[10] | (dd[11] << 8);
  info.bcdDevice          = dd[12] | (dd[13] << 8);
  uint8_t iMan = dd[14], iProd = dd[15], iSer = dd[16];
  info.iManufacturer = iMan; info.iProduct = iProd; info.iSerial = iSer;
  info.bNumConfigurations = dd[17];
  uint16_t langid = 0x0409;
  { uint8_t lb[4]; if (usb->getStrDescr(addr, 0, 4, 0, 0, lb) == 0 && lb[0] >= 4)
      langid = lb[2] | (lb[3] << 8); }
  eccReadUsbString(usb, addr, iMan,  langid, info.manufacturer, sizeof(info.manufacturer));
  eccReadUsbString(usb, addr, iProd, langid, info.product,      sizeof(info.product));
  eccReadUsbString(usb, addr, iSer,  langid, info.serial,       sizeof(info.serial));

  uint8_t cfg9[9];
  if (usb->getConfDescr(addr, 0, 9, 0, cfg9) != 0) return false;
  uint16_t wTotal = cfg9[2] | (cfg9[3] << 8);
  info.bNumInterfaces = cfg9[4];
  if (wTotal > 512) wTotal = 512;
  static uint8_t cfg[512];
  if (usb->getConfDescr(addr, 0, wTotal, 0, cfg) != 0) return false;

  if (!SD.exists(ECC_CLONE_DIR)) SD.mkdir(ECC_CLONE_DIR);
  { File f = SD.open(ECC_CLONE_DIR "/config.bin", FILE_WRITE); if (f) { f.write(cfg, wTotal); f.close(); } }

  int8_t  curIface = -1;
  uint16_t i = 0;
  while (i + 1 < wTotal) {
    uint8_t bLen = cfg[i], bType = cfg[i + 1];
    if (bLen == 0) break;
    if (bType == 0x04 && info.nIfaces < ECC_CLONE_MAX_IFACES) {
      EccCloneIface& it = info.ifaces[info.nIfaces];
      it.bInterfaceNumber   = cfg[i + 2];
      it.bInterfaceClass    = cfg[i + 5];
      it.bInterfaceSubClass = cfg[i + 6];
      it.bInterfaceProtocol = cfg[i + 7];
      it.hidReportLen       = 0;
      curIface = info.nIfaces;
      info.nIfaces++;
    } else if (bType == 0x21 && curIface >= 0) {
      if (bLen >= 9 && cfg[i + 6] == 0x22)
        info.ifaces[curIface].hidReportLen = cfg[i + 7] | (cfg[i + 8] << 8);
    }
    i += bLen;
  }

  for (uint8_t n = 0; n < info.nIfaces; n++) {
    uint16_t rlen = info.ifaces[n].hidReportLen;
    if (!rlen) continue;
    if (rlen > ECC_CLONE_MAX_RD) rlen = ECC_CLONE_MAX_RD;
    static uint8_t rd[ECC_CLONE_MAX_RD];
    uint8_t rc = usb->ctrlReq(addr, 0, 0x81, USB_REQUEST_GET_DESCRIPTOR,
                              0x00, 0x22, info.ifaces[n].bInterfaceNumber,
                              rlen, rlen, rd, NULL);
    if (rc == 0) {
      char path[32];
      snprintf(path, sizeof(path), ECC_CLONE_DIR "/rd%u.bin", info.ifaces[n].bInterfaceNumber);
      File f = SD.open(path, FILE_WRITE);
      if (f) { f.write(rd, rlen); f.close(); }
    }
  }

  info.valid = true;
  g_cloneInfo = info;
  return true;
}

static String eccUsbCloneInfoJson() {
  JsonDocument doc;
  doc["valid"] = g_cloneInfo.valid;
  if (!g_cloneInfo.valid) { String s; serializeJson(doc, s); return s; }
  char hx[7];
  snprintf(hx, sizeof(hx), "%04X", g_cloneInfo.idVendor);  doc["vid"] = hx;
  snprintf(hx, sizeof(hx), "%04X", g_cloneInfo.idProduct); doc["pid"] = hx;
  snprintf(hx, sizeof(hx), "%04X", g_cloneInfo.bcdDevice); doc["bcdDevice"] = hx;
  snprintf(hx, sizeof(hx), "%04X", g_cloneInfo.bcdUSB);    doc["bcdUSB"] = hx;
  doc["deviceClass"]    = g_cloneInfo.bDeviceClass;
  doc["numConfigs"]     = g_cloneInfo.bNumConfigurations;
  doc["numInterfaces"]  = g_cloneInfo.bNumInterfaces;
  doc["manufacturer"]   = g_cloneInfo.manufacturer;
  doc["product"]        = g_cloneInfo.product;
  doc["serial"]         = g_cloneInfo.serial;
  doc["iManufacturer"]  = g_cloneInfo.iManufacturer;
  doc["iProduct"]       = g_cloneInfo.iProduct;
  doc["iSerial"]        = g_cloneInfo.iSerial;
  JsonArray ifs = doc["interfaces"].to<JsonArray>();
  for (uint8_t n = 0; n < g_cloneInfo.nIfaces; n++) {
    JsonObject o = ifs.add<JsonObject>();
    o["number"]      = g_cloneInfo.ifaces[n].bInterfaceNumber;
    o["class"]       = g_cloneInfo.ifaces[n].bInterfaceClass;
    o["subclass"]    = g_cloneInfo.ifaces[n].bInterfaceSubClass;
    o["protocol"]    = g_cloneInfo.ifaces[n].bInterfaceProtocol;
    o["hidReportLen"]= g_cloneInfo.ifaces[n].hidReportLen;
  }
  String s; serializeJson(doc, s); return s;
}

static bool eccUsbCloneSaveProfile() {
  if (!g_cloneInfo.valid) return false;
  File f = SD.open(ECC_CLONE_PROFILE_PATH, FILE_WRITE);
  if (!f) return false;
  f.printf("%04X\n%04X\n%04X\n", g_cloneInfo.idVendor, g_cloneInfo.idProduct, g_cloneInfo.bcdDevice);
  f.printf("%s\n%s\n%s\n", g_cloneInfo.manufacturer, g_cloneInfo.product, g_cloneInfo.serial);
  f.close();
  return true;
}

static bool eccUsbCloneProfileExists() { return SD.exists(ECC_CLONE_PROFILE_PATH); }

static void eccUsbCloneClearProfile() {
  if (SD.exists(ECC_CLONE_PROFILE_PATH)) SD.remove(ECC_CLONE_PROFILE_PATH);
}

static String s_cloneMan, s_cloneProd, s_cloneSer;
static bool eccUsbCloneApply(EclUsbConfig& cfg) {
  if (!SD.exists(ECC_CLONE_PROFILE_PATH)) return false;
  File f = SD.open(ECC_CLONE_PROFILE_PATH, FILE_READ);
  if (!f) return false;
  String vid = f.readStringUntil('\n'); vid.trim();
  String pid = f.readStringUntil('\n'); pid.trim();
  String bcd = f.readStringUntil('\n'); bcd.trim();
  s_cloneMan = f.readStringUntil('\n'); s_cloneMan.trim();
  s_cloneProd= f.readStringUntil('\n'); s_cloneProd.trim();
  s_cloneSer = f.readStringUntil('\n'); s_cloneSer.trim();
  f.close();
  if (vid.length()) cfg.vid       = (uint16_t) strtol(vid.c_str(), nullptr, 16);
  if (pid.length()) cfg.pid       = (uint16_t) strtol(pid.c_str(), nullptr, 16);
  if (bcd.length()) cfg.bcdDevice = (uint16_t) strtol(bcd.c_str(), nullptr, 16);
  cfg.manufacturer = s_cloneMan.c_str();
  cfg.product      = s_cloneProd.c_str();
  cfg.serial       = s_cloneSer.c_str();
  return true;
}