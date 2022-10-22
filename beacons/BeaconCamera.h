#ifndef __BEACON_CAMERA_H__
#define __BEACON_CAMERA_H__

#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "JpegDecode.h"

#define CS 7

class BeaconCamera
{
public:
  BeaconCamera();
  bool process_jpeg();
  Jpeg jpg;

private:
  ArduCAM myCAM;

  bool start_capture;
};

#endif
