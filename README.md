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

1. Clone the repository.
2. Open the project in PlatformIO.
3. Build and upload the project to your microcontroller.
4. Open the terminal and check the IP address of the ESP32. When it connects, it will home the servos.
5. Enter the IP address of the ESP32 and open the web app in your browser.

## Config

You can configure the project by changing the `/include/config.h` file.
Distance between servos and distance of linkages can be configured.

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
