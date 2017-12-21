#include "ofme_filter.h"

float ofme_filter(float radian_filted, float radian, float radian_pt)
{
#if 1
// Ò»½×»¥²¹ÂË²¨
	radian_filted = 0.90*(radian_filted + radian_pt) +0.10*radian;

#endif
	return radian_filted;

}
