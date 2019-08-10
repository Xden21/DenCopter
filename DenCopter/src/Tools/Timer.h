#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <iostream>

////////////////////////////////////////////////////////
// Timer function that times from a reference point	  // 
// And gives the time since the reference point		  //
////////////////////////////////////////////////////////


using chrono = std::chrono::high_resolution_clock;
using us = std::chrono::microseconds;

static chrono::time_point startTime;
static bool timer_started = false;

// Start the timer (set the reference time)
void startTimer();


// Return the time in microseconds from the high resulution
us getTimeU();

#endif