/*
-----------------------------------------------------------------------------------------------
Módulo simulador para balanças HX711
COMUNICAÇÃO SERIAL
BAUD RATE           9600
DATA SIZE           8
PARITY              none
HANDSHAKE           OFF
MODE                Free
-----------------------------------------------------------------------------------------------
DESENVOLVEDOR       DATA
Ismael Valentin     21/03/2019
-----------------------------------------------------------------------------------------------
ALTERACOES 
NOME                DATA          OBSERVACAO
-----------------------------------------------------------------------------------------------
COMANDOS SERIAIS ENTRADA
NOME                OBSERVACAO
{DEBUG_ON}          Ativa o modo debug da balança mostrando pesagem e calibração
{DEBUG_OFF}         Desativa o modo debug voltando a enviar pesagens continuamente pela serial
{TARA}              Seta a tara na balança
{200280}            Seta o fator calibração, neste caso 200280 (PADRÃO BALANÇA BARRA)
{S10}               Soma 10 ao fator calibração
{D10}               Diminui 10 ao fator calibração
{S100}              Soma 100 ao fator calibração
{D100}              Diminui 100 ao fator calibração
{S1000}             Soma 1000 ao fator calibração
{D1000}             Diminui 1000 ao fator calibração
{S10000}            Soma 10000 ao fator calibração
{D10000}            Diminui 10000 ao fator calibração
-----------------------------------------------------------------------------------------------
caracteres especiais
STX \u000218
ETX \u0003
*/

#include "HX711.h"      // Biblioteca HX711 
 
#define DOUT 8          // HX711 DATA OUT = pino A0 do Arduino 
#define SCK 9           // HX711 SCK IN = pino A1 do Arduino 
 
HX711 balanca;          // instancia Balança HX711
 
float calibration_factor = 200280;                                 // fator de calibração para teste inicial
float peso = 0;
String comandoSerial;
char caractereSerial;
int debug = 0;                                                     // 1 ATIVA DEBUG   
 
void setup()
{
  Serial.begin(9600);                                              // monitor serial 9600 Bps
 
  balanca.begin(DOUT, SCK);
  balanca.set_scale();                                             // configura a escala da Balança
  zeraBalanca ();                                                  // zera a Balança

  comandoSerial = "";
}
 
void zeraBalanca ()
{
  balanca.tare();                                                 // zera a Balança
  if (debug == 1){
    Serial.println();                                               // salta uma linha
    Serial.println("Balança Zerada ");  
  }
}

void printPeso(){

  String valor = String(peso, 6);

  valor.replace(".","");

  String txt = valor;
    
  for( byte i= 0; i<(10 - valor.length()); i++ )
    txt = "0" + txt;      
      
  Serial.print(txt);
}
 
void loop()
{
  balanca.set_scale(calibration_factor);
  peso = balanca.get_units();

  if (peso < 0){
    peso = 0;
  }
  
  if(debug == 1){
    Serial.print("Peso: ");                                         // imprime no monitor serial
    Serial.print(balanca.get_units(), 4);                           // imprime peso da balança com 3 casas decimais
    Serial.print(" kg");
    Serial.print("\t");
    Serial.print("Fator de Calibração: ");                          // imprime no monitor serial
    Serial.println(calibration_factor);                             // imprime fator de calibração
  }else{
    
    /*Serial.write(0x02);                                             // STX - Start of text
    //Serial.print(peso, 4);
    Serial.write("i0  ");    
    printPeso();
    Serial.write(0x0D);                                             // CR - Enter
    Serial.write(0x08);                                             // BS - BackSpace
    */
    Serial.write("PL: ");                                             // STX - Start of text
    //Serial.print(peso, 6);
    printPeso();
    Serial.write(" T: 000170");  
    Serial.write(0x0D);                                             // CR - Enter
    Serial.write(0x0A);              
  }
 
  if (Serial.available())                                           // reconhece letra para ajuste do fator de calibração
  {

    caractereSerial = Serial.read();
    if (caractereSerial != '\n'){
      comandoSerial.concat(caractereSerial);
    }
    
    if(caractereSerial == '}'){

      if (debug == 1){
        Serial.print("COMANDO RECEBIDO: ");
        Serial.println(comandoSerial);
      }

      if (comandoSerial == "{DEBUG_ON}")
        debug = 1;
      else if (comandoSerial == "{DEBUG_OFF}")
        debug = 0;
      else if (comandoSerial == "{S10}")    // = aumenta 10
        calibration_factor += 10;
      else if (comandoSerial == "{D10}")    // = diminui 10
        calibration_factor -= 10;
      else if (comandoSerial == "{S100}")   // = aumenta 100
        calibration_factor += 100;
      else if (comandoSerial == "{D100}")   // = diminui 100
        calibration_factor -= 100;
      else if (comandoSerial == "{S1000}")  // = aumenta 1000
        calibration_factor += 1000;
      else if (comandoSerial == "{D1000}")  // = diminui 1000
        calibration_factor -= 1000;
      else if (comandoSerial == "{S10000}") // = aumenta 10000
        calibration_factor += 10000;
      else if (comandoSerial == "{D10000}") // = dimuni 10000
        calibration_factor -= 10000;
      else if (comandoSerial == "{TARA}")   // = zera a Balança
        zeraBalanca ();           

      comandoSerial = "";
    }
    delay(10);
  }
  
  /*
  scale.power_down();              // put the ADC in sleep mode
  delay(5000);
  scale.power_up()
  */
}

