#include "wavegen.h"
#include <stdio.h>
#include <math.h>

#define PI 3.1415926

//ISSUES: How to return a populated array generated by math function
//seems like it is always stuck in this functions
int gen_array(int max_points, int point, int type) {
	int val;
	switch (type) {
	//generates values for
	case 1:
		val = 1500 * sin(2 * PI * point / max_points) + 1500;
		break;

	case 2:
		val = 3000 * point / max_points;
		break;

	case 3:
		val = 3000 - 3000 * point / max_points;
		break;
	}
	return val;
}
