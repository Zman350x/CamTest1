#include "BeaconCamera.h"

BeaconCamera camera = BeaconCamera();

void setup()
{
  Serial.begin(921600);
}

void loop()
{
  if (camera.process_jpeg())
  {
    for (int i = 0; i < camera.jpg.data_len; i++)
    {
      char buf[5];
      sprintf(buf, "%02x, ", (uint) camera.jpg.data[i]);
      Serial.write(buf);
    }
    Serial.println();
  }
}