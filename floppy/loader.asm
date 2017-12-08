%include "loaderconstants.inc"
[ORG 0x6000]
[BITS 16]

;; Store boot drive
MOV [bootDrive],DL

;; Before we get too far, lets store our environment so we can get back to it in x86int mode at 0x5000
; Zero out some of these structures
MOV DI,0x5000
MOV CX,256
XOR  AX,AX
REP STOSW

; RM IDT
SIDT [0x5000]
; RM GDT
SGDT [0x5010]

; 8259 masks
MOV DX,0xA1
IN AL,DX
XCHG AL,AH
MOV DX,0x21
IN AL,DX
MOV WORD [0x5020],AX

;; OS Gdt to return to
MOV AX,GDTR
MOV WORD [0x5030],AX

;; Read first sector of ELF image and get needed data from it
XOR EAX,EAX
MOV AX,kernelLBAAddr
CALL ReadSector

;; First lets make sure its actually an ELF
MOV EAX,0x464c457f
CMP DWORD EAX,[0x7000]
JNZ badELF
; Make sure its 64 bit little endian
MOV AX,0x0102
CMP WORD AX,[0x7004]
JNZ badELF

;; Enter unreal mode, must be done before using copyData function
; Lets assume we have a computer built after 1997
MOV AX,0x2401
INT 0x15
CLI
LGDT [GDTR]
MOV EAX,CR0
INC EAX
MOV CR0,EAX
MOV BX,0x20
MOV FS,BX
DEC EAX
MOV CR0,EAX
STI

;; Now we should be in 16 bit REAL mode with access to the first 4G of RAM through FS
MOV BX,[0x703C]
MOV EAX,[0x7018]
MOV [krnlEntry],EAX
;; GET SHT Address
;; We use the Section Header instead of the PHT, so that we can have an extra section in a seperate location, ie an
;; x86 Real mode interrupt handler at 0x5000 while the kernel itself resides at 1MB
MOV DWORD ESI,[0x7028]
; Get sizeof(SHT)
MOV AX,[0x703A] ; Size of entry
MUL BX ; num entries


XOR ECX,ECX
MOV CX,AX
MOV EDI,0x8000

;; Copy SHT to 0x8000 using our data read functions
CALL copyData

;; Now that we have our sections in memory, lets go through them one by one and load them
XCHG BX,CX
MOV BX,0x8010
elfLoop:
PUSH CX
MOV EDI,[BX] ;; Destination address
ADD BX,8
MOV ESI,[BX] ;; Offset into file
ADD BX,8
MOV ECX,[BX] ;; Size

;; None of these can be zero
CMP EDI,DWORD 0
JZ .elSkipSection
CMP ESI,DWORD 0
JZ .elSkipSection
CMP ECX,DWORD 0
JZ .elSkipSection
CMP DWORD [stackStart],0
JNZ .itsLoaded
   MOV DWORD [stackStart],EDI
.itsLoaded:
CALL copyData
CMP DWORD [mallocStart],EDI
JNC .elSkipSection
MOV DWORD [mallocStart],EDI
.elSkipSection:
ADD BX,0x30
POP CX
LOOP elfLoop


;; Create page tables, assume 2MB pages are okay
;; Identity map the first 16MB We can set the rest up inside the kernel, for now
;; we know that we have at LEAST 16M
MOV EDI,0x10000
MOV DWORD [FS:EDI],0x11003
ADD EDI,0x1000
MOV DWORD [FS:EDI],0x12003
ADD EDI,0x1000
MOV DWORD [FS:EDI],0x000083
ADD EDI,0x8
MOV DWORD [FS:EDI],0x200083
ADD EDI,0x8
MOV DWORD [FS:EDI],0x400083;
ADD EDI,0x8
MOV DWORD [FS:EDI],0x600083;
ADD EDI,0x8
MOV DWORD [FS:EDI],0x800083;
ADD EDI,0x8
MOV DWORD [FS:EDI],0xa00083;
ADD EDI,0x8
MOV DWORD [FS:EDI],0xc00083;
ADD EDI,0x8
MOV DWORD [FS:EDI],0xe00083;


;; Get int 15 memory map and store the pmode idt in preperation for bios calls from the kernel


; use the INT 0x15, eax= 0xE820 BIOS function to get a memory map
; inputs: es:di -> destination buffer for 24 byte entries
; outputs: bp = entry count, trashes all registers except esi
MOV DI,0x7012
xor ebx, ebx            ; ebx must be 0 to start
xor bp, bp              ; keep an entry count in bp
mov edx, 0x0534D4150    ; Place "SMAP" into edx
mov eax, 0xe820
mov [es:di + 20], dword 1       ; force a valid ACPI 3.X entry
mov ecx, 24             ; ask for 24 bytes
int 0x15
jc short .failed        ; carry set on first call means "unsupported function"
mov edx, 0x0534D4150    ; Some BIOSes apparently trash this register?
cmp eax, edx            ; on success, eax must have been reset to "SMAP"
jne short .failed
test ebx, ebx           ; ebx = 0 implies list is only 1 entry long (worthless)
je short .failed
jmp short .jmpin
.e820lp:
mov eax, 0xe820         ; eax, ecx get trashed on every int 0x15 call
mov [es:di + 20], dword 1       ; force a valid ACPI 3.X entry
mov ecx, 24             ; ask for 24 bytes again
int 0x15
jc short .e820f         ; carry set means "end of list already reached"
mov edx, 0x0534D4150    ; repair potentially trashed register
.jmpin:
jcxz .skipent           ; skip any 0 length entries
cmp cl, 20              ; got a 24 byte ACPI 3.X response?
jbe short .notext
test byte [es:di + 20], 1       ; if so: is the "ignore this data" bit clear?
je short .skipent
.notext:
mov ecx, [es:di + 8]    ; get lower dword of memory region length
or ecx, [es:di + 12]    ; "or" it with upper dword to test for zero
jz .skipent             ; if length qword is 0, skip entry
inc bp                  ; got a good entry: ++count, move to next storage spot
add di, 24
.skipent:
test ebx, ebx           ; if ebx resets to 0, list is complete
jne short .e820lp
.e820f:
mov [0x7010], bp        ; store the entry count
clc                     ; there is "jc" on end of list to this point, so the carry must be cleared
JMP LetsGo
.failed:
stc                     ; "function unsupported" error exit
JMP LetsGo

LetsGo:
;; Lets jump from 16 bit to 32 to 64 then to the kernel
CLI ;; Goodbye interrupts until we are in C++ code
MOV EAX,CR0
INC EAX
MOV CR0,EAX
JMP 0x18:mode32
mode32:
[BITS 32]
MOV AX,0x20
MOV DS,AX
MOV DX,0x3F2 ;; Turn the floppy motor off, its annoying!
MOV AL,0xC
OUT DX,AL
;; Set PAE and PGE bit
MOV EAX, 10100000b
MOV CR4,EAX
MOV EDI,0x10000
MOV CR3,EDI
MOV ECX, 0xC0000080               ; Read from the EFER MSR.
RDMSR
OR EAX, 0x00000500                ; Set the LME bit.
WRMSR

MOV EBX,CR0                      ; Activate long mode -
OR EBX,0x80000001                 ; - by enabling paging and protection simultaneously.
MOV CR0,EBX

;; Now lets set up and activate all of that fancy math coprocessor support
;; SSE Instructions

MOV EAX,CR0
AND AX,0xfffb
OR AX,2
MOV CR0,EAX
MOV EAX,CR4
OR AX,3 << 9
MOV CR4,EAX

JMP 0x28: longMode
longMode:
[BITS 64]
MOV AX,0x30
MOV DS,AX
MOV ES,AX
MOV FS,AX
MOV GS,AX
MOV SS,AX
XOR RSP,RSP
MOV ESP,[stackStart]
MOV QWORD RAX,[krnlEntry]
XOR RDI,RDI
MOV EDI,[mallocStart]
MOV RBP,RSP
CALL RAX

CLI
HLT

[BITS 16]
RET

;; Functions

;; Copies data from ESI bytes into the file to address EDI of size ECX bytes
;; Dynamically loads sectors as needed
copyData:
PUSH EBX
PUSH ESI
PUSH EAX
PUSH EDX
PUSH ECX

;; First get starting sector
XOR EAX,EAX
XOR EDX,EDX
MOV EAX,ESI
MOV EBX,512
DIV EBX
ADD EAX,kernelLBAAddr
CALL ReadSector

;; Copy from first sector
MOV ECX,0x200
SUB ECX,EDX  ;; ecx has rest of sector count
POP EBX      ;; actual requested bytes in ebx
CMP EBX,ECX  ;; Is it less?  Can it all really fit in one sector?
JC .onlyOneNeeded ;; Yup
SUB EBX,ECX
PUSH EBX
JMP .doCopy
.onlyOneNeeded:
XCHG EBX,ECX
PUSH DWORD 0
.doCopy:
MOV ESI,EDX
ADD ESI,0x7000
CALL copyBytes

;; Ok, how much is left?
.cdSectorLoop:
POP ECX
CMP ECX,0
JZ .cdDone ;; No more data?
CMP ECX,0x200
JC .cdLastSector ;; Less than one sector of data left

;; Read a whole sector and transfer up to destination
SUB ECX,0x200
PUSH ECX
INC EAX
CALL ReadSector
MOV ECX,0x200
MOV ESI,0x7000
CALL copyBytes
JMP .cdSectorLoop

.cdLastSector:
INC EAX
CALL ReadSector
MOV ESI,0x7000
CALL copyBytes

.cdDone:
POP EDX
POP EAX
POP ESI
POP EBX
RET

;; Copies bytes from esi to edi
;; We have to do this this way since 16 bit rep movsb will only do 64k of ram, this can access the first 4G
copyBytes:
PUSH AX
.cbLoop:
MOV AL,[FS:ESI]
MOV [FS:EDI],AL
INC ESI
INC EDI
LOOP .cbLoop
POP AX
RET

;; Read a sector with the LBA address in EAX into 0x7000
ReadSector:
PUSHAD
MOV [currSector],EAX
CALL incrementSpinner
MOV DL,[bootDrive]
CMP DL,0x80
JNC .readHDD
; We dont need dword support for a floppy
CALL LBAtoCHS
MOV DL,[bootDrive]
MOV AX,0x201
MOV BX,0x7000
INT 0x13
JC readError
POPAD
RET
.readHDD:
MOV DWORD [HDDReadPacket.sector],EAX
MOV AX,0x4200
MOV SI,HDDReadPacket
INT 0x13
JC readError
POPAD
RET


;; Converts LBA to CHS address for a 1.44 floppy
LBAtoCHS:
;[in AX=LBA Sector]
;[out DX,CX]
XOR CX,CX
XOR DX,DX
DIV WORD [flpSecTrk]
INC DX
MOV CL,DL
XOR DX,DX
DIV WORD [flpHds]
MOV DH,DL
MOV CH,AL
RET

;; Incrememnts the spinner so that the user can see something is happening
incrementSpinner:
PUSH SI
PUSH CX
MOV SI,txtSpinner
XOR CX,CX
MOV CL,[txtSpPos]
INC CL
.incrementSpinner1:
ADD SI,3
LOOP .incrementSpinner1
MOV CL,[txtSpPos]
CALL printString
INC CL
CMP CL,4
JLE .incrementSpinnerOut
MOV CL,0
.incrementSpinnerOut:
MOV [txtSpPos],CL
POP CX
POP SI
RET

printString:
PUSH AX
PUSH BX
PUSH CX
MOV AH,0xe
XOR BX,BX
XOR CX,CX
.printStringLoop:
LODSB
TEST AL,AL
JZ .printStringExit
INT 0x10
JMP .printStringLoop
.printStringExit:
POP CX
POP BX
POP AX
RET

;; Error functions
readError:
MOV SI,readErrorStr
CALL printString
CLI
HLT
readErrorStr db 13,10,13,10,"Disk Read error",0

badELF:
MOV SI,badELFStr
CALL printString
CLI
HLT
badELFStr db 13,10,13,10,"Corrupted ELF Image!",0

;; Data
txtSpinner db 0,0,0,"/",8,0,"-",8,0,"\",8,0,"|",8,0,".",0
txtSpPos db 0
bootDrive db 0
currSector dd 0
flpSecTrk dw 18
flpHds dw 2
krnlEntry dq 0
mallocStart dd 0
stackStart dd 0
HDDReadPacket:
;; Some of these values are static
db 0x10
db 0
dw 1
dw 0x7000
dw 0
.sector dq 0

ALIGN 8
GDT:
dq 0
;; 16 Bit
    dd 0x0000ffff   ;; Code 0x8
    dd 0x00009c00
    dd 0x0000ffff   ;; Data 0x10
    dd 0x00009200
;; 32 Bit Segments
    dd 0x0000ffff   ;; Code 0x18
    dd 0x00cf9c00
    dd 0x0000ffff   ;; Data 0x20
    dd 0x00cf9200
;; 64 bit
    dq 0x002f98000000ffff ; Code 0x28
    dq 0x002f92000000ffff ; Data 0x30

GDTR:
dw (GDTR-GDT)-1
dd GDT

TIMES (512 * (loaderNumSects))-($-$$) DB 90
