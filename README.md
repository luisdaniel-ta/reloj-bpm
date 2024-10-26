## Clock BPM

Clock BPM is a monolithic project with the primary objective of developing a smartwatch through all its phases: 3D design, microcontroller (MCU) programming, 3D printing, and final product assembly. This device is capable of providing critical information to the user, such as heart rate, blood oxygen levels, and local weather updates. The device integrates the MAX30105 sensor for health measurements and the ESP32 microcontroller, which displays this data via an OLED screen. Additionally, the project includes the development of a simple mobile application that allows real-time data monitoring, with the option to view other relevant information.

Weather information is retrieved through the integration of a REST API provided by OpenWeatherMap, enabling real-time updates of the user’s city weather conditions.

### Key Features of the Device:

- Monitoring of heart rate and blood oxygen levels.
- Real-time weather information via a REST API.
- OLED screen for data display.
- Mobile application for monitoring and remote control.

## Hardware Components Used

### LILYGO Display (ESP32 + SSD1306 OLED)
For this project, the LILYGO_Display development board is used, combining the ESP32 microcontroller with an OLED screen based on the SSD1306 controller. This board offers several terminals that facilitate the connection of different peripherals, such as the MAX30105 sensor and other external modules. It also includes a dedicated terminal for connecting a battery, enabling the device’s portability.

<img src="/docs/LILYGO_DISPLAY.png" width="50%">

### Features of LILYGO_Display:

- WiFi and Bluetooth connectivity, allowing communication with external devices and cloud services.
- Compatibility with REST API to retrieve weather data.
- Energy-efficient OLED display, ideal for portable devices.
- Multiple GPIO terminals for sensor and component integration.

### MAX30105 Sensor
The MAX30105 sensor is the key component responsible for health measurements in the device. It uses optical technology to measure the user’s heart rate and blood oxygen levels. Communication between the sensor and the ESP32 microcontroller is facilitated through an I2C bus, making its integration seamless.

<img src="/docs/M30172.png" width="50%">


### Features of the MAX30105 Sensor:

- I2C communication for data reading.
- Built-in filters to enhance measurement accuracy.
- Use of the Millis() function for precise synchronization of readings.

## 3D Design
The 3D design of the watch was created using SolidEdge software, with the final files exported in STL format for 3D printing. The design includes all necessary parts for the assembly of the device, such as the main casing, covers, and supports for the OLED screen and MAX30105 sensor.

<embed src="/pdf/box.pdf" />

### 3D STL

<object data="/stl/box.stl" width="700px" height="700px">
   <embed src="/stl/box.stl" />
</object>

## Mobile Application

A simple mobile application has been developed, which connects to the smartwatch via Bluetooth, allowing the user to:

- Monitor heart rate and oxygen levels in real-time.
- Customize certain functions of the watch.

<img src="/app/03.png" width="50%">
