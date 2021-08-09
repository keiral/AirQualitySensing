print "CRC python file" 

#input_data = [0x03, 0x04, 0x01, 0xF4, 0x00, 0xFA]
input_data2 = [0x03, 0x04, 0x02, 0x19, 0x01, 0x04]
crc = 0xFFFF  

for  data in input_data2: 
	#print data
	crc = crc^data
	for i in range(8):
		if crc & 0x01:
			crc = crc >> 1 
			crc = crc ^ 0xA001 
		else:
			crc = crc >> 1 
print hex(crc)


			