# libsense  
An *adapted* library for Raspberry Pi sense hat. The full libsense library can be found at github.com/gsilber/libsense_p 
This is a c library for the Raspberry Pi Sense Hat (Astro Pi).  It is designed to cover basic communication with the various interfaces on the Sense Hat.  
  
Installation:  
	make: builds library libsense.a  
	make clean: removes all  
	make all: builds everything  
	make install: installs in /usr/local/lib and /usr/local/include/sense  
	make uninstall: removes from /usr/local  
  
Usage:  
The library consists of code for each sensor type.  General use is to connect to the device:  
	getFBDevice() : connect to the framebuffer for the led array  
	getJoystickDevice() : connect to the joystick event device  
	geti2cDevice() : connect to the i2c bus for sensors  
  
Each of these should be freed with a corresponding call to the correct free function:  
	freeFBDevice(device)  
	freeJoystickDevice(device)  
	freei2cDevice(device)  
  
For i2c devices, once connected, configure the specific device.  
	configureHumiditySensor(device); //Humidity and Temp  
	configurePressureSensor(device); //Pressure	  
	configureMag(device); //Magnetometer  
	configureAccelGyro(device); //Gyro and accelerometer  
  
Then call functions to manipulate that device as defined in the appropriate header file for that device.  
	example: to make the led array all red:  
  
	pi_framebuffer_t* buf=getFBDevice();  
	clearBitmap(buf->bitmap,getColor(255,0,0));  
	freeFBDevice(buf);  
  

  
