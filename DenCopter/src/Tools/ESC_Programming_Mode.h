#ifndef _ESC_PROGRAMMING_MODE_H_
#define _ESC_PROGRAMMING_MODE_H_

//Enter the programming mode of the esc('s)

#include "../Motors/Motor_Driver.h"
#include <iostream>
#include <limits>

void programmingMode(Motor_Driver &driver, Motor** motors);							//Program single ESC

#endif // !_ESC_PROGRAMMING_MODE_H_
