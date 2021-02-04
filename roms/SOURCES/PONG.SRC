; Note: this source has been modified by David WINTER on 17 SEP 1997
;       (only the syntax changed: it has been converted in CHIPPER)
;
; The source could be optimized to save some bytes, but I didn't wanted
; to modify it since there is no specific interest in this.
;
; NOTE THAT THE ORIGINAL SOURCE HAD SEVERAL ERRORS !!!
;
; ---------------------------------------------------------------------------
;
; From: vervalin@AUSTIN.LOCKHEED.COM (Paul Vervalin)
;
;
;        OK folks here it is!  PONG for the HP48SX written in CHIP-48.
;        Some things you should know before you start playing are...
;        1)  This is my first attempt at programming in CHIP-48, so I 
;            know there are probably a few things that could have been
;            done better.
;        2)  The game never ends.  It keeps score, but only up to 9 for
;            each player, then it will roll over to 0.  Sorry, its the
;            only way I could think of to do it.  So, you have to play
;            whoever gets to a number first, wins.
;        3)  It is kind of slow, but then there are two paddles and ball
;            moving around all at once.
;        4)  The player who got the last point gets the serve...
;        5)  Keys 7 and 4 (on the HP48) control the up and down of the 
;            left player and the / and * keys control the up and down of
;            the right player.
;
;        I think that's about it, so have fun!  
; 
; This is a detailed breakdown of the game, sooooooo, if anybody wants to
; make it better, or change it in some way, it might be a little easier.
; Also, about half of the code was in some way extracted from the BRIX
; program.  So, thanks to whoever wrote the original BRIX game.
;
;
; Registers
; ---------
; V0-V3  are scratch registers
; V4     X coord. of score
; V5     Y coord. of score
; V6     X coord. of ball
; V7     Y coord. of ball
; V8     X direction of ball motion
; V9     Y direction of ball motion
; VA     X coord. of left player paddle
; VB     Y coord. of left player paddle
; VC     X coord. of right player paddle
; VD     Y coord. of right player paddle
; VE     Score
; VF     collision detection


option binary  ; To assemble PONG for HP48 use, remove this option


    LD  VA, 2  ; Set left player X coord.
    LD  VB, 12 ; Set left player Y coord.
    LD  VC, 63 ; Set right player X coord.
    LD  VD, 12 ; Set right player Y coord.

    LD  I,  Paddle ; Get address of paddle sprite
    DRW VA, VB, 6  ; Draw left paddle
    DRW VC, VD, 6  ; Draw right paddle

    LD  VE, 0       ; Set score to 00
    CALL Draw_Score ; Draw score

    LD  V6, 3 ; Set X coord. of ball to 3
    LD  V8, 2 ; Set ball X direction to right


Big_Loop:

    LD  V0, #60 ; Set V0=delay before ball launch
    LD  DT, V0  ; Set delay timer to V0
DT_loop:        ;
    LD  V0, DT  ; Read delay timer into V0
    SE  V0, 0   ; Skip next instruction if V0=0
    JP  DT_Loop ; Read again delay timer if not 0

    RND V7, 23  ; Set Y coord. to rand # AND 23 (0...23)
    ADD V7, 8   ; And adjust it to is 8...31

    LD  V9, #FF   ; Set ball Y direction to up
    LD  I, Ball   ; Get address of ball sprite
    DRW V6, V7, 1 ; Draw ball

Padl_Loop:
    LD  I, Paddle ; Get address of paddle sprite
    DRW VA, VB, 6 ; Draw left paddle
    DRW VC, VD, 6 ; Draw right paddle

    LD  V0, 1   ; Set V0 to KEY 1
    SKNP V0     ; Skip next instruction if KEY in 1 is not pressed
    ADD VB, #FE ; Subtract 2 from Y coord. of left paddle

    LD  V0, 4   ; Set V0 to KEY 4
    SKNP V0     ; Skip next instruction if KEY in 4 is not pressed
    ADD VB, 2   ; Add 2 to Y coord. of left paddle

    LD  V0, 31    ; Set V0 to max Y coord.  | These three lines are here to
    AND VB, V0    ; AND VB with V0          | adjust the paddle position if
    DRW VA, VB, 6 ; Draw left paddle        | it is out of the screen

    LD  V0, #0C ; Set V0 to KEY C
    SKNP V0     ; Skip next instruction if KEY in C is not pressed
    ADD VD, #FE ; Subtract 2 from Y coord. of right paddle

    LD  V0, #0D ; Set V0 to KEY D
    SKNP V0     ; Skip next instruction if KEY in D is not pressed 
    ADD VD, 2   ; Add 2 to Y coord. of right paddle

    LD  V0, 31    ; Set V0 to max Y coord.  | These three lines are here to
    AND VD, V0    ; AND VD with V0          | adjust the paddle position if
    DRW VC, VD, 6 ; Draw right paddle       | it is out of the screen

    LD  I, Ball   ; Get address of ball sprite
    DRW V6, V7, 1 ; Draw ball

    ADD V6, V8  ; Compute next X coord of the ball
    ADD V7, V9  ; Compute next Y coord of the ball

    LD  V0, 63  ; Set V0 to max X location
    AND V6, V0  ; AND V6 with V0

    LD  V1, 31  ; Set V1 to max Y location
    AND V7, V1  ; AND V7 with V1

    SNE V6, 2      ; Skip next instruction if ball not at left
    JP  Left_Side  ;

    SNE V6, 63     ; Skip next instruction if ball not at right
    JP  Right_Side ; 

Ball_Loop:
    SNE V7, 31  ; Skip next instruction if ball not at bottom
    LD  V9, #FF ; Set Y direction to up

    SNE V7, 0   ; Skip next instruction if ball not at top
    LD  V9, 1   ; Set Y direction to down

    DRW V6, V7, 1 ; Draw ball
    JP  Padl_loop ;

Left_Side:
    LD  V8, 2    ; Set X direction to right
    LD  V3, 1    ; Set V3 to 1 in case left player misses ball
    LD  V0, V7   ; Set V0 to V7 Y coord. of ball
    SUB V0, VB   ; Subtract position of paddle from ball
    JP  Pad_Coll ; Check for collision

Right_Side:
    LD  V8, 254  ; Set X direction to left
    LD  V3, 10   ; Set V3 to 10 in case right player misses ball
    LD  V0, V7   ; Set V0 to V7 Y coord. of ball
    SUB V0, VD   ; Subtract position of paddle from ball

Pad_Coll:
    SE  VF, 1     ; Skip next instruction if ball not above paddle
    JP  Ball_Lost ;

    LD  V1, 2    ; Set V1 to 02
    SUB V0, V1   ; Subtract V1 from V0
    SE  VF, 1    ; Skip next instr. if ball not at top of paddle
    JP  Ball_Top ; Ball at top of paddle

    SUB V0, V1   ; Subtract another 2 from V0
    SE  VF, 1    ; Skip next instr. if ball not at middle of paddle
    JP  Pad_Hit  ; Ball in middle of paddle

    SUB V0, V1   ; Subtract another 2 from V0
    SE  VF, 1    ; Skip next instr. if ball not at bottom of paddle
    JP  Ball_Bot ; Ball at bottom of paddle

Ball_Lost:
    LD  V0, 32  ; Set lost ball beep delay
    LD  ST, V0  ; Beep for lost ball

    CALL Draw_Score ; Erase previous score
    ADD VE, V3      ; Add 1 or 10 to score depending on V3
    CALL Draw_Score ; Write new score

    LD  V6, 62  ; Set ball X coord. to right side
    SE  V3, 1   ; Skip next instr. if right player got point
    LD  V6, 3   ; Set ball X coord. to left side
    LD  V8, #FE ; Set direction to left
    SE  V3, 1   ; Skip next instr. if right player got point
    LD  V8, 2   ; Set direction to right
    JP Big_Loop ;

Ball_Top:
    ADD V9, #FF ; Subtract 1 from V9, ball Y direction
    SNE V9, #FE ; Skip next instr. if V9 != FE (-2)
    LD  V9, #FF ; Set V9=FF (-1)
    JP  Pad_Hit

Ball_Bot:
    ADD V9, 1   ; Add 1 to V9, ball Y direction
    SNE V9, 2   ; Skip next instr. if V9 != 02
    LD  V9, 1   ; Set V9=01

Pad_Hit:
    LD  V0, 4   ; Set beep for paddle hit
    LD  ST, V0  ; Sound beep

    ADD V6, 1   ;
    SNE V6, 64  ;
    ADD V6, 254 ;

    JP  Ball_Loop

Draw_Score:
    LD  I,  Score   ; Get address of Score
    LD  B,  VE      ; Stores in memory BCD representation of VE
    LD  V2, [I]     ; Reads V0...V2 in memory, so the score
    LD  F,  V1      ; I points to hex char in V1, so the 1st score char
    LD  V4, #14     ; Set V4 to the X coord. to draw 1st score char
    LD  V5, 0       ; Set V5 to the Y coord. to draw 1st score char
    DRW V4, V5, 5   ; Draw 8*5 sprite at (V4,V5) from M[I], so char V1
    ADD V4, #15     ; Set X to the X coord. of 2nd score char
    LD  F, V2       ; I points to hex char in V2, so 2nd score char
    DRW V4, V5, 5   ; Draw 8*5 sprite at (V4,V5) from M[I], so char V2
    RET             ; Return

Paddle:
    DW #8080
    DW #8080
    DW #8080

Ball:
    DW #8000

Score:
    DW #0000
    DW #0000
