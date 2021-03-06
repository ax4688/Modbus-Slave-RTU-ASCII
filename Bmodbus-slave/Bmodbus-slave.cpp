/*	Create: Burak DERELİ
	Contact: www.burakdereli.net
	Version: v0.40
*/

#include "Bmodbus-slave.h"

byte modbus_id; //Slave Modbus ID
byte tp; //TX Enable Pin
String bmod = "";  
byte readData[50];
byte sendData[100];
String readAscii;
String asciiData[50];
int readL=0;
int address;

void Bmodbus::mode (String mod , int m_id , int baud, int prt, byte t_pin, int adr){
	
	// Modbus Select Mod
	if (mod == "RTU" || mod == "ASCII"){
		bmod = mod;
	}

	//Modbus ID
	modbus_id = m_id;

	//Select Baudrate
	long br = 9600;
	switch (baud) {
		case 0:
			br = 9600;
		break;
		case 1:
			br = 38400;
		break;
		case 2:
			br = 19200;
		break;
		case 3:
			br = 57600;
		break;
		case 4:
			br = 115200;
		break;
	}

	switch (prt) {
		case 0:
			Serial.begin(br,SERIAL_8N1);
		break;
		case 1:
			Serial.begin(br,SERIAL_7N1);
		break;
		case 2:
			Serial.begin(br,SERIAL_8N2);
		break;
		case 3:
			Serial.begin(br,SERIAL_7N2);
		break;
		case 4:
			Serial.begin(br,SERIAL_8E1);
		break;
		case 5:
			Serial.begin(br,SERIAL_7E1);
		break;
		case 6:
			Serial.begin(br,SERIAL_8E2);
		break;
		case 7:
			Serial.begin(br,SERIAL_7E2);
		break;
		case 8:
			Serial.begin(br,SERIAL_8O1);
		break;
		case 9:
			Serial.begin(br,SERIAL_7O1);
		break;
		case 10:
			Serial.begin(br,SERIAL_8O2);
		break;
		case 11:
			Serial.begin(br,SERIAL_7O2);
		break;
		
		default:
			Serial.begin(9600,SERIAL_8N1);
			break;
	}

	//TX Eneble Pin Output
	pinMode(t_pin, OUTPUT);
  	digitalWrite(t_pin,LOW);
	tp = t_pin;


	//Start Address
	address = adr;
}

//Hex string to byte
byte Bmodbus::hex_convert(String _hex){
	char c[4]= {0};
	_hex.toCharArray(c, 4);
	int i = strtoul(c, nullptr, 16);
	return i; 
}


//Convert Full HEX
String printHex(int num, int precision) {
     char tmp[16];
     char format[128];

     sprintf(format, "%%.%dX", precision);

     sprintf(tmp, format, num);
     return tmp;
}


//RTU CRC
#define UInt16 uint16_t
byte crc16_l;
byte crc16_h;
void Bmodbus::RTU_CRC(byte * buf, int len)
{
  UInt16 crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++) {crc ^= uint16_t (buf[pos]);          
    for (int i = 8; i != 0; i--) {    
      if ((crc & 0x0001) != 0) {crc >>= 1; crc ^= 0xA001;}
      else                            
        crc >>= 1;                    
    }}

  
  crc16_h = highByte(crc);
  crc16_l = lowByte(crc);
}

// ASCII LRC
String lrc_byte;
void Bmodbus::ASCII_LRC(String _mdata)
{
  int sum = 0;
  for (int i = 0; i < _mdata.length(); i+=2) {
          sum += hex_convert(_mdata.substring(i,i+2));  
  }
  int xsum = 0 - sum;
  lrc_byte = String(lowByte(xsum),HEX );
  lrc_byte.toUpperCase();
}



void Bmodbus::RTU_function3 (){

	int data_adr = readData[2] << 8;
	data_adr += readData[3];
	data_adr -= address;

	int data_len = readData[4] << 8;
	data_len += readData[5];

	if(data_adr >= 0 && data_adr < sizeof(reg) ){ //Modbus Addres Limit Control
		sendData[0] = modbus_id;
		sendData[1] = 0x03;
		sendData[2] = data_len*2;

		int len = 3;
		for (int i = 0; i < data_len; i++) {
			sendData[len] = highByte( reg[data_adr + i] );
			sendData[len+1] = lowByte( reg[data_adr + i] );
			len += 2;
		}

		len-=2;
		digitalWrite(tp, HIGH);
		RTU_CRC(sendData,len);
		delay(1);
		Serial.write(sendData,len);
		Serial.write(crc16_l);
		Serial.write(crc16_h);
		Serial.flush();



	}


}

void Bmodbus::RTU_function6 (){

	int data_adr = readData[2] << 8;
	data_adr += readData[3];
	data_adr -= address;

	int data_val = readData[4] << 8;
	data_val += readData[5];

	if(data_adr >= 0 && data_adr < sizeof(reg) ){ //Modbus Addres Limit Control
		reg[data_adr] = data_val;
		digitalWrite(tp, HIGH);
		delay(1);
		Serial.write(readData,readL);
		Serial.flush();
	}


}

void Bmodbus::RTU_function16 (){

	int data_adr = readData[2] << 8;
	data_adr += readData[3];
	data_adr -= address;

	int data_len = readData[4] << 8;
	data_len += readData[5];

	int data_byte = readData[6];

	if(data_adr >= 0 && data_adr < sizeof(reg) ){ //Modbus Addres Limit Control
		
		int len = 7;
		for (int i = 0; i < data_len; i++) {
			reg[data_adr + i] = 0;
			reg[data_adr + i] = readData[len] << 8;
			reg[data_adr + i] += readData[len+1];
			len += 2;
		}
		
		sendData[0] = modbus_id;
		sendData[1] = 0x10;
		sendData[2] = readData[2];
		sendData[3] = readData[3];
		sendData[4] = readData[4];
		sendData[5] = readData[5];

		digitalWrite(tp, HIGH);
		RTU_CRC(sendData,6);
		delay(1);
		Serial.write(sendData,6);
		Serial.write(crc16_l);
		Serial.write(crc16_h);
		Serial.flush();
	}
}


void Bmodbus::ASCII_function3 (){
	
	int data_adr = ( hex_convert(readAscii.substring(5,7)) << 8 )+ hex_convert(readAscii.substring(7,9));
	data_adr -= address;

	int data_len = ( hex_convert(readAscii.substring(9,11)) << 8 )+ hex_convert(readAscii.substring(11,13));

	if(data_adr >= 0 && data_adr < sizeof(reg) ){ //Modbus Addres Limit Control
		String ascii_send = "0403" + printHex(data_len*2,2);
		for (int i = 0; i < data_len; i++) {
			ascii_send += printHex( reg[data_adr + i] , 4);
		}
		digitalWrite(tp, HIGH);
		ASCII_LRC( ascii_send ); //LRC
		delay(1);
		ascii_send = ":"+ascii_send+lrc_byte;
		Serial.println(ascii_send);
		Serial.flush();
	}
}

void Bmodbus::ASCII_function6 (){
	
	int data_adr = ( hex_convert(readAscii.substring(5,7)) << 8 )+ hex_convert(readAscii.substring(7,9));
	data_adr -= address;

	int data_val = ( hex_convert(readAscii.substring(9,11)) << 8 )+ hex_convert(readAscii.substring(11,13));

	if(data_adr >= 0 && data_adr < sizeof(reg) ){ //Modbus Addres Limit Control
		reg[data_adr] = data_val;
		digitalWrite(tp, HIGH);
		delay(1);
		Serial.println(readAscii);
		Serial.flush();
	}
}

void Bmodbus::ASCII_function16 (){
	
	int data_adr = ( hex_convert(readAscii.substring(5,7)) << 8 )+ hex_convert(readAscii.substring(7,9));
	data_adr -= address;

	int data_len = ( hex_convert(readAscii.substring(9,11)) << 8 )+ hex_convert(readAscii.substring(11,13));

	int data_byte = hex_convert(readAscii.substring(13,15));

	if(data_adr >= 0 && data_adr < sizeof(reg) ){ //Modbus Addres Limit Control
		int len = 15;
		for (int i = 0; i < data_len; i++) {
			reg[data_adr + i] = 0;
			reg[data_adr + i] = ( hex_convert(readAscii.substring(len,len+2)) << 8 )+ hex_convert(readAscii.substring(len+2,len+4));
			len += 4;
		}
		String ascii_send = readAscii.substring(1,13);
		digitalWrite(tp, HIGH);
		ASCII_LRC( ascii_send ); //LRC
		delay(1);
		ascii_send = ":"+ascii_send+lrc_byte;
		Serial.println(ascii_send);
		Serial.flush();
	}
}




void Bmodbus::check (){

digitalWrite(tp, LOW);
if (bmod == "RTU"){ //RTU Mod

	if (Serial.available() > 0){
		readL = 0;
		delay (40);
		while(Serial.available() > 0){readData[readL] = Serial.read(); readL++;} 
		delay (1);
		if(modbus_id == readData[0]) // Modbus ID Control
		{
			RTU_CRC(readData,readL-2); //CRC
			if( crc16_l == readData[readL-2] && crc16_h == readData[readL-1] ) //CRC Control
			{
				switch (readData[1]) {
					case 0x3:
						RTU_function3();
					break;
					case 0x6:
						RTU_function6();
					break;
					case 0x10:
						RTU_function16();
					break;

				}
		
			}else{ //CRC Error
			byte err[3] = {modbus_id,readData[1],0x04};
			RTU_CRC(err,3);
			digitalWrite(tp, HIGH);
			delay(1);
			Serial.write(err,3);
			Serial.write(crc16_l);
			Serial.write(crc16_h);
			Serial.flush();
			}
		}
	}
}else if(bmod == "ASCII") { //ASCII Mod

		if (Serial.available() > 0){
			readAscii = "";
			delay (40);
			while(Serial.available() > 0){readAscii += (char)Serial.read();} 
			delay (1);

			if(modbus_id == hex_convert( readAscii.substring(1,3) ) ) // Modbus ID Control
			{
				ASCII_LRC( readAscii.substring(1, readAscii.length()-4) ); //LRC	
				if( readAscii.substring(readAscii.length()-4, readAscii.length()-2) == lrc_byte ) //LRC Control
				{
					switch ( hex_convert( readAscii.substring(3,5) ) ) {
					case 0x3:
						ASCII_function3();
					break;
					case 0x6:
						ASCII_function6();
					break;
					case 0x10:
						ASCII_function16();
					break;

					}
			
				}
			}
		}

	}
}




