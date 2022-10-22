#include "BeaconCamera.h"

BeaconCamera::BeaconCamera() : myCAM(OV2640, CS), jpg()
{
  start_capture = true;

  Wire.begin();
  //Serial.begin(921600);
  //Serial.println(F("ACK CMD ArduCAM Start! END"));

  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  // initialize SPI:
  SPI.begin();
    //Reset the CPLD
  myCAM.write_reg(0x07, 0x80);
  delay(100);
  myCAM.write_reg(0x07, 0x00);
  delay(100);

  byte vid, pid, temp, temp_last;

  while (1)
  {
    //Check if the ArduCAM SPI bus is OK
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55)
    {
      //Serial.println(F("ACK CMD SPI interface Error!END"));
      delay(1000); continue;
    }
    else
    {
      //Serial.println(F("ACK CMD SPI interface OK.END")); break;
    }
  }

  while (1)
  {
    //Check if the camera module type is OV2640
    myCAM.wrSensorReg8_8(0xff, 0x01);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 )))
    {
      //Serial.println(F("ACK CMD Can't find OV2640 module!"));
      delay(1000); continue;
    }
    else
    {
      //Serial.println(F("ACK CMD OV2640 detected.END")); break;
    }
  }
  //Change to JPEG capture mode and initialize the OV2640 module
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
  myCAM.OV2640_set_JPEG_size(OV2640_160x120);
  delay(1000);
  myCAM.clear_fifo_flag();
}

bool BeaconCamera::process_jpeg()
{
  if (start_capture)
  {
    myCAM.flush_fifo();
    myCAM.clear_fifo_flag();
    //Start capture
    myCAM.start_capture();
    start_capture = false;
  }
  if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
  {
    bool is_header = false;
    uint length = 0;
    length = myCAM.read_fifo_length();
    if ((length >= MAX_FIFO_SIZE) | (length == 0))
    {
      myCAM.clear_fifo_flag();
      start_capture = true;
      return false;
    }
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();//Set fifo burst mode
    byte temp = SPI.transfer(0x00);
    byte temp_last;
    length --;
    while ( length-- )
    {
      temp_last = temp;
      temp = SPI.transfer(0x00);
      if (is_header == true)
      {
        //char buf1[5];
        //sprintf(buf1, "%02x, ", temp);
        ////Serial.write(buf1);
        jpg.data[jpg.data_len] = temp;
        jpg.data_len++;
      }
      else if ((temp == SOS) & (temp_last == MARKER))
      {
        is_header = true;
        jpg.data_len = 0;
        /*//Serial.println(F("ACK CMD IMG END"));
        char buf2[9];
        sprintf(buf2, "%02x, %02x, ", temp_last, temp);
        //Serial.write(buf2);*/
      }
      if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end, break while
      {
        break;
      }
      delayMicroseconds(15);
    }
    myCAM.CS_HIGH();
    myCAM.clear_fifo_flag();
    start_capture = true;
    is_header = false;

    return true;
  }
  return false;
}