;; CRBOs Top Level floppy image wrapper

%include "loaderconstants.inc"
%include "bootsect.asm"
incbin "loader.bin"
incbin "../kernel/kernel"
TIMES (2880 * 512)-($-$$) DB 0
