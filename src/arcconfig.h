#pragma once


/*
	Color quantization function
	quantizeScale8: Scales float to u8
	quantizeRound8: Scales float to u8 and rounds
*/

#define ARC_COLOR_QUANTIZE		quantizeRound8


/*
	Default color channel ordering
	ARGB: Alpha highest byte
	RGBA: Alpha lowest byte
*/
#define ARC_COLOR_DEFAULT_CHANNEL_ORDER	ARGB


/*
	Task Executor sleep policy
	ARC_TASK_SPIN: Spin when queue empty
	ARC_TASK_SLEEP_ATOMIC: Atomic wait until new element arrives
	ARC_TASK_PERIODIC_SLEEP: Sleep for a certain amount of time (ARC_TASK_SLEEP_DURATION in us)
*/

#define ARC_TASK_SLEEP_DURATION	50
#define ARC_TASK_SPIN
//#define ARC_TASK_SLEEP_ATOMIC
//#define ARC_TASK_PERIODIC_SLEEP


/*
	Task Executor termination method (destruction only)
	ARC_TASK_END_WAIT: Assists until tasks are finished
*/

#define ARC_TASK_END_WAIT


/*
	Log settings
	ARC_LOG_STDIO_UNSYNC: Unsyncs stdio from cout. Logging is accelerated but data races become possible.
	ARC_LOG_EXCEPTION_ABORT: Aborts when Log throws. Do not use in production code!
*/

#define ARC_LOG_STDIO_UNSYNC
#define ARC_LOG_EXCEPTION_ABORT


/*
	Allocator debugging
	ARC_ALLOCATOR_DEBUG_CHECKS: Enables debug assertions during heap creation/allocation
	ARC_ALLOCATOR_DEBUG_LOG: Logs all allocations performed with Arclight Allocators
*/

#define ARC_ALLOCATOR_DEBUG_CHECKS
#define ARC_ALLOCATOR_DEBUG_LOG
