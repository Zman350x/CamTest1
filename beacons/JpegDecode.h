#ifndef __JPEG_DECODE_H__
#define __JPEG_DECODE_H__

#include "jpg.h"

class Jpeg
{
public:
  Jpeg();

  byte data[5600];
  uint data_len;

private:
  Header header;
};

#endif