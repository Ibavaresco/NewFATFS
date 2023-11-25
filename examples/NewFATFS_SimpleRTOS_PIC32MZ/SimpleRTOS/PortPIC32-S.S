/*============================================================================*/
/*
SimpleRTOS - Very simple RTOS for Microcontrollers - PIC32 port
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
#if		defined __XC32__ || defined __C32_VERSION__
/*============================================================================*/
#include "PortPIC32Internals.h"
/*============================================================================*/
#include <p32xxxx.h>
#include <sys/asm.h>
/*============================================================================*/
		.set		nomips16
		.set		noreorder
/*============================================================================*/
		.extern		Switch
		.extern		errno
		.extern		FPUOwner
/*============================================================================*/
	 	.set		noreorder
		.set 		noat

 		.global		SaveContext

 		.ent		SaveContext

SaveContext:	sw		s0,0(sp)

SaveContext2:	lw		s0,CurrentTask

		sw		s1,64(s0)
		sw		s2,68(s0)

		lw		s1,0(sp)
		sw		s1,60(s0)
		lw		s1,4(sp)
		sw		s1,120(s0)
		addiu		sp,sp,8

		lw		s1,errno
#if	    defined __PIC32MK__ || defined __PIC32MZ__
		sw		s1,172(s0)
#else	/*  defined __PIC32MK__ || defined __PIC32MZ__ */
		sw		s1,144(s0)
#endif	/*  defined __PIC32MK__ || defined __PIC32MZ__ */

		mfc0		s1,_CP0_EPC
		sw		s1, 132(s0)

		mfc0		s1,_CP0_STATUS
		sw		s1, 128(s0)
		ori		s2,zero,MAX_SYSCALL_INTERRUPT_PRIORITY
		ins		s1,s2,10,9
		ins		s1,zero,1,4

		/* Re-enable interrupts */
		mtc0		s1,_CP0_STATUS

		/* Here the interrups above kernel priority are re-enabled */

		sw		 $1,  0(s0)
		sw		 $2,  4(s0)
		sw		 $3,  8(s0)
		sw		 $4, 12(s0)
		sw		 $5, 16(s0)
		sw		 $6, 20(s0)
		sw		 $7, 24(s0)
		sw		 $8, 28(s0)
		sw		 $9, 32(s0)
		sw		$10, 36(s0)
		sw		$11, 40(s0)
		sw		$12, 44(s0)
		sw		$13, 48(s0)
		sw		$14, 52(s0)
		sw		$15, 56(s0)
/*
		sw		$16, 60(s0)	; s0 and
		sw		$17, 64(s0)	; s1 were already saved at the very beginning
		sw		$18, 68(s0)	; s2
*/
		sw		$19, 72(s0)
		sw		$20, 76(s0)
		sw		$21, 80(s0)
		sw		$22, 84(s0)
		sw		$23, 88(s0)
		sw		$24, 92(s0)
		sw		$25, 96(s0)
		sw		$26,100(s0)
		sw		$27,104(s0)
		sw		$28,108(s0)
		sw		$29,112(s0)
		sw		$30,116(s0)
/*		sw		$31,120(s0)	; Already saved */

		mfhi		s1,$ac0
		sw		s1, 136(s0)
		mflo		s1,$ac0
		sw		s1, 140(s0)

#if	    defined __PIC32MK__ || defined __PIC32MZ__
		mfhi		s1,$ac1
		sw		s1, 144(s0)
		mflo		s1,$ac1
		sw		s1, 148(s0)

		mfhi		s1,$ac2
		sw		s1, 152(s0)
		mflo		s1,$ac2
		sw		s1, 156(s0)

		mfhi		s1,$ac3
		sw		s1, 160(s0)
		mflo		s1,$ac3
		sw		s1, 164(s0)

		rddsp		s1
		sw		s1, 168(s0)
#endif	/*  defined __PIC32MK__ || defined __PIC32MZ__ */

		jr		ra
		nop

 		.end		SaveContext
/*============================================================================*/
	 	.set		noreorder
		.set 		noat
		.global		TickInterrupt
		.global		RestoreContext
		.extern		SchedulerSuspended
		.extern		IFS0CLR

 		.ent		TickInterrupt

TickInterrupt:	addiu		sp,sp,-8
    
		sw		s0,0(sp)

		lw		s0,SchedulerSuspended
		beqz		s0,1f

  		lui		s0,%hi(SystemTick)
		sw		s1,4(sp)
		lw		s1,%lo(SystemTick)(s0)
		addiu		s1,s1,1
		sw		s1,%lo(SystemTick)(s0)

		lui		s0,%hi(IFS0CLR)
#if         !defined USE_CORE_TIMER || USE_CORE_TIMER == 0
		li		s1,_IFS0_T1IF_MASK	/* Timer1 */
#else	/*  !defined USE_CORE_TIMER || USE_CORE_TIMER == 0 */
		li		s1,_IFS0_CTIF_MASK	/* Core Timer */
#endif	/*  !defined USE_CORE_TIMER || USE_CORE_TIMER == 0 */
		sw		s1,%lo(IFS0CLR)(s0)
  
		lw		s0,0(sp)
		lw		s1,4(sp)

		addiu		sp,sp,8
		eret
    
1:		sw		ra,4(sp)

		jal		SaveContext2
		nop
		/*------------------------------------------------------------*/
		jal		Switch
		nop
		/*------------------------------------------------------------*/
 		.end		TickInterrupt

		.ent		RestoreContext
RestoreContext:
		lw		s0,CurrentTask

		lw		 $1,  0(s0)
		lw		 $2,  4(s0)
		lw		 $3,  8(s0)
		lw		 $4, 12(s0)
		lw		 $5, 16(s0)
		lw		 $6, 20(s0)
		lw		 $7, 24(s0)
		lw		 $8, 28(s0)
		lw		 $9, 32(s0)
		lw		$10, 36(s0)
		lw		$11, 40(s0)
		lw		$12, 44(s0)
		lw		$13, 48(s0)
		lw		$14, 52(s0)
		lw		$15, 56(s0)
/*		lw		$16, 60(s0)	; s0 and			*/
/*		lw		$17, 64(s0)	; s1 will be restored last	*/
/*		lw		$18, 68(s0)	; s2				*/
/*		lw		$19, 72(s0)	; s3				*/
		lw		$20, 76(s0)
		lw		$21, 80(s0)
		lw		$22, 84(s0)
		lw		$23, 88(s0)
		lw		$24, 92(s0)
		lw		$25, 96(s0)
		lw		$26,100(s0)
		lw		$27,104(s0)
		lw		$28,108(s0)
		lw		$29,112(s0)
		lw		$30,116(s0)
		lw		$31,120(s0)

		lw		s1, 136(s0)
		mthi		s1,$ac0
		lw		s1, 140(s0)
		mtlo		s1,$ac0

#if	    defined __PIC32MK__ || defined __PIC32MZ__
		lw		s1, 144(s0)
		mthi		s1,$ac1
		lw		s1, 148(s0)
		mtlo		s1,$ac1

		lw		s1, 152(s0)
		mthi		s1,$ac2
		lw		s1, 156(s0)
		mtlo		s1,$ac2

		lw		s1, 160(s0)
		mthi		s1,$ac3
		lw		s1, 164(s0)
		mtlo		s1,$ac3

		lw		s1, 168(s0)
		wrdsp		s1
#endif	/*  defined __PIC32MK__ || defined __PIC32MZ__ */

/* Here we must disable interrups */

#if	    defined __PIC32MK__ || ( defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' )

		lw		s2,FPUOwner
		beq		s0,s2,1f
		lw		s1, 128(s0)
		ins		s1,zero,29,1
1:		

#else	/*  defined __PIC32MK__ || ( defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' ) */
		lw		s1, 128(s0)
#endif	/*  defined __PIC32MK__ || ( defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' ) */

		mtc0		s1,_CP0_STATUS

		lw		s1, 132(s0)
		mtc0		s1,_CP0_EPC

#if	    defined __PIC32MK__ || defined __PIC32MZ__
		lw		s1,172(s0)
#else	/*  defined __PIC32MK__ || defined __PIC32MZ__ */
		lw		s1,144(s0)
#endif	/*  defined __PIC32MK__ || defined __PIC32MZ__ */
		la		s2,errno
		sw		s1,(s2)

		lw		s3,72(s0)
		lw		s2,68(s0)
		lw		s1,64(s0)
		lw		s0,60(s0)

		eret
		/*------------------------------------------------------------*/
		.end		RestoreContext
/*============================================================================*/
		.extern		CurrentTask
/*============================================================================*/
		.global		ForceYield
 		.ent		ForceYield

ForceYield:	mfc0		t0,_CP0_STATUS
		ori		t0,t0,2		/* Disable interrupts */
		mtc0		t0,_CP0_STATUS
		ehb
		mtc0		ra,_CP0_EPC
		ehb
		/*------------------------------------------------------------*/
		addiu		sp,sp,-8
		sw		ra,4(sp)

		jal		SaveContext
		nop
		/*------------------------------------------------------------*/
		lw		t0,HighestReadyPriority
		sll		t0,t0,2
		la		t1,ReadyTasks
		addu		t1,t1,t0
		lw		t0,0(t1)

		la		t1,CurrentTask
		sw		t0,0(t1)
		/*------------------------------------------------------------*/
		j		RestoreContext
		nop
		/*------------------------------------------------------------*/
		.end		ForceYield
/*============================================================================*/
#endif	/*  defined __XC32__  || defined __C32_VERSION__ */
/*============================================================================*/

#if	    defined __PIC32MK__ || ( defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' )

		.global		SaveFP
 		.ent		SaveFP

SaveFP:		sdc1	$f0,0(a0)
		sdc1	$f1,8(a0)
		sdc1	$f2,16(a0)
		sdc1	$f3,24(a0)
		sdc1	$f4,32(a0)
		sdc1	$f5,40(a0)
		sdc1	$f6,48(a0)
		sdc1	$f7,56(a0)
		sdc1	$f8,64(a0)
		sdc1	$f9,72(a0)
		sdc1	$f10,80(a0)
		sdc1	$f11,88(a0)
		sdc1	$f12,96(a0)
		sdc1	$f13,104(a0)
		sdc1	$f14,112(a0)
		sdc1	$f15,120(a0)
		sdc1	$f16,128(a0)
		sdc1	$f17,136(a0)
		sdc1	$f18,144(a0)
		sdc1	$f19,152(a0)
		sdc1	$f20,160(a0)
		sdc1	$f21,168(a0)
		sdc1	$f22,176(a0)
		sdc1	$f23,184(a0)
		sdc1	$f24,192(a0)
		sdc1	$f25,200(a0)
		sdc1	$f26,208(a0)
		sdc1	$f27,216(a0)
		sdc1	$f28,224(a0)
		sdc1	$f29,232(a0)
		sdc1	$f30,240(a0)
		sdc1	$f31,248(a0)
		cfc1	a1, $f31
		sw	a1,256(a0)

		jr	ra
		nop

		.end	SaveFP

/*============================================================================*/

		.global		RestoreFP
 		.ent		RestoreFP

RestoreFP:	lw	a1,256(a0)
		ctc1	a1, $f31
		
		ldc1	$f31,248(a0)
		ldc1	$f30,240(a0)
		ldc1	$f29,232(a0)
		ldc1	$f28,224(a0)
		ldc1	$f27,216(a0)
		ldc1	$f26,208(a0)
		ldc1	$f25,200(a0)
		ldc1	$f24,192(a0)
		ldc1	$f23,184(a0)
		ldc1	$f22,176(a0)
		ldc1	$f21,168(a0)
		ldc1	$f20,160(a0)
		ldc1	$f19,152(a0)
		ldc1	$f18,144(a0)
		ldc1	$f17,136(a0)
		ldc1	$f16,128(a0)
		ldc1	$f15,120(a0)
		ldc1	$f14,112(a0)
		ldc1	$f13,104(a0)
		ldc1	$f12,96(a0)
		ldc1	$f11,88(a0)
		ldc1	$f10,80(a0)
		ldc1	$f9,72(a0)
		ldc1	$f8,64(a0)
		ldc1	$f7,56(a0)
		ldc1	$f6,48(a0)
		ldc1	$f5,40(a0)
		ldc1	$f4,32(a0)
		ldc1	$f3,24(a0)
		ldc1	$f2,16(a0)
		ldc1	$f1,8(a0)
		ldc1	$f0,0(a0)

		jr	ra
		nop

		.end	RestoreFP

#endif	/*  defined __PIC32MK__ || ( defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' ) */

/*============================================================================*/
