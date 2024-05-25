// Biblioteca Geral
#include <SPI.h>
// Bibliotecas para o RFID
#include <Wire.h>
#include <MFRC522.h>
//Biblioteca para SD card
#include <SD.h>
// Bilioteca para RTC
#include "RTClib.h"
// Biblioteca para o LCD
#include <LiquidCrystal.h>

#define SS_PIN 10     //Pino SDA
#define RST_PIN 9     //Pino de RESET
#define chipSelect 4  //Pino de conexão do pino CS do modulo SD

MFRC522 rfid(SS_PIN, RST_PIN);  //Passagem de parâmetros referente aos pinos para o RFID

RTC_DS1307 rtc;  //Objeto do tipo RTC_DS1307

LiquidCrystal disp(8,   // RS no digital 8
                   7,   // EN no digital 7
                   5,   // D4 no digital 5
                   4,   // D5 no digital 4
                   3,   // D6 no digital 3
                   2);  // D7 no digital 2

//Declaração dos dias da semana para o RTC
char daysOfTheWeek[7][12] = { "Domingo", "Segunda", "Terça", "Quarta", "Quinta", "Sexta", "Sábado" };

// Pinos referentes ao leds
const int redLED = 7;
const int greenLED = 8;

// Registro para guardar os nomes e as tags do RFID que será ligada a pessoa
struct AeroPoint {
  String tag, name;
  int validation;
};

// Inicialização do vetor do tipo AeroPoint
AeroPoint aeroPoint[7];

void setup() {
  // Inicialização das Bibliotecas
  Serial.begin(9600);
  Wire.begin();       // WIRE
  SPI.begin();        // Barramento SPI
  rfid.PCD_Init();    // MFRC522
  disp.begin(16, 2);  // LCD

  // Saídas
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  // Se o rtc não for inicializado
  if (!rtc.begin()) {
    Serial.println("DS1307 não encontrado");
    while (1)
      ;  // Sempre entre no loop
  }
  // Se o rtc não estiver rodando, configura
  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("DS1307 configurado");
  }

  // Inicia comunicacao com o modulo SD
  Serial.println("Iniciando SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Falha ao acessar o cartao !");
    return;
  }
  Serial.println("Inicialização completa.");

  // Preenchimento do vetor
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
  aeroPoint[6].name = "Levy";
  aeroPoint[6].tag = "D3:0B:4D:11";

  for (int i = 0; i < 7; i++) {
    aeroPoint[i].validation = 0;
  }
}

void loop() {
  // Variável para verificação
  int veri;
  //Chama as funções
  veri = leituraRfid();
  verifyCheckIn(aeroPoint, veri);
  // Resetar o valor da validação, para esparar entrada e saída
  for(int i = 0; i < 7; i++){
    if(aeroPoint[i].validation > 2){
      aeroPoint[i].validation = 0;
    }
  }
}

// Função de validação e confirmação da tag RIFD
int leituraRfid() {
  int position;

  // Verifica se o cartão presente no leitor é diferente do último cartão lido. Caso não seja retorna para ler novamente
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  /***Início do bloco responsável por gerar a tag RIFD lida***/
  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") + String(rfid.uid.uidByte[i], HEX) + (i != 3 ? ":" : "");
  }
  strID.toUpperCase();
  /***Fim do bloco***/

  for (int i = 0; i < 7; i++) {
    // Verifica se a tag lida é igual a alguma tag já cadastrada
    if (strID.indexOf(aeroPoint[i].tag) >= 0) {  //SE O ENDEREÇO DA TAG LIDA FOR IGUAL AO ENDEREÇO INFORMADO, FAZ
      logCard(aeroPoint, i);
      disp.setCursor(0, 0);
      disp.print(aeroPoint[i].name);
      showDate();
      aeroPoint[i].validation += 1;
      position = i;
    }
  }

  rfid.PICC_HaltA();       //PARADA DA LEITURA DO CARTÃO
  rfid.PCD_StopCrypto1();  //PARADA DA CRIPTOGRAFIA NO PCD

  return position;
}

void logCard(AeroPoint* person, int position) {
  digitalWrite(chipSelect, LOW);

  // Abre o arquivo datalog.txt
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  if (dataFile) {
    Serial.println("File opened ok");
    dataFile.print(person[position].name);
    dataFile.print(", ");

    DateTime now = rtc.now();
    dataFile.print(daysOfTheWeek[now.dayOfTheWeek()]);
    dataFile.print(" - ");
    dataFile.print(now.day(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.year(), DEC);
    dataFile.print(" - ");
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);
    dataFile.println();
    Serial.println("sucessfully written on SD card");
    dataFile.close();
  } else {
    Serial.println("error opening data.txt");
  }

  digitalWrite(chipSelect, HIGH);
}

void verifyCheckIn(AeroPoint* veri, int position) {
  if (veri[position].validation == 1) {
    digitalWrite(greenLED, HIGH);
    delay(2000);
    digitalWrite(greenLED, LOW);
    disp.setCursor(0, 1);
    disp.print("                ");
    disp.print("You're welcome!");
  } else {
    digitalWrite(redLED, HIGH);
    delay(2000);
    digitalWrite(redLED, LOW);
    disp.setCursor(0, 1);
    disp.print("                ");
    disp.println("You are late...");
  }
}

void showDate() {
  DateTime now = rtc.now();

  disp.setCursor(0, 1);
  disp.print(now.day(), DEC);
  disp.print('/');
  disp.print(now.month(), DEC);
  disp.print(" - ");
  disp.print(now.hour(), DEC);
  disp.print(':');
  disp.print(now.minute(), DEC);
}
