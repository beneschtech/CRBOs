;; This is the bootsector code
;;
; Its main purpose is to load the loader code at 0x6000 and jump to it
;;
; For the purposes of the floppy image, this is the next few sectors of the disk

[BITS 16]
[ORG 0x7c00]

XOR AX,AX
MOV DS,AX
MOV ES,AX
MOV FS,AX
MOV GS,AX
MOV SP,0x5ffe

JMP 0:bsstart
bsstart:
MOV [bootDrive],DL
;; Reset floppy
XOR AX,AX
XOR DX,DX
INT 0x13

MOV SI,loadingStr
CALL printString

;; Int 13 to read the next few sectors of the floppy
MOV AX,(0x200 + loaderNumSects)
MOV CX,2
XOR DX,DX
MOV BX,0x6000
INT 0x13
JC diskReadError

MOV DL,[bootDrive]
CALL 0x6000

CLI
HLT

;; Functions
diskReadError:
MOV SI,readErrStr
CALL printString
CLI
HLT

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

;; Data section
readErrStr:
db "Disk read error",13,10,0

loadingStr:
db "Loading CRBOs ",0

bootDrive db 0

TIMES 510-($-$$) DB 0
DW 0xaa55
