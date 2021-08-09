
// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device

// Created 29 March 2006

/* There are both 7- and 8-bit versions of I2C addresses. 
7 bits identify the device, and the eighth bit determines if it's being written to or read from. 
The Wire library uses 7 bit addresses throughout. If you have a datasheet or sample code that uses 8 bit address, you'll want to drop the low bit 
(i.e. shift the value one bit to the right), yielding an address between 0 and 127. 
However the addresses from 0 to 7 are not used because are reserved so the first address that can be used is 8. 
Please note that a pull-up resistor is needed when connecting SDA/SCL pins. Please refer to the examples for more informations.*/


#include <Wire.h>

#define TEMP_ADD (0xb8 >> 1) //address of temp sensor 



void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output

}

void loop()
{
  Wire.beginTransmission(TEMP_ADD); // transmit to device address
  Wire.write(0x03);        // command
  Wire.write(0x00);        // start address 
  Wire.write(0x04);              // length requested
  Wire.endTransmission();    // stop transmitting

 delay(2000);
  
  Wire.requestFrom(TEMP_ADD, 8);    // request 6 bytes from temp sensor (slave device 0xB8)
  Wire.requestFrom(TEMP_ADD, 8);    // Manufacturer recommends making request twice for accurate data
  
  int i=0; 
  uint8_t high_hum = 0; 
  uint8_t low_hum = 0; 
  uint8_t high_temp = 0;
  uint8_t low_temp = 0; 
  uint8_t crc_L = 0;
  uint8_t crc_H = 0; 
  uint8_t func_code = 0;
  uint8_t num_byt = 0; 
  
  while (Wire.available()) { // slave may send less than requested
   char c = Wire.read(); // receive a byte as character
   if (i==0)
   {
    func_code = c;
   }
   else if(i==1){
    num_byt = c; 
   }
   else if (i==2){
    high_hum = c;  
   } 
   else if (i==3)
   {
    low_hum = c;  
   }
   else if (i==4)
   {
    high_temp = c;
   }
   else if (i==5)
   {
    low_temp =c;  
   }
   else if (i==6)
   {
    crc_L = c; 
   }
   else if (i==7)
   {
    crc_H = c; 
   }
   else
   {
    //MISRA :)
   }
   
   if (i < 8)
   {
     i++;
   }
    
  }

  uint16_t crc_temp = (( crc_H << 8)  | crc_L );
  bool negativeTemp = (0x80 & high_temp);
  uint16_t tempConvBase = (((0x7F & high_temp) << 8) | low_temp) / 10;
  uint16_t tempConvDec = (((0x7F & high_temp) << 8) | low_temp) % 10;

  uint16_t hum = ((high_hum << 8) | low_hum) / 10;

  char tempSign = '+';
  if (negativeTemp)
  {
    tempSign = '-';
  }


  uint16_t crc_temp_calc = 0xFFFF; 
  uint8_t temp_mesg[6];
  temp_mesg[0] = func_code;
  temp_mesg[1] = num_byt;
  temp_mesg[2] = high_hum;
  temp_mesg[3] = low_hum;
  temp_mesg[4] = high_temp;
  temp_mesg[5] = low_temp; 
//  temp_mesg [func_code, num_byt, high_hum, low_hum, high_temp, low_temp];
  //temp_mesg [0x03, 0x04, 0x02, 0x19, 0x01, 0x04]; //test values 
  for (int j=0; j<6; j++)
  {
    crc_temp_calc = crc_temp_calc^temp_mesg[j];
  
  for (int k=0; k<8; k++){
    if (crc_temp_calc & 0x01)
    {
      crc_temp_calc = crc_temp_calc >> 1;
      crc_temp_calc = crc_temp_calc ^ 0xA001;
    }else{
      crc_temp_calc = crc_temp_calc >> 1;
    }
  }
    
  }
    
  if (crc_temp_calc == crc_temp){
  Serial.print("Temp. = ");
  Serial.print(tempSign);
  Serial.print(tempConvBase);
  Serial.print(".");
  Serial.println(tempConvDec);
  Serial.print("Rel. Humidity = ");
  Serial.print(hum); 
  Serial.println("%");
  }
}
