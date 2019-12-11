/*
Author: Burak DERELİ
Website: www.burakdereli.net
*/

#include <Bmodbus-slave.h>

Bmodbus slave;

void setup() {

 slave.mode("ASCII", 4,0,3,4096); // ( Mod:ASCII/RTU , Modbus ID, Baudrate, TX Enable Pin, Start Address )

}

void loop() {
  slave.check();

  slave.reg[0] = 61; //H1000
  slave.reg[1] = 1461; //H1001
  slave.reg[2] = 30000; //H1002
  slave.reg[15] = 1453; //H100F
  slave.reg[50] = 61; //H1032
}
