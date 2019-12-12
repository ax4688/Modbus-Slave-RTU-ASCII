/*	Create: Burak DERELİ
	Contact: www.burakdereli.net
	Version: v0.20
*/

#ifndef Bmodbus_slave_H
#define Bmodbus_slave_H

#include <Arduino.h>

class Bmodbus {
	
	public:
	void mode (String mod , int m_id , int baud, byte t_pin, int adr); // ( Mod:ASCII/RTU , Modbus ID, Baudrate, TX Enable Pin, Start Adress )
	int reg[100];
	bool input[20];
	bool output[20];
	void check();
	private:
	void RTU_CRC(byte * buf, int len);
	void ASCII_LRC(String _mdata);
	byte hex_convert(String _hex);
	void RTU_function1();
	void RTU_function2();
	void RTU_function3();
	void RTU_function4();
	void RTU_function5();
	void RTU_function6();
	void RTU_function15();
	void RTU_function16();
	void ASCII_function1();
	void ASCII_function2();
	void ASCII_function3();
	void ASCII_function4();
	void ASCII_function5();
	void ASCII_function6();
	void ASCII_function15();
	void ASCII_function16();
};

#endif

