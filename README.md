# WiFi_Camera_Firmware
### WiFi camera device application firmware project<br>
MCU: STM32F437<br>
IDE: IAR for ARM v7.8<br>
Script env: Python 3.6<br>
Script path: WiFi_Camera_Firmware/script<br>

------
## Communication Protocol	

###  Message Format:
####  Data Structure:
StartCode(5) + Command(1) + Index(2) + Length(2) + Payload(0~1000) + Checksum(1) + EndCode(5)<br>

####  Checksum:
Checksum is the sum start from command to the last payload data<br>

####  Data Endian:
16bit and 32bit data all low byte at first<br>

####  Data Size:
Message Command Size without Payload: 16 bytes<br>
Max Transmit Payload: 1000 bytes<br>
Max Receive Payload: 1000 bytes<br>

#### Message Command:

```C
/* Recognize code */
#define MSG_RECOGNIZE_CODE_LEN  5       /* 5 */
#define MSG_START_CODE          0x7B    /* 123 */
#define MSG_END_CODE            0xA8    /* -88 */

/* App get command code */
#define MSG_GET_BASE            0x10
#define MSG_GET_MAC             (MSG_GET_BASE + 1)
#define MSG_GET_IMAGE           (MSG_GET_BASE + 2)
#define MSG_GET_STATE           (MSG_GET_BASE + 3)
#define MSG_GET_VERSION         (MSG_GET_BASE + 4)

/* App set command code */
#define MSG_SET_BASE            0x20
#define MSG_SET_ACCOUNT         (MSG_SET_BASE + 1)
#define MSG_SET_WIFI            (MSG_SET_BASE + 2)
#define MSG_SET_MOTOR           (MSG_SET_BASE + 3)
#define MSG_SET_TIME            (MSG_SET_BASE + 4)
#define MSG_SET_SCH             (MSG_SET_BASE + 5)

/* Device push command code */
#define MSG_PUSH_BASE           0x30
#define MSG_PUSH_IMAGE          (MSG_PUSH_BASE + 1)
#define MSG_PUSH_ACCOUNT        (MSG_PUSH_BASE + 2)
#define MSG_PUSH_ALARM          (MSG_PUSH_BASE + 3)

/* App ota update code */
#define MSG_OTA_BASE            0x40
#define MSG_OTA_REQUEST         (MSG_OTA_BASE + 1)
#define MSG_OTA_BIN             (MSG_OTA_BASE + 2)
#define MSG_OTA_VERIFY          (MSG_OTA_BASE + 3)

/* Factory new magic code */
#define MSG_FACTORY_NEW         0xA5

/* Feedback state code */
#define MSG_FB_BASE             0xF0
#define MSG_FB_OK               (MSG_FB_BASE + 0)
#define MSG_FB_UPDATED          (MSG_FB_BASE + 1)   /* For ota request */
#define MSG_FB_ERROR            (MSG_FB_BASE + 2)
```

### Detail Protocol:
#### Get Version: 
App Tx: command, no payload<br>
```c
7B 7B 7B 7B 7B 14 00 00 00 00 14 A8 A8 A8 A8 A8
```
App Rx: feedback ok + 2 bytes payload of version value: V2.05 = 205 = 0xCD<br>
```c
7B 7B 7B 7B 7B F0 00 00 02 00 CD 00 BF A8 A8 A8 A8 A8
```

#### Get MAC: 
App Tx: command, no payload<br>
```c
7B 7B 7B 7B 7B 11 00 00 00 00 11 A8 A8 A8 A8 A8 
```
App Rx: feedback ok + 18 bytes payload of mac string: 68:C6:3A:A0:FC:5F<br>
```c
7B 7B 7B 7B 7B F0 00 00 12 00 36 38 3A 43 36 3A 33 41 3A 41 30 3A 46 43 3A 35 46 00 F4 A8 A8 A8 A8 A8
``` 

#### Get Image: 
App Tx: command, no payload<br>
```c
7B 7B 7B 7B 7B 12 00 00 00 00 12 A8 A8 A8 A8 A8  
```
App Rx: feedback ok and wait for push image<br>
```c
7B 7B 7B 7B 7B F0 00 00 00 00 F0 A8 A8 A8 A8 A8 
``` 

#### Factory New: 
App Tx: command, no payload<br>
```c
7B 7B 7B 7B 7B A5 00 00 00 00 A5 A8 A8 A8 A8 A8  
```
App Rx: feedback ok and disconnect<br>
```c
7B 7B 7B 7B 7B F0 00 00 00 00 F0 A8 A8 A8 A8 A8  
``` 

#### Set WiFi AP: 
App Tx: command, <br>
length=2+ssid length(<=32)+passwd length(<=32)<br>
payload: ssid length(1 byte), passwd length(1byte), ssid string, passwd string<br>
```c
7B 7B 7B 7B 7B 22 00 00 0D 00 03 08 41 42 43 31 32 33 34 35 36 37 38 A4 A8 A8 A8 A8 A8  
```
App Rx: feedback ok<br>
```c
7B 7B 7B 7B 7B F0 00 00 00 00 F0 A8 A8 A8 A8 A8  
``` 

#### Set Web Account: 
App Tx: command, <br>
length=4+server length(<=64)+port length(=2) + web id length(<=32) + passwd length(<=32)<br>
payload: server length(1 byte), port length(1byte), web id length(1byte), passwd length(1byte), server string, port value(2byte), web id string web passwd string<br>
```c
7B 7B 7B 7B 7B 21 00 00 23 00 0B 02 07 0B 31 39 32 2E 31 36 38 2E 31 2E 32 90 1F 69 64 5F 30 30 30 31 41 42 43 44 45 46 47 48 49 4A 4B 29 A8 A8 A8 A8 A8 
```
App Rx: feedback ok<br>
```c
7B 7B 7B 7B 7B F0 00 00 00 00 F0 A8 A8 A8 A8 A8  
```
 

#### Set RTC Clock: 
App Tx: command, <br>
length=7<br>
payload: year value 2bytes, month value, day value, hour value, minute value, second value<br>
```c
7B 7B 7B 7B 7B 24 00 00 07 00 E2 07 03 07 17 14 0D 56 A8 A8 A8 A8 A8 
```
App Rx: feedback ok<br>
```c
7B 7B 7B 7B 7B F0 00 00 00 00 F0 A8 A8 A8 A8 A8  
``` 

#### Set Feed Schedule: 
App Tx: command, <br>
length=7 * plan number<br>
payload: Time hour value(1byte), time minute value(1byte) , motor1 feed level(1byte) , motor2feed level(1byte) , motor3 feed level(1byte) , motor4 feed level(1byte) , motor5 feed level(1byte), add has more plan, add schedule until finish<br>
```c
7B 7B 7B 7B 7B 25 00 00 0E 00 0B 1E 01 02 03 04 05 0F 2D 07 05 03 00 00 B6 A8 A8 A8 A8 A8  
```
App Rx: feedback ok<br>
```c
7B 7B 7B 7B 7B F0 00 00 00 00 F0 A8 A8 A8 A8 A8  
``` 

#### Set Motor Parameter: 
App Tx: command, <br>
length=25<br>
payload: dir1(1 char ‘R’ or ‘L’), dir2,dir3,dir4,dir5, Freq1(2byte) , Freq2, Freq3, Freq4, Freq5, UnitStep1(2bytes) , UnitStep2, UnitStep3, UnitStep4, UnitStep5<br>
```c
7B 7B 7B 7B 7B 23 00 00 19 00 52 52 4C 4C 4C E8 03 E8 03 E8 03 E8 03 E8 03 C8 00 C8 00 C8 00 C8 00 C8 00 43 A8 A8 A8 A8 A8  
```
App Rx: feedback ok<br>
```c
7B 7B 7B 7B 7B F0 00 00 00 00 F0 A8 A8 A8 A8 A8  
``` 

#### Push Image: 
##### Pack index = 0: device send image information
App Rx: command,<br>
	Index = 0 <br>
length=4 + filename length<br>
payload: image size(4bytes, 32bit), filename string<br>
##### Pack index > 0: device send image data
App Rx: command,<br>
	Index > 0 <br>
length=image packet data size<br>
payload: image data<br>

### Detail Protocol -- OTA:
#### Step1 Request OTA: 
App Tx: command, MSG_OTA_REQUEST<br>
length=2<br>
payload: new version 2bytes(example 206)<br>
```c
7B 7B 7B 7B 7B 41 00 00 02 00 CE 00 11 A8 A8 A8 A8 A8 
```
App Rx: feedback updated; because new version is the same as device, device return updated to reject ota<br>
```c
7B 7B 7B 7B 7B F1 00 00 00 00 F1 A8 A8 A8 A8 A8 
```
App Rx: feedback ok, if device version is lower than new version, return ok to start ota<br>
```c
7B 7B 7B 7B 7B F0 00 00 00 00 F0 A8 A8 A8 A8 A8
``` 

#### Step2 Send OTA Bin Data: 
App Tx: command, MSG_OTA_BIN<br>
	Index = bin packet id<br>
length=bin data size<br>
payload: bin data<br>
App Rx: feedback ok to continue next packet<br>
```c
7B 7B 7B 7B 7B F0 00 00 00 00 F0 A8 A8 A8 A8 A8
```

#### Step3 Verify OTA Firmware: 
App Tx: command, MSG_OTA_VERIFY<br>
length=0<br>
payload: none<br>
```c
7B 7B 7B 7B 7B 43 00 00 00 00 43 A8 A8 A8 A8 A8
```
App Rx: feedback ok if verify ok and disconnect to reset mcu<br>
```c
7B 7B 7B 7B 7B F0 00 00 00 00 F0 A8 A8 A8 A8 A8
```
