# Libsense (mini version) 
An *adapted* library for Raspberry Pi sense hat. The full libsense library can be found [here](github.com/gsilber/libsense_p). This version contains only filed needed for joystick and framebuffer usage.

 
This is a c library for the Raspberry Pi Sense Hat (Astro Pi).  It is designed to cover basic communication with the various interfaces on the Sense Hat.  

---
### Usage:  
Connect to the device:  
	getFBDevice() : connect to the framebuffer for the led array  
	getJoystickDevice() : connect to the joystick event device
  
Each of these should be freed with a corresponding call to the correct free function:  
	freeFBDevice(device)  
	freeJoystickDevice(device)  

Then call functions to manipulate that device as defined in the appropriate header file for that device.  
	example: to make the led array all red:  
  
	pi_framebuffer_t* buf=getFBDevice();  
	clearBitmap(buf->bitmap,getColor(255,0,0));  
	freeFBDevice(buf);  
  

  
