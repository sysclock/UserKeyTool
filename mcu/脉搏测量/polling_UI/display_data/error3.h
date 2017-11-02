/////////////////////////////////////////////////////////////////////////
// Bitmap点阵数据表                                                    //
// 图片: C:\..y_data\e3.bmp,纵向取模下高位,数据排列:从左到右从上到下   //
// 图片尺寸: 128 * 64                                                  //
/////////////////////////////////////////////////////////////////////////
static unsigned char error3[]=
{
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,
      0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,
      0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,
      0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,
      0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x7C,0x82,0x01,0x44,0x28,
      0x10,0x28,0x44,0x01,0x82,0x7C,0x00,0x00,
      0x00,0x00,0xFE,0x01,0x00,0x00,0x01,0xFE,
      0x00,0x00,0xFE,0x01,0x00,0x00,0x01,0xFE,
      0x00,0x00,0xFE,0x01,0x00,0x00,0x01,0xFE,
      0x00,0x00,0x83,0x00,0x10,0x10,0x28,0xC7,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x02,
      0x02,0x02,0x02,0x01,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x01,0x02,0x02,0x01,0x00,
      0x00,0x00,0x00,0x01,0x02,0x02,0x01,0x00,
      0x00,0x00,0x00,0x01,0x02,0x02,0x01,0x00,
      0x00,0x00,0x01,0x02,0x02,0x02,0x02,0x01,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0xC0,0x20,0x10,0x48,0x88,
      0x08,0x88,0x48,0x10,0x20,0xC0,0x00,0x00,
      0x00,0x00,0x00,0x88,0x90,0x00,0x10,0x50,
      0x50,0xF8,0x50,0x50,0x50,0x10,0x00,0x00,
      0x40,0x48,0x50,0x40,0xC0,0x40,0x50,0x48,
      0x40,0x00,0x00,0x00,0xC8,0x10,0x00,0x00,
      0x10,0xD0,0x10,0x10,0x10,0xF0,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x07,0x08,0x10,0x24,0x22,
      0x21,0x22,0x24,0x10,0x08,0x07,0x00,0x00,
      0x00,0x00,0x00,0x00,0x7F,0x20,0x01,0x7F,
      0x15,0x15,0x15,0x55,0x7F,0x01,0x00,0x42,
      0x42,0x22,0x12,0x0A,0x07,0x0A,0x12,0x22,
      0x42,0x42,0x00,0x00,0x7F,0x00,0x11,0x09,
      0x25,0x3F,0x01,0x41,0x40,0x7F,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x80,0x80,0x80,0xC0,
      0xA0,0x80,0x80,0x80,0x80,0x00,0x00,0x00,
      0x40,0x40,0x40,0x00,0x00,0x00,0xE0,0x00,
      0x00,0x00,0x00,0x40,0x80,0xC0,0x40,0x40,
      0x40,0xC0,0x00,0x80,0x00,0xE0,0x00,0x00,
      0xE0,0x20,0x20,0x20,0x60,0xA0,0x20,0x20,
      0xE0,0x00,0x00,0x80,0x80,0x80,0x80,0x80,
      0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0x80,
      0xC0,0xA0,0x80,0xC0,0x00,0xE0,0x80,0x40,
      0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0xFF,0x24,0x24,0x24,
      0x24,0x24,0x24,0x24,0xFF,0x00,0x00,0x62,
      0x5A,0x46,0x52,0x62,0x00,0xC1,0x3F,0x01,
      0x01,0xFF,0x00,0x84,0x48,0x3F,0x80,0x7F,
      0x80,0x3F,0x00,0x3F,0x00,0xFF,0x00,0x01,
      0x5F,0x55,0x55,0x55,0xFF,0x55,0x55,0x55,
      0x5F,0x01,0x00,0x40,0x40,0x3F,0x20,0xA0,
      0x61,0x1F,0x01,0x01,0x01,0xFF,0x00,0x00,
      0xFE,0x2A,0x2A,0xFE,0x00,0xF9,0x42,0x22,
      0x12,0xC3,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,
      0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x01,
      0x01,0x00,0x00,0x00,0x00,0x01,0x00,0x00,
      0x00,0x01,0x00,0x00,0x01,0x01,0x00,0x01,
      0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
      0x01,0x01,0x00,0x00,0x00,0x00,0x01,0x00,
      0x00,0x00,0x00,0x01,0x01,0x00,0x00,0x00,
      0x01,0x00,0x01,0x01,0x00,0x00,0x01,0x01,
      0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
