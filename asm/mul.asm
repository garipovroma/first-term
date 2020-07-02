                section         .text

                global          _start
_start:

                sub             rsp, 4 * 128 * 8
                lea             rdi, [rsp + 128 * 8]
                mov             rcx, 128
                call            read_long
                mov             rdi, rsp
                call            read_long
                lea             rdi, [rsp + 128 * 8]
                lea             rsi, [rsp]                
                lea             r9, [rsp + 2 * 128 * 8]
                call            mul_long_long
                
                mov             rdi, r9
                add             rcx, rcx       ;  2 * rcx qwords
                call            write_long

                mov             al, 0x0a
                call            write_char

                jmp             exit
                
                
; multuplies two long numbers
;    rdi --  adress of operand #1 (long number)
;    rsi -- adress of operand #2 (long number)
;    rcx - length of long number in qwords
;    r8 -- adress of inner buffer for calculations, which size = rcx qwords
; result:
;    multiply is written to r9, 2 * rcx qwords
mul_long_long:                
                shl             rcx, 3
                sub             rsp, rcx
                mov             r8, rsp
                shr             rcx, 3
                
                push            rsi
                push            rdi
                push            rcx              
                push            r8
                push            r9
;   r11 -- current offset for add
                mov             r12, rcx
                xor             r11, r11
.loop:          
                mov             rbx, [rsi]                
                call            special_mul_long_short
                
                push            rdi
                push            rsi
                
                call            special_add_long_long
;    r8 + r9 calculated                                
                pop             rsi     
                pop             rdi                
                lea             r11, [r11 + 8]
                lea             rsi, [rsi + 8]
                
                dec             r12
                jnz             .loop
                
                pop             r9
                pop             r8
                pop             rcx
                pop             rdi 
                pop             rsi       
                
                shl             rcx, 3
                add             rsp, rcx                
                shr             rcx, 3
                
                
                ret                
                
                
; adds two long number
;    r9 -- address of summand #1 (long number)
;    r8 -- address of summand #2 (long number)
;    rcx -- length of long numbers in qwords
;    r11 -- offset in bytes
;    r10 -- carry from previous special_mul_long_short operation
; result:
;    sum is written to r9
special_add_long_long:
                push            r9
                push            r8
                push            rcx
                push            r10
                push            r11

                clc
.loop:
                mov             rax, [r8]
                lea             r8, [r8 + 8]
                adc             [r9 + r11], rax
                lea             r9, [r9 + 8]
                dec             rcx
                jnz             .loop

                adc             [r9 + r11], r10
                pop             r11
                pop             r10
                pop             rcx
                pop             r8
                pop             r9
                ret


; multiplies long number by a short
;    rdi -- address of multiplier #1 (long number)
;    rbx -- multiplier #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    product is written to r8
;    carry is written to r10
special_mul_long_short:
                push            rax
                push            rdi
                push            rcx
                push            r8
                push            rsi              
                push            r9
                clc
                xor             rsi, rsi                
.loop:
                mov             rax, [rdi]
                mul             rbx
                add             rax, rsi
                adc             rdx, 0                
                mov             [r8], rax
                add             r8, 8
                add             rdi, 8
                mov             rsi, rdx
                dec             rcx                
                jnz             .loop

                mov             r10, rsi
                
                pop             r9
                pop             rsi
                pop             r8
                pop             rcx
                pop             rdi
                pop             rax
                ret
                
                
; adds 64-bit number to long number
;    rdi -- address of summand #1 (long number)
;    rax -- summand #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    sum is written to rdi
add_long_short:
                push            rdi
                push            rcx
                push            rdx

                xor             rdx,rdx
.loop:
                add             [rdi], rax
                adc             rdx, 0
                mov             rax, rdx
                xor             rdx, rdx
                add             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rdx
                pop             rcx
                pop             rdi
                ret

; adds two long number
;    rdi -- address of summand #1 (long number)
;    rsi -- address of summand #2 (long number)
;    rcx -- length of long numbers in qwords
; result:
;    sum is written to rdi
add_long_long:
                push            rdi
                push            rsi
                push            rcx

                clc
.loop:
                mov             rax, [rsi]
                lea             rsi, [rsi + 8]
                adc             [rdi], rax
                lea             rdi, [rdi + 8]
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rsi
                pop             rdi
                ret
                
                
; multiplies long number by a short
;    rdi -- address of multiplier #1 (long number)
;    rbx -- multiplier #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    product is written to rdi
mul_long_short:
                push            rax
                push            rdi
                push            rcx

                xor             rsi, rsi
.loop:
                mov             rax, [rdi]
                mul             rbx
                add             rax, rsi
                adc             rdx, 0
                mov             [rdi], rax
                add             rdi, 8
                mov             rsi, rdx
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rdi
                pop             rax
                ret
                
; divides long number by a short
;    rdi -- address of dividend (long number)
;    rbx -- divisor (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    quotient is written to rdi
;    rdx -- remainder
div_long_short:
                push            rdi
                push            rax
                push            rcx

                lea             rdi, [rdi + 8 * rcx - 8]
                xor             rdx, rdx

.loop:
                mov             rax, [rdi]
                div             rbx
                mov             [rdi], rax
                sub             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rax
                pop             rdi
                ret

; assigns a zero to long number
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
set_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep stosq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; checks if a long number is a zero
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
; result:
;    ZF=1 if zero
is_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep scasq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; read long number from stdin
;    rdi -- location for output (long number)
;    rcx -- length of long number in qwords
read_long   :
                push            rcx
                push            rdi

                call            set_zero
.loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              .done
                cmp             rax, '0'
                jb              .invalid_char
                cmp             rax, '9'
                ja              .invalid_char

                sub             rax, '0'
                mov             rbx, 10
                call            mul_long_short
                call            add_long_short
                jmp             .loop

.done:
                pop             rdi
                pop             rcx
                ret

.invalid_char:
                mov             rsi, invalid_char_msg
                mov             rdx, invalid_char_msg_size
                call            print_string
                call            write_char
                mov             al, 0x0a
                call            write_char

.skip_loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              exit
                jmp             .skip_loop

; write long number to stdout
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
write_long:
                push            rax
                push            rcx

                mov             rax, 20
                mul             rcx
                mov             rbp, rsp
                sub             rsp, rax

                mov             rsi, rbp

.loop:
                mov             rbx, 10
                call            div_long_short
                add             rdx, '0'
                dec             rsi
                mov             [rsi], dl
                call            is_zero
                jnz             .loop

                mov             rdx, rbp
                sub             rdx, rsi
                call            print_string

                mov             rsp, rbp
                pop             rcx
                pop             rax
                ret

; read one char from stdin
; result:
;    rax == -1 if error occurs
;    rax \in [0; 255] if OK
read_char:
                push            rcx
                push            rdi

                sub             rsp, 1
                xor             rax, rax
                xor             rdi, rdi
                mov             rsi, rsp
                mov             rdx, 1
                syscall

                cmp             rax, 1
                jne             .error
                xor             rax, rax
                mov             al, [rsp]
                add             rsp, 1

                pop             rdi
                pop             rcx
                ret
.error:
                mov             rax, -1
                add             rsp, 1
                pop             rdi
                pop             rcx
                ret

; write one char to stdout, errors are ignored
;    al -- char
write_char:
                sub             rsp, 1
                mov             [rsp], al

                mov             rax, 1
                mov             rdi, 1
                mov             rsi, rsp
                mov             rdx, 1
                syscall
                add             rsp, 1
                ret

exit:
                mov             rax, 60
                xor             rdi, rdi
                syscall

; print string to stdout
;    rsi -- string
;    rdx -- size
print_string:
                push            rax

                mov             rax, 1
                mov             rdi, 1
                syscall

                pop             rax
                ret


                section         .rodata
invalid_char_msg:
                db              "Invalid character: "
invalid_char_msg_size: equ             $ - invalid_char_msg
