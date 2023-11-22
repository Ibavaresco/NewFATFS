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
#define MAX_SYSCALL_INTERRUPT_PRIORITY	0x03
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

#if	    defined __PIC32MZ__
		addiu		sp,sp,-(40*4)
#else	/*  defined __PIC32MZ__ */
		addiu		sp,sp,-(34*4)
#endif	/*  defined __PIC32MZ__ */

		sw		s1,64(sp)

		mfc0		s0,_CP0_EPC
		sw		s0, 120(sp)

		mfc0		s0,_CP0_STATUS
		sw		s0, 116(sp)
		ori		s1,zero,MAX_SYSCALL_INTERRUPT_PRIORITY
		ins		s0,s1,10,9
		ins		s0,zero,1,4

		/* Re-enable interrupts */
		mtc0		s0,_CP0_STATUS

#if	    defined __PIC32MZ__
		rddsp		s0
		sw		s0, 156(sp)

		mfhi		s0,$ac3
		sw		s0, 152(sp)
		mflo		s0,$ac3
		sw		s0, 148(sp)

		mfhi		s0,$ac2
		sw		s0, 144(sp)
		mflo		s0,$ac2
		sw		s0, 140(sp)

		mfhi		s0,$ac1
		sw		s0, 136(sp)
		mflo		s0,$ac1
		sw		s0, 132(sp)
#endif	/*  defined __PIC32MZ__ */

		mfhi		s0,$ac0
		sw		s0, 128(sp)
		mflo		s0,$ac0
		sw		s0, 124(sp)

		lw		s0,errno
		sw		s0, 112(sp)

		/* Here the interrups above kernel priority are re-enabled */

		sw		$28,108(sp)
		sw		$27,104(sp)
		sw		$26,100(sp)
		sw		$25, 96(sp)
		sw		$24, 92(sp)
		sw		$23, 88(sp)
		sw		$22, 84(sp)
		sw		$21, 80(sp)
		sw		$20, 76(sp)
		sw		$19, 72(sp)
		sw		$18, 68(sp)	/* s2 */
		sw		$17, 64(sp)	/* s1 */
		sw		$30, 60(sp)	/* fp */
		sw		$15, 56(sp)
		sw		$14, 52(sp)
		sw		$13, 48(sp)
		sw		$12, 44(sp)
		sw		$11, 40(sp)
		sw		$10, 36(sp)
		sw		 $9, 32(sp)
		sw		 $8, 28(sp)
		sw		 $7, 24(sp)
		sw		 $6, 20(sp)
		sw		 $5, 16(sp)
		sw		 $4, 12(sp)
		sw		 $3,  8(sp)
		sw		 $2,  4(sp)
		sw		 $1,  0(sp)

		lw		s0,CurrentTask
		sw		sp,0(s0)

		jr		ra
		nop

 		.end		SaveContext
/*============================================================================*/
	 	.set		noreorder
		.set 		noat
		.global		CTInterrupt
		.global		RestoreContext

 		.ent		CTInterrupt

CTInterrupt:	addiu		sp,sp,-8
		sw		ra,4(sp)

		jal		SaveContext
		nop
		/*------------------------------------------------------------*/
		jal		Switch
		nop
		/*------------------------------------------------------------*/
RestoreContext:	lw		s0,CurrentTask
		lw		sp,0(s0)

		lw		 $1,  0(sp)
		lw		 $2,  4(sp)
		lw		 $3,  8(sp)
		lw		 $4, 12(sp)
		lw		 $5, 16(sp)
		lw		 $6, 20(sp)
		lw		 $7, 24(sp)
		lw		 $8, 28(sp)
		lw		 $9, 32(sp)
		lw		$10, 36(sp)
		lw		$11, 40(sp)
		lw		$12, 44(sp)
		lw		$13, 48(sp)
		lw		$14, 52(sp)
		lw		$15, 56(sp)
		lw		$30, 60(sp)	/* fp	*/
/*		lw		$17, 64(sp)	   s1	*/
/*		lw		$18, 68(sp)	   s2	*/
		lw		$19, 72(sp)	/* s3	*/
		lw		$20, 76(sp)
		lw		$21, 80(sp)
		lw		$22, 84(sp)
		lw		$23, 88(sp)
		lw		$24, 92(sp)
		lw		$25, 96(sp)
		lw		$26,100(sp)
		lw		$27,104(sp)
		lw		$28,108(sp)

		la		s2,errno
		lw		s1, 112(sp)
		sw		s1,0(s2)

		lw		s1, 124(sp)
		mtlo		s1,$ac0
		lw		s1, 128(sp)
		mthi		s1,$ac0

#if	    defined __PIC32MZ__
		lw		s1, 132(sp)
		mtlo		s1,$ac1
		lw		s1, 136(sp)
		mthi		s1,$ac1

		lw		s1, 140(sp)
		mtlo		s1,$ac2
		lw		s1, 144(sp)
		mthi		s1,$ac2

		lw		s1, 148(sp)
		mtlo		s1,$ac3
		lw		s1, 152(sp)
		mthi		s1,$ac3

		lw		s1, 156(sp)
		wrdsp		s1
#endif	/*  defined __PIC32MZ__ */

/* Here we must disable interrups */

#if	    defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F'

		lw		s1,FPUOwner
		beq		s0,s1,1f
		lw		s1, 116(sp)
		ins		s1,zero,29,1
1:		

#else	/*  defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' */
		lw		s1, 116(sp)
#endif	/*  defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' */

		mtc0		s1,_CP0_STATUS

		lw		s1, 120(sp)
		mtc0		s1,_CP0_EPC

		lw		s2,68(s0)
		lw		s1,64(s0)

#if	    defined __PIC32MZ__
		addiu		sp,sp,(40*4)
#else	/*  defined __PIC32MZ__ */
		addiu		sp,sp,(34*4)
#endif	/*  defined __PIC32MZ__ */

		lw		s0,0(sp)
		lw		ra,4(sp)

		addiu		sp,sp,8

		eret
		/*------------------------------------------------------------*/
		.end		CTInterrupt
/*============================================================================*/
		.extern		CurrentTask
/*============================================================================*/
		.global		ForceYield
 		.ent		ForceYield

ForceYield:	di		t0
		ori		t0,t0,3		/* Disable interrupts */
		mtc0		t0,_CP0_STATUS
		mtc0		ra,_CP0_EPC
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

#if	    defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F'

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

#endif	/*  defined __PIC32MZ__ && __PIC32_FEATURE_SET1 == 'F' */

/*============================================================================*/
