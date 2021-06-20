/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Timer objects and scheduling management

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/



#include "timer.h"
/*!
 * This flag is used to make sure we have looped through the main several time to avoid race issues
 */
volatile uint8_t HasLoopedThroughMain = 0;

/*!
 * Timers list head pointer
 */
static TimerEvent_t *TimerListHead = NULL;

/*!
 * \brief Adds or replace the head timer of the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *         next timer to expire.
 *
 * \param [IN]  obj Timer object to be become the new head
 * \param [IN]  remainingTime Remaining time of the previous head to be replaced
 */
static void TimerInsertNewHeadTimer( TimerEvent_t *obj, uint32_t remainingTime );

/*!
 * \brief Adds a timer to the list.
 *
 * \remark The list is automatically sorted. The list head always contains the
 *         next timer to expire.
 *
 * \param [IN]  obj Timer object to be added to the list
 * \param [IN]  remainingTime Remaining time of the running head after which the object may be added
 */
static void TimerInsertTimer( TimerEvent_t *obj, uint32_t remainingTime );

/*!
 * \brief Sets a timeout with the duration "timestamp"
 *
 * \param [IN] timestamp Delay duration
 */
static void TimerSetTimeout( TimerEvent_t *obj );

/*!
 * \brief Check if the Object to be added is not already in the list
 *
 * \param [IN] timestamp Delay duration
 * \retval true (the object is already in the list) or false
 */
static bool TimerExists( TimerEvent_t *obj );

/*!
 * \brief Read the timer value of the currently running timer
 *
 * \retval value current timer value
 */
TimerTime_t TimerGetValue( void );

void TimerInit( TimerEvent_t *obj, void ( *callback )( void ) )
{
    obj->Timestamp = 0;
    obj->ReloadValue = 0;
    obj->IsRunning = false;
    obj->Callback = callback;
    obj->Next = NULL;
	#ifdef TimerDebug
    LOG("##i am in TimerInit\n\r");
    LOG("		-Timer Init with pointer=%d\n\r",obj);
	#endif
}

void TimerStart( TimerEvent_t *obj )
{
    uint32_t elapsedTime = 0;
    uint32_t remainingTime = 0;
	#ifdef TimerDebug
    LOG("##i am in TimerStart \n\r");
	#endif

    __disable_irq( );

    if( ( obj == NULL ) || ( TimerExists( obj ) == true ) )
    {
        __enable_irq( );
	#ifdef TimerDebug
        LOG("		-timer input is null or this timer Exist in list Timers \n\r");
	#endif
        return;

    }


    obj->Timestamp = obj->ReloadValue;
    obj->IsRunning = false;

    if( TimerListHead == NULL )
    {
        TimerInsertNewHeadTimer( obj, obj->Timestamp );
		#ifdef TimerDebug
        LOG("		-timer add list in heads Timer list \n\r");
		#endif
    }
    else
    {
		#ifdef TimerDebug
    	LOG("		-timer add list but not head \n\r");
		#endif
        if( TimerListHead->IsRunning == true )
        {
            elapsedTime = TimerGetValue( );
            if( elapsedTime > TimerListHead->Timestamp )
            {
                elapsedTime = TimerListHead->Timestamp; // security but should never occur
            }
            remainingTime = TimerListHead->Timestamp - elapsedTime;
        }
        else
        {
            remainingTime = TimerListHead->Timestamp;
        }

        if( obj->Timestamp < remainingTime )
        {
            TimerInsertNewHeadTimer( obj, remainingTime );
			#ifdef TimerDebug
            	LOG("		-timer add list in head timer list2 \n\r");
			#endif
        }
        else
        {
             TimerInsertTimer( obj, remainingTime );
			#ifdef TimerDebug
             	 LOG("		-timer inser timer \n\r");
			#endif
        }
    }
    __enable_irq( );
}

static void TimerInsertTimer( TimerEvent_t *obj, uint32_t remainingTime )
{
    uint32_t aggregatedTimestamp = 0;      // hold the sum of timestamps
    uint32_t aggregatedTimestampNext = 0;  // hold the sum of timestamps up to the next event

    TimerEvent_t* prev = TimerListHead;
    TimerEvent_t* cur = TimerListHead->Next;

    if( cur == NULL )
    { // obj comes just after the head
        obj->Timestamp -= remainingTime;
        prev->Next = obj;
        obj->Next = NULL;
    }
    else
    {
        aggregatedTimestamp = remainingTime;
        aggregatedTimestampNext = remainingTime + cur->Timestamp;

        while( prev != NULL )
        {
            if( aggregatedTimestampNext > obj->Timestamp )
            {
                obj->Timestamp -= aggregatedTimestamp;
                if( cur != NULL )
                {
                    cur->Timestamp -= obj->Timestamp;
                }
                prev->Next = obj;
                obj->Next = cur;
                break;
            }
            else
            {
                prev = cur;
                cur = cur->Next;
                if( cur == NULL )
                { // obj comes at the end of the list
                    aggregatedTimestamp = aggregatedTimestampNext;
                    obj->Timestamp -= aggregatedTimestamp;
                    prev->Next = obj;
                    obj->Next = NULL;
                    break;
                }
                else
                {
                    aggregatedTimestamp = aggregatedTimestampNext;
                    aggregatedTimestampNext = aggregatedTimestampNext + cur->Timestamp;
                }
            }
        }
    }
}

static void TimerInsertNewHeadTimer( TimerEvent_t *obj, uint32_t remainingTime )
{
    TimerEvent_t* cur = TimerListHead;
	#ifdef TimerDebug
    	LOG(" ## i am in TimerInsertHeadTimer\n\r");
	#endif

    if( cur != NULL )
    {
        cur->Timestamp = remainingTime - obj->Timestamp;
        cur->IsRunning = false;
		#ifdef TimerDebug
        	LOG("		-cur != NULL \n\r");
		#endif
    }

    obj->Next = cur;
    obj->IsRunning = true;
    TimerListHead = obj;
    TimerSetTimeout( TimerListHead );

}

void TimerIrqHandler( void )
{
    uint32_t elapsedTime = 0;

    elapsedTime = TimerGetValue( );
	#ifdef TimerDebug
    	LOG("## i am in TimerIrqHandler \n\r");
    	LOG("		- elaspsed Time =%d \n\r",elapsedTime);
	#endif

    if( elapsedTime >= TimerListHead->Timestamp )
    {
        TimerListHead->Timestamp = 0;
		#ifdef TimerDebug
        LOG(" 		- elapsedTime >= TimerListHead->Timestamp\n\r");
		#endif
    }
    else
    {
        TimerListHead->Timestamp -= elapsedTime;
		#ifdef TimerDebug
        LOG(" 		- elapsedTime !>= TimerListHead->Timestamp \n\r");
		#endif
    }

    TimerListHead->IsRunning = false;

    while( ( TimerListHead != NULL ) && ( TimerListHead->Timestamp == 0 ) )
    {
        TimerEvent_t* elapsedTimer = TimerListHead;
        TimerListHead = TimerListHead->Next;

        if( elapsedTimer->Callback != NULL )
        {
            elapsedTimer->Callback( );
        }
    }

    // start the next TimerListHead if it exists
    if( TimerListHead != NULL )
    {
        if( TimerListHead->IsRunning != true )
        {
            TimerListHead->IsRunning = true;
            TimerSetTimeout( TimerListHead );
        }
    }
}

void TimerStop( TimerEvent_t *obj )
{
    __disable_irq( );

    uint32_t elapsedTime = 0;
    uint32_t remainingTime = 0;

    TimerEvent_t* prev = TimerListHead;
    TimerEvent_t* cur = TimerListHead;

	#ifdef TimerDebug
    	LOG("##i am in TimerStop \n\r");
	#endif
    // List is empty or the Obj to stop does not exist
    if( ( TimerListHead == NULL ) || ( obj == NULL ) )
    {
        __enable_irq( );
		#ifdef TimerDebug
        	LOG("		-timer list is empty or input object timer parameter is null \n\r");
		#endif
        return;
    }

    if( TimerListHead == obj ) // Stop the Head
    {
		#ifdef TimerDebug
    		LOG("		-input object parameter is head timer list \n\r");
		#endif

    	if( TimerListHead->IsRunning == true ) // The head is already running
        {

            elapsedTime = TimerGetValue( );
			#ifdef TimerDebug
            	LOG("		-this head timer list that we want stop that is running \n\r");
			#endif
            if( elapsedTime > obj->Timestamp )
            {
                elapsedTime = obj->Timestamp;
            }

            remainingTime = obj->Timestamp - elapsedTime;

            if( TimerListHead->Next != NULL )
            {
                TimerListHead->IsRunning = false;
                TimerListHead = TimerListHead->Next;
                TimerListHead->Timestamp += remainingTime;
                TimerListHead->IsRunning = true;
                TimerSetTimeout( TimerListHead );
            }
            else
            {
                TimerListHead = NULL;
            }
        }
        else // Stop the head before it is started
        {
			#ifdef TimerDebug
        		LOG("		-this timer that we want stop that is head but is not running \n\r");
			#endif
            if( TimerListHead->Next != NULL )
            {
                remainingTime = obj->Timestamp;
                TimerListHead = TimerListHead->Next;
                TimerListHead->Timestamp += remainingTime;
            }
            else
            {
                TimerListHead = NULL;
            }
        }
    }
    else // Stop an object within the list
    {
		#ifdef TimerDebug
    		LOG("		- this timer we want stop that is a object on list timers \n\r");
		#endif
        remainingTime = obj->Timestamp;

        while( cur != NULL )
        {
            if( cur == obj )
            {
                if( cur->Next != NULL )
                {
                    cur = cur->Next;
                    prev->Next = cur;
                    cur->Timestamp += remainingTime;
                }
                else
                {
                    cur = NULL;
                    prev->Next = cur;
                }
                break;
            }
            else
            {
                prev = cur;
                cur = cur->Next;
            }
        }
    }
    __enable_irq( );
}

static bool TimerExists( TimerEvent_t *obj )
{
    TimerEvent_t* cur = TimerListHead;

    while( cur != NULL )
    {
        if( cur == obj )
        {
            return true;
        }
        cur = cur->Next;
    }
    return false;
}

void TimerReset( TimerEvent_t *obj )
{
    TimerStop( obj );
    TimerStart( obj );
}

void TimerSetValue( TimerEvent_t *obj, uint32_t value )
{
    TimerStop( obj );
    obj->Timestamp = value;
    obj->ReloadValue = value;
	#ifdef TimerDebug
    LOG("##i am in TimerSetValue\n\r");
    LOG("          -i set value timer with pointer =%d and wieh valu=%d \n\r",obj,value);
	#endif
}

TimerTime_t TimerGetValue( void )
{
    return RtcGetElapsedAlarmTime( );
}

TimerTime_t TimerGetCurrentTime( void )
{
    return RtcGetTimerValue( );
}

TimerTime_t TimerGetElapsedTime( TimerTime_t savedTime )
{
    return RtcComputeElapsedTime( savedTime );
}

TimerTime_t TimerGetFutureTime( TimerTime_t eventInFuture )
{
    return RtcComputeFutureEventTime( eventInFuture );
}

static void TimerSetTimeout( TimerEvent_t *obj )
{
    HasLoopedThroughMain = 0;
	#ifdef TimerDebug
    	LOG("## i am in TimerSetTimout \n\r");
    	LOG("		- timestamp input = %d\n\r", obj->Timestamp);
	#endif
    obj->Timestamp = RtcGetAdjustedTimeoutValue( obj->Timestamp );
	#ifdef TimerDebug
    	LOG("		-time stamp change with RtcGrtAdjustTimeout=%d \n\r",obj->Timestamp);
	#endif
    RtcSetTimeout( obj->Timestamp );
}


void TimerLowPowerHandler( void )
{
//    if( ( TimerListHead != NULL ) && ( TimerListHead->IsRunning == true ) )
//    {
//        if( HasLoopedThroughMain < 1 )
//        {
//            HasLoopedThroughMain++;
//        }
//        else
//        {
//            HasLoopedThroughMain = 0;
//            if( GetBoardPowerSource() == BATTERY_POWER )
//            {
//            	LOG("i go to stopmode\n\r");
//                RtcEnterLowPowerStopMode( );
//                LOG("i wake up in stop mode wihe system clock =%d\n\r",SystemCoreClock);
//            }
//        }
//    }
}
