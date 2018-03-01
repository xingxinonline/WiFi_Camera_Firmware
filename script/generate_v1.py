# -*- coding: utf-8 -*-
"""
Description:  insert crc16(2bytes) and file size(4bytes) at the start of bin file
    1. open bin file in binary format
    2. read to data buffer
    3. calculate crc16
    4. f.seek(0,0) point to file start
    5. write 4bytes filesize at the start of file(lsb, msb, total 4bytes)
    6. write 2bytes crc16 
    
Created on Wed Feb 14 14:36:24 2018

@author: Douglas Xie
@email:  douglas2011@qq.com
"""

# bin file location
filedir = '../EWARM/WiFi_Camera/Exe/WiFi_Camera.bin'
savedir = '../EWARM/WiFi_Camera/Exe/OTA_Firmware.bin'

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
    2. read to data buffer
    3. calculate crc16
    4. f.seek(0,0) point to file start
    5. write 4bytes filesize at the start of file(lsb, msb, total 4bytes)
    6. write 2bytes crc16 
'''
print("read original firmware data")
fw = open(filedir, 'rb')
fw_data = fw.read()
fw.close()
fw_size = len(fw_data)
print("generating crc16...")
fw_crc16 = crc16_ccitt(fw_data, fw_size)
print("generating crc16 success")
insert_data = []
insert_data.append(fw_crc16 & 0xFF)
insert_data.append((fw_crc16 >> 8) & 0xFF)
for i in range(4):
    insert_data.append((fw_size >> (i * 8)) & 0xFF)

new_data = bytes(insert_data) + fw_data;
print("generate ota firmware file")
ota = open(savedir, 'wb')
ota.write(new_data)
ota.flush()
ota.close()
print("finish")