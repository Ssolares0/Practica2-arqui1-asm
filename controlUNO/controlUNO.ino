
extern "C" {
  extern void _config_ports();        //Configuracion Inicial
  extern void _config_usart0();   //Configuracion USART0
  extern void _recibir_caracter();//Recibe Caracter ASCCI
  //extern void _start_();
  extern void _verificar_status_control();
}

void setup() {
  // put your setup code here, to run once:
  _config_ports();
  _config_usart0();
}

void loop() {
  // put your main code here, to run repeatedly:
  //_enviar_caracter();
  //delay(800);
  //_start_();
  _recibir_caracter();
  _verificar_status_control();
}

