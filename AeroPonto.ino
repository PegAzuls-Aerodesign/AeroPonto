//Programa: Teste modulo cartao micro SD Arduino
//Autor: Arduino e Cia
#include <SPI.h>
#include <SD.h>
//Pino de conexão do pino CS do modulo
const byte chipSelect = 4;
const byte buttonPin = 2;
const byte ledPin = 5;

byte buttonState = 0;

void setup()
{
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
  Serial.println("Inicializando cartao SD...");
  
  // Inicia comunicacao com o modulo SD
  if (!SD.begin(chipSelect))
  {
    Serial.println("Falha ao acessar o cartao !");
    return;
  }
  
  Serial.println("Cartao iniciado corretamente !");
  Serial.println();
}

void loop()
{
  buttonState = digitalRead(buttonPin);
  unsigned long currentMillis = millis();
 
  // Abre o arquivo datalog.txt
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  
  // Grava as informacoes no arquivo
  if (dataFile) {
    if (buttonState == HIGH) {
      dataFile.print("Batido ponto de Paulo");
      digitalWrite(ledPin, HIGH);

      Serial.println("Batido ponto de Paulo");  // Log
    }
    
    dataFile.close();
  } else {
    Serial.println("Erro ao abrir o arquivo datalog.txt");
  }

  delay(2000);
  digitalWrite(ledPin, LOW);
}
