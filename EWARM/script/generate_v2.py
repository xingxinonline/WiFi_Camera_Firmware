# -*- coding: utf-8 -*-
"""
Description:  insert version(2bytes) crc16(2bytes) and file size(4bytes) at the start of bin file
    1. open bin file in binary format
    2. read to data buffer and close file
    3. search firmware version
    4. calculate crc16
    5. add 2byte firmware version at the start of file(lsb, msb, total 4bytes)
    6. add 2bytes crc16 
    7. add 4bytes filesize
    8. create ota firmware file
    
    IAR Build Actions: python $PROJ_DIR$\script\generate_v2.py $PROJ_DIR$
    
Created on Wed Feb 14 14:36:24 2018

@author: Douglas Xie
@email:  douglas2011@qq.com
"""

import sys
import time

# bin file location
if len(sys.argv) == 1:
    filedir = '../WiFi_Camera/Exe/WiFi_Camera.bin'
    savedir = '../WiFi_Camera/Exe/OTA_Firmware_V'
else:
    working_path = sys.argv[1]
    filedir = working_path + '/WiFi_Camera/Exe/WiFi_Camera.bin'
    savedir = working_path + '/WiFi_Camera/Exe/OTA_Firmware_V'
    
#firmware version pre-str
prestr = 'RunTime Version V'

# CRC16-CCITT Algorithm 
def crc16_ccitt(data, length):  
    seed = 0xFFFF
    poly16 = 0x1021

    wTemp = 0 
    wCRC = seed
    for i in range(length):            
        for j in range(8):        
            wTemp = ((data[i] << j) & 0x80 ) ^ ((wCRC & 0x8000) >> 8);      
            wCRC = wCRC << 1      
    
            if wTemp != 0:
                wCRC = wCRC ^ poly16
                
    wCRC = wCRC & 0xFFFF
    return wCRC  

'''
step:
    1. open bin file in binary format
    2. read to data buffer and close file
    3. search firmware version
    4. calculate crc16
    5. add 2byte firmware version at the start of file(lsb, msb, total 4bytes)
    6. add 2bytes crc16 
    7. add 4bytes filesize
    8. create ota firmware file
'''

print("***************************************************************")
print("* Python Script for Generate OTA Firmware File by Douglas Xie")
print("***************************************************************")
print("read original firmware data")
fw = open(filedir, 'rb')
fw_data = fw.read()
fw.close()
fw_size = len(fw_data)

print("searching firmware version")
start_index = 0
for i in range(fw_size-17):
    try:
        s = bytes.decode(fw_data[i:(i+17)])
        if s == prestr:
            start_index = i
            break
    except:
        index = 0

end_index = start_index + 1
while fw_data[end_index] != str.encode('\r')[0]:
    end_index = end_index + 1
    if end_index >= len(fw_data):
        break

try:
    runtime = bytes.decode(fw_data[start_index:end_index])
    print("  >>>", runtime)
    ver_str = runtime[17:]
    
    fw_ver = 0
    point = ver_str.find('.', 0)
    for i in range(len(ver_str)):
        if i != point:
            fw_ver *= 10
            fw_ver = fw_ver + str.encode(ver_str[i])[0] - 0x30
except:
    print("  >>> firmware version not found")
    fw_ver = 0
    ver_str = "0.00"

print("generating crc16...")
fw_crc16 = crc16_ccitt(fw_data, fw_size)
print("  >>> generating crc16 success")
insert_data = []
insert_data.append(fw_ver & 0xFF)
insert_data.append((fw_ver >> 8) & 0xFF)
insert_data.append(fw_crc16 & 0xFF)
insert_data.append((fw_crc16 >> 8) & 0xFF)
for i in range(4):
    insert_data.append((fw_size >> (i * 8)) & 0xFF)

new_data = bytes(insert_data) + fw_data;
print("generate ota firmware file")
savedir = savedir + ver_str + ".bin"
otaname = 'OTA_Firmware_V'+ ver_str + ".bin"
ota = open(savedir, 'wb')
ota.write(new_data)
ota.flush()
ota.close()
print("create OTA firmware success")
print("  >>> path: ", savedir)
print("  >>> size: ", len(new_data), "bytes")
print("finish")

if len(sys.argv) == 1:
    print("close windows after 5 second")
    for i in range(5):
        print(5 - i)
        time.sleep(1)
    
