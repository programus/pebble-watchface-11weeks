#include "necessary.h"

#define SCREEN_WIDTH  144
#define SCREEN_HEIGHT 168

#define SX        23    // start point x
#define SY        18    // start point y
#define DX        3     // margin x
#define DY        3     // margin y
#define CW        14    // cell width
#define CH        12    // cell height
#define WN        11    // number of weeks
#define DW        7     // days in a week

#define SEC_SX    SX
#define SEC_SY    (SY + CH * WN + DY)
#define SEC_W     (CW * DW - 1)
#define SEC_H     5
