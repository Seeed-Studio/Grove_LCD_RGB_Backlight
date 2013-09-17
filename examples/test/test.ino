// test i2c of grove - lcd rgb backlight
// loovee 2013-9-7

#include <Wire.h>
#include <Streaming.h>

const int I2C_ADDRESS = 0x7c>>1;
void setup()
{
    Serial.begin(115200);
    Wire.begin();
    
    cout << "hello world" << endl;
}

void loop()
{
    sendI2cTest();
    delay(100);
}

void sendI2cTest()
{
    Wire.beginTransmission(I2C_ADDRESS);    // transmit to device #4
    Wire.write("a");                        // sends five bytes

    Wire.endTransmission();                 // stop transmitting
}

