/*============================================================================*/
#include "Portable.h"
/*============================================================================*/
#define MAX_PRIORITIES  (5)
/*============================================================================*/

const unsigned int  MaxPriorities   = MAX_PRIORITIES;

/*============================================================================*/
/*
Points to the head of the running tasks contexts list. This list is organized
as a double-linked circular list.
*/

context_t           *ReadyTasks[MAX_PRIORITIES];

/*============================================================================*/
