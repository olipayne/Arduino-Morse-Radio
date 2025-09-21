# OTA Update Feature Guide

## Overview

The Arduino Morse Radio now includes an Over-The-Air (OTA) update feature that allows you to update the firmware wirelessly without needing to connect the device to a computer. This feature is triggered by a special boot sequence and provides visual feedback through the wave band LEDs.

## How to Trigger an OTA Update

### Boot Sequence
1. **Power on** the device (or wake it from sleep)
2. **Within 5 seconds** of boot-up, press the **WiFi button 3 times**
3. The device will automatically start the OTA update process

### Visual Feedback During Update

The update process provides clear visual feedback through the wave band LEDs:

1. **Short Wave LED (Blue)** - Flashing: Searching for and connecting to WiFi networks
2. **Medium Wave LED (Yellow)** - Flashing: Downloading the latest firmware
3. **Long Wave LED (Red)** - Flashing: Installing the firmware update

### Success and Error Indicators

- **Success**: Device will automatically reboot with the new firmware
- **WiFi Connection Failed**: All three LEDs flash together 5 times
- **Update Failed**: Long Wave LED (Red) flashes rapidly 10 times

## WiFi Configuration

The OTA updater will attempt to connect to up to 3 pre-configured WiFi networks. By default, these are set to placeholder values that you need to modify.

### Configuring WiFi Credentials

To set your WiFi credentials, modify the `checkOTABootSequence()` function in `src/PowerManager.cpp`:

```cpp
// Configure WiFi credentials (modify these with your actual networks)
otaManager.addWiFiCredentials("YourWiFiName1", "YourPassword1");
otaManager.addWiFiCredentials("YourWiFiName2", "YourPassword2");
otaManager.addWiFiCredentials("YourWiFiName3", "YourPassword3");
```

Replace the placeholder values with your actual WiFi network names and passwords.

## How It Works

### Version Checking
- The device fetches the latest release information from the GitHub repository
- It compares the current firmware version with the latest available version
- Only proceeds with download if a newer version is available

### Download Process
- Fetches the latest release information from GitHub API
- Parses the release assets to find the correct firmware binary
- Specifically looks for assets containing "release-ota.bin" in the filename
- Downloads the firmware using the `browser_download_url` from the GitHub API response

### Security Considerations
- Uses HTTPS for all communications
- Verifies the integrity of the downloaded firmware
- Only installs if the download is complete and valid

## Troubleshooting

### Update Not Starting
- Ensure you press the WiFi button exactly 3 times within 5 seconds of boot
- Check that the device has power and is properly booting
- Verify the button is working correctly

### WiFi Connection Issues
- Verify your WiFi credentials are correct in the code
- Ensure at least one of the configured networks is available
- Check that the WiFi networks have internet access
- Make sure the networks are 2.4GHz (ESP32 doesn't support 5GHz)

### Download/Install Failures
- Ensure stable internet connection
- Check that GitHub releases are accessible
- Verify there's sufficient free flash memory for the update
- Try again - temporary network issues can cause failures

### LED Indicators Not Working
- Check that the LED pins are properly connected
- Verify the LED configuration in the hardware setup

## Technical Details

### Files Added/Modified
- `src/OTAManager.h` - OTA manager class definition
- `src/OTAManager.cpp` - OTA manager implementation
- `src/PowerManager.h` - Added OTA boot sequence detection
- `src/PowerManager.cpp` - Added button press detection and OTA integration
- `src/main.cpp` - Added OTA check to system update loop

### Dependencies
The OTA feature uses the following libraries (already included in platformio.ini):
- ArduinoJson - For parsing GitHub API responses
- HTTPClient - For downloading firmware
- Update - For performing OTA updates
- WiFi - For network connectivity

### Asset Selection Logic
The OTA system intelligently selects the correct firmware binary:

1. **API Call**: Makes a GET request to `https://api.github.com/repos/olipayne/Arduino-Morse-Radio/releases/latest`
2. **Asset Parsing**: Parses the JSON response to find the `assets` array
3. **File Selection**: Searches for assets with "release-ota.bin" in the filename
4. **URL Extraction**: Uses the `browser_download_url` field for the download

Example asset structure from GitHub API:
```json
{
  "assets": [
    {
      "name": "firmware-v1.11.4-release-ota.bin",
      "browser_download_url": "https://github.com/olipayne/Arduino-Morse-Radio/releases/download/v1.11.4/firmware-v1.11.4-release-ota.bin"
    }
  ]
}
```

### Memory Usage
The OTA feature adds minimal memory overhead during normal operation. During an update, it temporarily uses additional RAM for:
- HTTP client buffers
- JSON parsing
- Firmware download buffering

## Development Notes

### Testing
- Test the feature in a controlled environment first
- Ensure you have a way to recover the device if an update fails
- Verify WiFi credentials before deploying

### Customization
You can customize the OTA behavior by modifying:
- WiFi credentials in `PowerManager.cpp`
- LED flash patterns in `OTAManager.cpp`
- Timeout values in `OTAManager.h`
- GitHub repository URL in `OTAManager.h`

### Future Enhancements
Possible improvements for future versions:
- WiFi credential configuration through web interface
- Multiple firmware variants (debug/release)
- Rollback capability
- Progress indicators beyond LED flashing
- Configuration of update server URL

## Safety Notes

⚠️ **Important Safety Information**

- Never interrupt the update process once it has started
- Ensure stable power supply during updates
- Keep the device close to the WiFi router during updates
- Have a backup plan to reflash the device if needed
- Test updates in a development environment first

The OTA update process will automatically reboot the device upon successful completion. If the update fails, the device will continue running the current firmware.
