// Código para os Arduinos Pro Mini (Controlador de Motor de Passo via I2C)
#include <Wire.h>

#define DEBUG true

#define pinMotor1 9
#define pinMotor2 8
#define pinMotor3 7
#define pinMotor4 6

// Defina o endereço I2C deste Pro Mini
#define I2C_ADDRESS 0x10  // Controlador do Motor de Passo: endereço 0x10

int portasMotor[4] = { pinMotor1, pinMotor2, pinMotor3, pinMotor4 };

int sequencia[8][4] = {
  { HIGH, LOW, LOW, HIGH },  // Passo 1
  { HIGH, LOW, LOW, LOW },   // Passo 2
  { HIGH, HIGH, LOW, LOW },  // Passo 3
  { LOW, HIGH, LOW, LOW },   // Passo 4
  { LOW, HIGH, HIGH, LOW },  // Passo 5
  { LOW, LOW, HIGH, LOW },   // Passo 6
  { LOW, LOW, HIGH, HIGH },  // Passo 7
  { LOW, LOW, LOW, HIGH }    // Passo 8
};

int passo = 0;
unsigned long delayPasso;

//Variáveis de controle da movimentação do motor
int estado = 0;        //0=parado  1=girando
int sentido = 0;       //0=sobe    1=desce
int velocidade = 255;  //(de 0-parado até 255-velocidade máxima)

void setup() {
  pinMode(pinMotor1, OUTPUT);
  pinMode(pinMotor2, OUTPUT);
  pinMode(pinMotor3, OUTPUT);
  pinMode(pinMotor4, OUTPUT);

  Wire.begin(I2C_ADDRESS);  // Inicializa o I2C com o endereço específico
  Wire.onReceive(recebeDados);
  Wire.onRequest(enviarDados);

#if DEBUG
  Serial.begin(9600);
  Serial.println("DEBUG Iniciado");
#endif
}

void loop() {
  if (((micros() - delayPasso) > (750 + ((255 - velocidade) * 36.27451))) && (velocidade > 0) && (estado != 0)) {
    delayPasso = micros();

    if (!sentido) {
      passo++;
      if (passo > 7) passo = 0;
    } else {
      passo--;
      if (passo < 0) passo = 7;
    }

    for (int nL = 0; nL < 4; nL++) {
      digitalWrite(portasMotor[nL], sequencia[passo][nL]);
    }
  }
}

void recebeDados(int num_bytes) {
  int command = Wire.read();

  if (command != -1) {
    bool acaoCmd = bitRead(command, 7);
    bool estadoCmd = bitRead(command, 6);
    bool sentidoCmd = bitRead(command, 5);
    int valorCmd = (command & 0x1F);  

#if DEBUG
    Serial.print(F(" acao:"));
    Serial.print(acaoCmd);
    Serial.print(F(" estado:"));
    Serial.print(estadoCmd);
    Serial.print(F(" sentido:"));
    Serial.print(sentidoCmd);
    Serial.print(F(" valor:"));
    Serial.println(valorCmd);
#endif

    if (!acaoCmd) {
      //Comando de acionamento direto do motor
      estado = estadoCmd;
      sentido = sentidoCmd;
      velocidade = (valorCmd * 8.22);  //Converte a velocidade de 0 a 31 para a faixa de 0 a 255
    }
  }
}

void enviarDados() {
  Wire.write(0);  //nada definido para retornar
}