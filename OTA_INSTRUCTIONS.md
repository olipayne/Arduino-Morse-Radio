# OTA Update Instructions

## Binary Types

The project releases include two types of binary files:

1. **Full Firmware Binaries** - Used for initial flashing via USB
   - `firmware-vX.X.X-release.bin` - Production release build
   - `firmware-vX.X.X-debug.bin` - Debug build with verbose output

2. **OTA-specific Binaries** - Used for over-the-air updates
   - `firmware-vX.X.X-release-ota.bin` - Production release OTA build
   - `firmware-vX.X.X-debug-ota.bin` - Debug OTA build with verbose output

## Performing OTA Updates

1. Download the appropriate OTA-specific binary (ending with `-ota.bin`) from the [GitHub Releases page](https://github.com/olipayne/Arduino-Morse-Radio/releases)

2. Connect to the device's WiFi access point
   - The SSID will be in the format `Radio_XXXXXX`
   - No password is required

3. Open `http://192.168.4.1/update` in your web browser

4. Click "Choose File" and select the OTA-specific binary file (the one ending with `-ota.bin`)
   - **Important**: Do NOT use the regular binary files (without `-ota` suffix) for OTA updates

5. Click "Update" and wait for the process to complete

6. The device will automatically restart with the updated firmware

## Troubleshooting

If you experience issues with OTA updates:

1. Make sure you're using the correct binary (with `-ota` suffix)
2. Try using the debug version to get more verbose output
3. If updates consistently fail, revert to USB flashing using the full firmware binary

## Initial Device Flashing

For initial setup or to recover from failed OTA updates, use the regular (non-OTA) binary files with USB flashing:

```bash
# Using esptool.py directly
esptool.py --chip esp32s3 --port /dev/ttyUSB0 --baud 921600 write_flash -z 0x0 firmware-vX.X.X-release.bin

# Or with PlatformIO
pio run -t upload
``` 