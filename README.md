# Scara Drawing Robot

![IMG_1262-ezgif com-optimize](https://github.com/user-attachments/assets/8ba335ce-78a2-4f69-bf28-65f6955566e9)

## Overview

This project is for making a Scara Drawing Robot with ESP32 S3 controller and 3 servo motors.

## Model

You can find the 3D model for this project on MakerWorld:
[Tenstar C3 Mini Model](https://makerworld.com/en/models/978425)

## File Structure

- `/web`: Web app files (uses preact)
- `/src`: Source code files
- `/include`: Header files
- `/lib`: Libraries
- `platformio.ini`: PlatformIO configuration file

## Getting Started

1. Download the latest firmware release from the [GitHub releases page](releases).
2. Flash the firmware to your ESP32 using [ESPHome Flasher](https://web.esphome.io/). **Note:** You need to click "Install" on the ESPHome Flasher page to use it.
3. Once flashed, the ESP32 will connect to your network and home the servos automatically.
4. Access the web interface by entering the IP address of the ESP32 in your browser, or use the default domain `drawer.local` (note: not all devices support mDNS, so you may need to use the IP address instead).

## Building from Source

To build a flashable firmware file from source:

1. Clone the repository and open the project in PlatformIO.
2. Run the following command to create a merged firmware binary:
   ```sh
   pio run -t mergebin
   ```
3. This will create a flashable binary at `.pio/build/lolin_c3_mini/firmware-merged.bin` that can be flashed using the [ESPHome Flasher](https://web.esphome.io/). You can also just flash it via Platform.io.

## Config

You can configure the project by changing the `/include/config.h` file.
Distance between servos and distance of linkages can be configured.

### Custom Domain

The device is accessible by default at `drawer.local` via mDNS. However, not all devices support mDNS, so you may need to use the IP address instead. You can configure a custom domain in the configuration file if needed.

## Known Issues

It's possible to select position that is not reachable by the robot. This will cause the robot to restart.

## Development Server

To run the development server for the web app, run:

```sh
cd web
npm run dev
```

## Contact

For any questions or support, please contact the project maintainer.

![2025-01-19_6668bdaa25b37](https://github.com/user-attachments/assets/8b4d329f-8c22-412f-94e3-f2f5626df43d)
