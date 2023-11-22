/*============================================================================*/
/*
SimpleRTOS - Very simple RTOS for Microcontrollers
v2.00 (2014-01-21)
isaacbavaresco@yahoo.com.br
*/
/*============================================================================*/
/*
 Copyright (c) 2007-2014, Isaac Marino Bavaresco
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Neither the name of the author nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*============================================================================*/
#if			defined __XC32__ || defined __C32_VERSION__
/*============================================================================*/

	/*------------------------------------------------------------------------*/
	#if         defined __PIC32MX__ || defined __PIC32MZ__
	/*------------------------------------------------------------------------*/

		#include "PortPIC32Internals.h"

	/*------------------------------------------------------------------------*/
	#else   /*  defined __PIC32MX__ || defined __PIC32MZ__ */
	/*------------------------------------------------------------------------*/

		#error "Platform not supported."

	/*------------------------------------------------------------------------*/
	#endif  /*  defined __PIC32MX__ || defined __PIC32MZ__ */
	/*------------------------------------------------------------------------*/

/*============================================================================*/
#elif		defined __XC16__
/*============================================================================*/

	/*------------------------------------------------------------------------*/
	#if         defined __dsPIC33E__
	/*------------------------------------------------------------------------*/

		#include "PortPIC24dsPICInternals.h"

	/*------------------------------------------------------------------------*/
	#elif       defined __dsPIC33F__
	/*------------------------------------------------------------------------*/

		#include "PortPIC24dsPICInternals.h"

	/*------------------------------------------------------------------------*/
	#elif       defined __dsPIC30F__
	/*------------------------------------------------------------------------*/

		#include "PortPIC24dsPICInternals.h"

	/*------------------------------------------------------------------------*/
	#elif   	defined __PIC24E__ || defined __PIC24H__ || defined __PIC24F__ || defined __PIC24FK__
	/*------------------------------------------------------------------------*/

		#include "PortPIC24dsPICInternals.h"

	/*------------------------------------------------------------------------*/
	#else   /*  defined __PIC24E__ || defined __PIC24H__ || defined __PIC24F__ || defined __PIC24FK__ */
	/*------------------------------------------------------------------------*/

		#error "Platform not supported."

	/*------------------------------------------------------------------------*/
	#endif  /*  defined __dsPIC33E__ */
	/*------------------------------------------------------------------------*/

/*============================================================================*/
#elif		defined __XC8__ || defined HI_TECH_C || defined __18CXX
/*============================================================================*/

	#include "PortPIC18Internals.h"

/*============================================================================*/
#elif		defined __arm__
/*============================================================================*/

#include "PortSAM3Internals.h"

/*============================================================================*/
#else   /*  defined __XC8__ || defined HI_TECH_C || defined __18CXX */
/*============================================================================*/

	#error "Platform not supported."

/*============================================================================*/
#endif  /* defined __XC32__ || defined __C32_VERSION__ */
/*============================================================================*/
