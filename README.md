# Async-Web-Server-M5StickC-BME280-Environment-Sesnsing

 - M5StickC is a mini M5Stack, powered by ESP32. It is a portable, easy-to-use, open source, IoT development board.
 
 - BME280 is an environmental sensor with temperature, barometric pressure and humidity. This sensor is great for all sorts of indoor environmental sensing and can even be used in both I2C and SPI.
 
 - In the program the M5StickC board and BME280 sensor are connected over the I2C communication interface.

 - An asynchronous server is created in M5StickC using XMLHttpRequest which reads the BME280 sensor values.
 - Clients can connect to server in either:
      - Wi-Fi Station Mode, or
      - Soft Access Point Mode
  
 - The webpage can be accessed through client’s browser using the IP address of M5StickC.
 - The values are automatically updated every 5 seconds. 

![Screenshot](screenshots/browser_view.png)





