;;;;
.equ PORTD, 0x0B  ; Direccion de PORT D
.equ DDRD, 0x0A   ; Direccion de DDR D
.equ PIND, 0x09   ; Direccion de PIN D
.equ PORTB, 0x05  ; Direccion de PORT B
.equ DDRB, 0x04   ; Direccion de DDR B
.equ PINB, 0x03   ; Direccion de PIN B
;
;USART0
.equ UBRR0H, 0xC5
.equ UBRR0L, 0xC4
.equ UCSR0B, 0xC1
.equ UCSR0A, 0xC0
.equ UDR0, 0xC6
;
;;;;

.section .text
.global _config_ports
.global _config_usart0
.global _enviar_caracter
.global _recibir_caracter
.global _verificar_status_control

_config_ports:
    ; Configurar Pines Puerto D como entrada
    ldi r16, 0x00
    out DDRD, r16
    ; Desactivar Pull-up Puerto D
    ldi r16, 0x00
    out PORTD, r16
    ; Configurar Pin Puerto B como Salida
    sbi DDRB, 0x00
    ; Limpiar Pin Puerto B
    cbi PORTB, 0x00
    RET

; Configurar USART0
; 8 bits de datos, sin paridad, 1 bit de parada
; Baud = Fosc / (16 * (UBRR + 1)) = 9600
; Fosc = 16 MHz
; UBRR = 103
_config_usart0:
    ;; Configurar baud rate
    ldi r16, 0x00
    sts UBRR0H, r16
    ldi r16, 0x67
    sts UBRR0L, r16
    ; Configurar formato de trama
    ; Se utilizarn los valores por defecto
    ; Configurar interrupciones
    ;; Habilitar USART0
    ldi r16, 0x18
    sts UCSR0B, r16
    RET

; Enviar caracter
; R18 = caracter a enviar
_enviar_caracter:
    ;Verificar que el buffer de transmision este vacio
    lds r16, UCSR0A
    sbrs r16, 0x05
    rjmp _fin_enviar_caracter
    ; Limpiar TXC0
    lds r16, UCSR0A
    ldi r17, 0xBF
    and r16, r17
    sts UCSR0A, r16
    ; Enviar caracter
    //ldi r18, 0x41
    sts UDR0, r18
_fin_enviar_caracter:
    RET

; Recibir caracter
; R18 = caracter recibido
_recibir_caracter:
    ;Verificar si se completo la recepcion
    lds r16, UCSR0A
    sbrs r16, 0x07
    rjmp _fin_recibir_caracter
    ; Leer caracter
    lds r18, UDR0
    ; Prender Vibrador en el pin del Puerto B por 5 segundos
    sbi PORTB, 0x00
    ;Delay 5 segundos
    ldi r19, 0x64 ;100 = 0x64
    ldi r20, 0x33 ;51 = 0x33
    ldi r21, 0x37 ;55 = 0x37
    ldi r22, 0x5F ;95 = 0x5F
    call _delay_
    ; Apagar Vibrador
    cbi PORTB, 0x00
_fin_recibir_caracter:
    RET

; Retardo
; R19 = nn
; R20 = rr
; R21 = mm
; R22 = xx
_delay_:
    mov xl, r19
_ciclo_1:
    dec xl
    breq _fin_delay_
    mov xh, r20
_ciclo_2:
    dec xh
    breq _ciclo_1
    mov yl, r21
_ciclo_3:
    dec yl
    breq _ciclo_2
    mov yh, r22
_ciclo_4:
    dec yh
    brne _ciclo_4
    rjmp _ciclo_3   
_fin_delay_:
    RET

_verificar_status_control:
    ;Leer PIND
    in r16, PIND
    ;Aplicar Mascara b'11111100' = 0xFC
    andi r16, 0xFC
    ;Correr 2 veces a la derecha para -> b'00111111'
    lsr r16
    lsr r16
    ;Verificar si se presiono algun boton
    brne _aplicar_delay_antirebote_
    RET
_aplicar_delay_antirebote_:
    ;Guardar estado de control en r18
    mov r18, r16
    ;Delay 80ms
    ldi r19, 0x0D ;13 = 0x0D
    ldi r20, 0x1D ;29 = 0x1D
    ldi r21, 0x1D ;29 = 0x1D
    ldi r22, 0x27 ;39 = 0x27
    call _delay_
_esperar_soltar_boton:
    ;Leer PIND
    in r16, PIND
    ;Aplicar Mascara b'11111100' = 0xFC
    andi r16, 0xFC
    ;Correr 2 veces a la derecha para -> b'00111111'
    lsr r16
    lsr r16
    ;Verificar si se solto el boton
    brne _esperar_soltar_boton
    ;Delay 80ms
    ldi r19, 0x0D ;13 = 0x0D
    ldi r20, 0x1D ;29 = 0x1D
    ldi r21, 0x1D ;29 = 0x1D
    ldi r22, 0x27 ;39 = 0x27
    call _delay_
    ;;;Se presiono un boton Correctamente
    ;Verifica si se presiono el boton Arriba -> "U"
    sbrs r18, 0x00
    rjmp _verificacion_abajo_
    ;Enviar caracter "U"
    ldi r18, 0x55
    call _enviar_caracter
    rjmp _vibrar_control_
    ;Verifica si se presiono el boton Abajo -> "D"
_verificacion_abajo_:
    sbrs r18, 0x01
    rjmp _verificacion_derecha_
    ;Enviar caracter "D"
    ldi r18, 0x44
    call _enviar_caracter
    rjmp _vibrar_control_
    ;Verifica si se presiono el boton Derecha -> "R"
_verificacion_derecha_:    
    sbrs r18, 0x02
    rjmp _verificacion_izquierda_
    ;Enviar caracter "R"
    ldi r18, 0x52
    call _enviar_caracter
    rjmp _vibrar_control_
    ;Verifica si se presiono el boton Izquierda -> "L"
_verificacion_izquierda_:
    sbrs r18, 0x03
    rjmp _verificacion_select_
    ;Enviar caracter "L"
    ldi r18, 0x4C
    call _enviar_caracter
    rjmp _vibrar_control_
    ;Verifica si se presiono el boton Select -> "S"
_verificacion_select_:
    sbrs r18, 0x04
    rjmp _verificacion_menu_
    ;Enviar caracter "S"
    ldi r18, 0x53
    call _enviar_caracter
    rjmp _vibrar_control_
    ;Verifica si se presiono el boton Menu -> "M"
_verificacion_menu_:
    sbrs r18, 0x05
    rjmp _fin_verificar_status_control
    ;Enviar caracter "M"
    ldi r18, 0x4D
    call _enviar_caracter
    rjmp _vibrar_control_
_vibrar_control_:
    ;Activar Vibrador en el pin del Puerto G
    sbi PORTB, 0x00
    ;Delay 500ms
    ldi r19, 0x23 ;35 = 0x23
    ldi r20, 0x21 ;33 = 0x21
    ldi r21, 0x21 ;33 = 0x21
    ldi r22, 0x46 ;70 = 0x46
    call _delay_
    ;Apagar Vibrador
    cbi PORTB, 0x00
_fin_verificar_status_control:
    RET