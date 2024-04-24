#include <Wire.h> //INCLUSÃO DA BIBLIOTECA NECESSÁRIA
#include <SPI.h> //INCLUSÃO DE BIBLIOTECA
#include <MFRC522.h> //INCLUSÃO DE BIBLIOTECA

#define SS_PIN 10 //PINO SDA
#define RST_PIN 9 //PINO DE RESET

MFRC522 rfid(SS_PIN, RST_PIN); //PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS

struct AeroPoint{
  String tag, name; 
}

AeroPoint aeroPoint[6]; 

void setup(){
  Serial.begin(9600);
  Wire.begin(); //INICIALIZA A BIBLIOTECA WIRE
  SPI.begin(); //INICIALIZA O BARRAMENTO SPI
  rfid.PCD_Init(); //INICIALIZA MFRC522
  

  aeroPoint[0].name = "Victória";
  aeroPoint[0].tag = "F3:05:9D:AA";
  aeroPoint[1].name = "Sanzia";
  aeroPoint[1].tag = "C3:BC:1A:AD";
  aeroPoint[2].name = "Max Sena";
  aeroPoint[2].tag = "63:6D:16:AD";
  aeroPoint[3].name = "José";
  aeroPoint[3].tag = "A3:40:30:AD";
  aeroPoint[4].name = "Erick";
  aeroPoint[4].tag = "43:9F:95:AC";
  aeroPoint[5].name = "Paulo";
  aeroPoint[5].tag = "F3:89:AE:1B";
}

void loop() {
  leituraRfid(); //CHAMA A FUNÇÃO RESPONSÁVEL PELA VALIDAÇÃO DA TAG RFID
}

//FUNÇÃO DE VALIDAÇÃO DA TAG RFID
void leituraRfid(){
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) //VERIFICA SE O CARTÃO PRESENTE NO LEITOR É DIFERENTE DO ÚLTIMO CARTÃO LIDO. CASO NÃO SEJA, FAZ
    return; //RETORNA PARA LER NOVAMENTE

  /***INICIO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/
  String strID = ""; 
  for (byte i = 0; i < 4; i++) {
    strID +=
    (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(rfid.uid.uidByte[i], HEX) +
    (i!=3 ? ":" : "");
  }
  strID.toUpperCase();
/***FIM DO BLOCO DE CÓDIGO RESPONSÁVEL POR GERAR A TAG RFID LIDA***/

  //O ENDEREÇO "27:41:AA:AB" DEVERÁ SER ALTERADO PARA O ENDEREÇO DA SUA TAG RFID QUE CAPTUROU ANTERIORMENTE
  for(int i = 0; i < 7; i++){
    if (strID.indexOf(aeroPoint[i].tag) >= 0) { //SE O ENDEREÇO DA TAG LIDA FOR IGUAL AO ENDEREÇO INFORMADO, FAZ
      Serial.println(aeroPoint[i].name);
      // Data, Horar e nome da pessoa
      // Pensar em uma 
    }else{ //SENÃO, FAZ (CASO A TAG LIDA NÃO SEJÁ VÁLIDA)
    
    }
  }

  rfid.PICC_HaltA(); //PARADA DA LEITURA DO CARTÃO
  rfid.PCD_StopCrypto1(); //PARADA DA CRIPTOGRAFIA NO PCD
  }