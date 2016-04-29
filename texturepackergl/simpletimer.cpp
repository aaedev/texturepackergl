#include <Windows.h>
#include <mmsystem.h>
#include "log.h"
#include "simpletimer.h"


int ani_counter;         //Counter for screen animations

UINT ani_counter_id = 0;

#pragma comment( lib, "winmm.lib") \



void CALLBACK ani_counter_callback(UINT uTimerID, UINT uMsg, DWORD_PTR param, DWORD_PTR dw1, DWORD_PTR dw2)
{ 
	ani_counter++;

	//Failsafe
	if (ani_counter > 0xffff) ani_counter=0;
} 


void timers_init()
{
    
	ani_counter=0;
	//Set multimedia timer resolution to 1ms
	timeBeginPeriod(1);
	//Install the timer
	ani_counter_id = timeSetEvent(275, 1, ani_counter_callback, 0, TIME_PERIODIC);

}


void timers_shutdown()
{
    //Stop the multimedia timer
    timeEndPeriod(1);
 	timeKillEvent(ani_counter_id);
}


