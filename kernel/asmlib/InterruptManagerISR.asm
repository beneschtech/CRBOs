;; This may look a little odd, but these are wrappers for
;; interrupt handlers.  They dont use the standard C++
;; idioms of mov esp,ebp .. etc.. And need iret and pusha and
;; all that as wrappers.  We will do our best to macro-ize them as
;; much as possible

[BITS 64]

SECTION .data
ALIGN 16
_fxsave:
TIMES 512 DB 0

SECTION .text
;; Exception Handlers
global _ZN16InterruptManager7nullISREv:function
global _ZN16InterruptManager7ISRisr0Ev:function
global _ZN16InterruptManager7ISRisr1Ev:function
global _ZN16InterruptManager7ISRisr2Ev:function
global _ZN16InterruptManager7ISRisr3Ev:function
global _ZN16InterruptManager7ISRisr4Ev:function
global _ZN16InterruptManager7ISRisr5Ev:function
global _ZN16InterruptManager7ISRisr6Ev:function
global _ZN16InterruptManager7ISRisr7Ev:function
global _ZN16InterruptManager7ISRisr8Ev:function
global _ZN16InterruptManager7ISRisr9Ev:function
global _ZN16InterruptManager8ISRisr10Ev:function
global _ZN16InterruptManager8ISRisr11Ev:function
global _ZN16InterruptManager8ISRisr12Ev:function
global _ZN16InterruptManager8ISRisr13Ev:function
global _ZN16InterruptManager8ISRisr14Ev:function
global _ZN16InterruptManager8ISRisr16Ev:function
global _ZN16InterruptManager8ISRisr17Ev:function
global _ZN16InterruptManager8ISRisr18Ev:function
global _ZN16InterruptManager8ISRisr19Ev:function
global _ZN16InterruptManager8ISRisr20Ev:function
global _ZN16InterruptManager8ISRisr30Ev:function

; IRQ Handlers
global _Z10_irqEntry0v:function
global _Z10_irqEntry1v:function

extern _ZN16InterruptManager8irqChainEy
extern _ZN16InterruptManager4isr0Ev
extern _ZN16InterruptManager4isr1Ev
extern _ZN16InterruptManager4isr2Ev
extern _ZN16InterruptManager4isr3Ev
extern _ZN16InterruptManager4isr4Ev
extern _ZN16InterruptManager4isr5Ev
extern _ZN16InterruptManager4isr6Ev
extern _ZN16InterruptManager4isr7Ev
extern _ZN16InterruptManager4isr8Ev
extern _ZN16InterruptManager4isr9Ev
extern _ZN16InterruptManager5isr10Ev
extern _ZN16InterruptManager5isr11Ev
extern _ZN16InterruptManager5isr12Ev
extern _ZN16InterruptManager5isr13Ev
extern _ZN16InterruptManager5isr14Ev
extern _ZN16InterruptManager5isr16Ev
extern _ZN16InterruptManager5isr17Ev
extern _ZN16InterruptManager5isr18Ev
extern _ZN16InterruptManager5isr19Ev
extern _ZN16InterruptManager5isr20Ev
extern _ZN16InterruptManager5isr30Ev

%macro pushall64 0
PUSHFQ
PUSH RAX
PUSH RBX
PUSH RCX
PUSH RDX
PUSH RBP
PUSH RSI
PUSH RDI
PUSH R8
PUSH R9
PUSH R10
PUSH R11
PUSH R12
PUSH R13
PUSH R14
PUSH R15
FXSAVE [_fxsave]
%endmacro

%macro popall64 0
FXRSTOR [_fxsave]
POP R15
POP R14
POP R13
POP R12
POP R11
POP R10
POP R9
POP R8
POP RDI
POP RSI
POP RBP
POP RDX
POP RCX
POP RBX
POP RAX
POPFQ
%endmacro

;; Just does a quick return
align 16
_ZN16InterruptManager7nullISREv:
   IRETQ

;; Divide by zero actual ISR handler, just calls isr0, but returns the proper way
align 16
_ZN16InterruptManager7ISRisr0Ev:
   CALL _ZN16InterruptManager4isr0Ev
   IRETQ

align 16
_ZN16InterruptManager7ISRisr1Ev:
   CALL _ZN16InterruptManager4isr1Ev
   IRETQ

align 16
_ZN16InterruptManager7ISRisr2Ev:
   CALL _ZN16InterruptManager4isr2Ev
   IRETQ

align 16
_ZN16InterruptManager7ISRisr3Ev:
   CALL _ZN16InterruptManager4isr3Ev
   IRETQ

align 16
_ZN16InterruptManager7ISRisr4Ev:
   CALL _ZN16InterruptManager4isr4Ev
   IRETQ

align 16
_ZN16InterruptManager7ISRisr5Ev:
   CALL _ZN16InterruptManager4isr5Ev
   IRETQ

align 16
_ZN16InterruptManager7ISRisr6Ev:
   CALL _ZN16InterruptManager4isr6Ev
   IRETQ

align 16
_ZN16InterruptManager7ISRisr7Ev:
   CALL _ZN16InterruptManager4isr7Ev
   IRETQ

align 16
_ZN16InterruptManager7ISRisr8Ev:
   CALL _ZN16InterruptManager4isr8Ev
   IRETQ

align 16
_ZN16InterruptManager7ISRisr9Ev:
   CALL _ZN16InterruptManager4isr9Ev
   IRETQ

align 16
_ZN16InterruptManager8ISRisr10Ev:
   POP R15
   CALL _ZN16InterruptManager5isr10Ev
   IRETQ

align 16
_ZN16InterruptManager8ISRisr11Ev:
   POP R15
   CALL _ZN16InterruptManager5isr11Ev
   IRETQ

align 16
_ZN16InterruptManager8ISRisr12Ev:
   POP R15
   CALL _ZN16InterruptManager5isr12Ev
   IRETQ

align 16
_ZN16InterruptManager8ISRisr13Ev:
   POP R15
   CALL _ZN16InterruptManager5isr13Ev
   IRETQ

align 16
_ZN16InterruptManager8ISRisr14Ev:
   POP R15
   CALL _ZN16InterruptManager5isr14Ev
   IRETQ

align 16
_ZN16InterruptManager8ISRisr16Ev:
   CALL _ZN16InterruptManager5isr16Ev
   IRETQ

align 16
_ZN16InterruptManager8ISRisr17Ev:
   POP R15
   CALL _ZN16InterruptManager5isr17Ev
   IRETQ

align 16
_ZN16InterruptManager8ISRisr18Ev:
   CALL _ZN16InterruptManager5isr18Ev
   IRETQ

align 16
_ZN16InterruptManager8ISRisr19Ev:
   CALL _ZN16InterruptManager5isr19Ev
   IRETQ

align 16
_ZN16InterruptManager8ISRisr20Ev:
   CALL _ZN16InterruptManager5isr20Ev
   IRETQ

align 16
_ZN16InterruptManager8ISRisr30Ev:
   POP R15
   CALL _ZN16InterruptManager5isr30Ev
   IRETQ

align 16
_Z10_irqEntry0v:
   pushall64
   XOR RDI,RDI
   CALL _ZN16InterruptManager8irqChainEy
   popall64
   IRETQ

align 16
_Z10_irqEntry1v:
   pushall64
   MOV RDI,1
   CALL _ZN16InterruptManager8irqChainEy
   popall64
   IRETQ
