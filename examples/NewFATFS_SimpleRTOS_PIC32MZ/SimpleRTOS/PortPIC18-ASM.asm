; I found the following message in Microchip's support site. The URL is:
; http://support2.microchip.com/KBSearch/KB_Ticket.aspx?ID=Tt6UJ9A0003LM
;
; Unfortunately these pseudo-instructions don't work as inline assembly,
; so I had to make this separate .asm module.
; The functions are used inside 'port.c' and are called by 'pxPortInitialiseStack'
; (GetTMPDATALen, GetTMPDATAStrt, GetMATHDATALen, GetMATHDATAStrt) and
; by the macros 'portSAVE_CONTEXT' (SaveTMP_MATH) and 'portRESTORE_CONTEXT'
; (RestoreTMP_MATH').

;-------------------------------------------------------------------------------
; Start of the message:
;-------------------------------------------------------------------------------
;
; Issue
;
; I'm developping a RTOS for the PIC18F258 using MPLAB-C18. I need to save in the
; task context the "MATH_DATA" and ".tmpdata" sections. But their address and size
; are only known after linking. The question is: How can I know the address and
; size of both sections at run time?
;
; Thanks
;
; Solution
; You CAN get the section location and size at runtime using MPASM.
; There are a few undocumented operators and pseudo-instructions.
;
; The SCNSZ operators give you the size of the section.
; The SCNSTART operators give you the starting address of a section.
; The SCNEND operators give you the ending address of a section.
; The SCNSTART_LFSR pseudo-instruction gives you an LFSR instruction with the starting address of the section.
; The SCNEND_LFSR pseudo-instruction gives you an LFSR instruction with the ending address of the section.
;
; Example:
; CODE
; movlw SCNSZ_LOW .tmpdata
; movlw SCNSZ_HIGH .tmpdata
; movlw SCNSZ_UPPER .tmpdata
; movlw SCNSTART_LOW .tmpdata
; movlw SCNSTART_HIGH .tmpdata
; movlw SCNSTART_UPPER .tmpdata
; movlw SCNEND_LOW .tmpdata
; movlw SCNEND_HIGH .tmpdata
; movlw SCNEND_UPPER .tmpdata
;
; SCNSTART_LFSR 0, .tmpdata
; SCNEND_LFSR 0, .tmpdata
;
; END
;
; These operators and pseudo-instructions will be documented in the next revision
; of the MPASM User's Guide.
;
;-------------------------------------------------------------------------------
; End of the message
;-------------------------------------------------------------------------------

;===============================================================================
; Copyright (c) 2007-2008, Isaac Marino Bavaresco
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Neither the name of the author nor the
;       names of its contributors may be used to endorse or promote products
;       derived from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY
; EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;===============================================================================
; isaacbavaresco@yahoo.com.br
;===============================================================================
#include <P18CXXX.INC>
;===============================================================================
		radix	decimal
;===============================================================================
		code
;===============================================================================
		global	GetTMPDATAStrt

GetTMPDATAStrt:	movlw	SCNSTART_LOW .tmpdata
		movwf	PRODL
		movlw	SCNSTART_HIGH .tmpdata
		movwf	PRODH
		return	0
;===============================================================================
		global	GetTMPDATALen

GetTMPDATALen:	movlw	SCNSZ_LOW .tmpdata
		return	0
;===============================================================================
		global	GetMATHDATAStrt
GetMATHDATAStrt:movlw	SCNSTART_LOW MATH_DATA
		movwf	PRODL
		movlw	SCNSTART_HIGH MATH_DATA
		movwf	PRODH
		return	0
;===============================================================================
		global	GetMATHDATALen
GetMATHDATALen:	movlw	SCNSZ_LOW MATH_DATA
		return	0
;===============================================================================
		global	SaveTMP_MATH

SaveTMP_MATH:
		SCNSTART_LFSR 0, .tmpdata
		movlw	SCNSZ_LOW .tmpdata
SaveLoop1:	movff	POSTINC0,PREINC1
		addlw	-1
		bnz	SaveLoop1

		SCNSTART_LFSR 0, MATH_DATA
		movlw	SCNSZ_LOW MATH_DATA
SaveLoop2:	movff	POSTINC0,PREINC1
		addlw	-1
		bnz	SaveLoop2

		return	0
;===============================================================================
		global	RestoreTMP_MATH

RestoreTMP_MATH:
		SCNEND_LFSR 0, MATH_DATA
		movlw	SCNSZ_LOW MATH_DATA
RestoreLoop1:	movff	POSTDEC1,POSTDEC0
		addlw	-1
		bnz	RestoreLoop1

		SCNEND_LFSR 0, .tmpdata
		movlw	SCNSZ_LOW .tmpdata
RestoreLoop2:	movff	POSTDEC1,POSTDEC0
		addlw	-1
		bnz	RestoreLoop2

		return	0
;===============================================================================
		end
;===============================================================================
