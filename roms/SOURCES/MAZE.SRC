OPTION BINARY   ; We want a binary file, not an HP48 one.
ALIGN OFF       ; And we don't want auto alignement, as some
                ; data can be made of bytes instead of words.


; Drawing a random maze like this one consists in drawing random diagonal
; lines. There are two possibilities: right-to-left line, and left-to-right
; line. Each line is composed of a 4*4 bitmap. As the lines must form non-
; circular angles, the two bitmaps won't be '/' and '\'. The first one
; (right line) will be a little bit modified. See at the end of this source.
;
; The maze is composed of 8 lines (as the bitmaps are 4 pixels high), each
; line consists of 16 bitmaps.
; Bitmaps are drawn in random mode. We choose a random value (0 or 1).
; If it is 1, we draw a left line bitmap. If it is 0, we draw a right one.


; Rsgister usage:
;
; V0: X-coordinate of the bitmap
; V1: Y-coordinate of the bitmap
; V2: Random number

    LD  V0, 0
    LD  V1, 0

LOOP:
    LD  I,  LEFT    ; We draw a left line by default, as the random number
                    ; is 0 or 1. If we suppose that it will be 1, we keep
                    ; drawing the left line. If it is 0, we change register
                    ; I to draw a right line.

    RND V2, 1       ; Load in V2 a 0...1 random number

    SE  V2, 1       ; It is 1 ? If yes, I still refers to the left line
                    ; bitmap.

    LD  I,  RIGHT   ; If not, we change I to make it refer the right line
                    ; bitmap.

    DRW V0, V1, 4   ; And we draw the bitmap at V0, V1.

    ADD V0, 4       ; The next bitmap is 4 pixels right. So we update
                    ; V0 to do so.

    SE  V0, 64      ; If V0==64, we finished drawing a complete line, so we
                    ; skip the jump to LOOP, as we have to update V1 too.

    JP  LOOP        ; We did not draw a complete line ? So we continue !

    LD  V0, 0       ; The first bitmap of each line is located 0, V1.

    ADD V1, 4       ; We update V1. The next line is located 4 pixels doan.

    SE  V1, 32      ; Have we drawn all the lines ? If yes, V1==32.
    JP  LOOP        ; No ? So we continue !

FIN:    JP FIN      ; Infinite loop...

RIGHT:              ; 4*4 bitmap of the left line

    DB $1.......
    DB $.1......
    DB $..1.....
    DB $...1....

LEFT:               ; 4*4 bitmap of the right line
                    ; And YES, it is like that...
    DB $..1.....
    DB $.1......
    DB $1.......
    DB $...1....
