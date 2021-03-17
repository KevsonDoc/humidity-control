// Instituto Federal de Brasília. Campus Ceilândia

//Alunos: Cauã Nascimento, Daniel Alves, Nillo Buarque, Thales Sousa, Kelyton de Lucas

//Programação do Projeto Integrador. No projeto foi utilizado o conjunto de blibiotecas + o aplicativo blynk.

// Parte da programação Wi-Fi.

//--------------------------------------CAIXA CONTROLADORA DE TEMPERATURA E UMIDADE--------------------------------------


//Inclusão de Blibiotecas
#define BLYNK_PRINT Serial    
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <DHT.h>

//-------------------------------------------------------------------------------


//inclusão do token de autenticação para controle pelo app e inclusão do nome e senha da rede local.

char auth[] = "d1v6m7ixaSiJI8tRh0lDw7dv5TTx-xxU"; 
char ssid[] = "ALHN-2E82";  
char pass[] = "HzbBN94=7h";  

//-------------------------------------------------------------------------------

//definindo onde o sensor e o módulo relé vão ser conectados no hardware.

#define DHTPIN 2          
#define DHTTYPE DHT11   
#define rele1 14
#define rele2 0

//-------------------------------------------------------------------------------

//conversando com o dht informando o pino e o tipo.

DHT dht(DHTPIN, DHTTYPE);

//enviando informação do sensor para o aplicativo de 1 em 1 segundo
SimpleTimer timer;

//-------------------------------------------------------------------------------

//declaração de variaveis para o funcioanemto correto do aplicativo e das informações aparentes neste.

int temperatura;
int valor;

int F;

float umidadeAmbiente;
float umidadeUser;

float temperaturaAmbiente;
float temperaturaUser;

//-------------------------------------------------------------------------------

//essa função pega o valor do pino virtual setado no aplicativo e salva este valor na variavel temperaturaUser.

BLYNK_WRITE(V2)
{
    temperaturaUser = param.asInt(); //param.asInt() serve para ler o valor do pino virtual.
}

//essa função faz o controle da temperatura; ela ira ler qual a temperatura atual e, então, desligara ou ligara o ventilador se esta estiver de acordo com o escolhido pelo usuario.
void controleTemperatura()
{
   pinMode(rele1, OUTPUT); // definindo o pino rele 1 como saída

  
   temperaturaAmbiente = dht.readTemperature(); //lendo a temperatura do sensor e salvando em temperaturaAmbiente.
  
  if(temperaturaAmbiente >= temperaturaUser)
  {
      digitalWrite(rele1, HIGH);
   
   } 

   else
   {
        digitalWrite(rele1, LOW);

   }
}


//-------------------------------------------------------------------------------

//essa função pega o valor do pino virtual setado no aplicativo e salva este valor na variavel umidadeUser.


BLYNK_WRITE(V3)
{
  
  umidadeUser = param.asInt(); //param.asInt() serve para ler o valor do pino virtual.
  
}

//essa função faz o controle da umidade; ela ira ler qual a umidade atual e, então, desligara ou ligara o umidificador se esta estiver de acordo com o escolhido pelo usuario.

void controleUmidade()
{

  pinMode(rele2,OUTPUT); // definindo o pino rele 2 como saída
  
   umidadeAmbiente = dht.readHumidity(); //lendo informações do sensor e salvando na varialvel umidadeAmbiente
    
  if(umidadeAmbiente < umidadeUser)
  {
      digitalWrite(rele2, HIGH);
  } 

   else 
   {
        digitalWrite(rele2, LOW);
   }
}

//-------------------------------------------------------------------------------

//essa função envia para os pinos virtuais(onde esta inserido uma tela lcd) a temperatura local, fazendo o tratamento para fahrenheit. 

void enviatemp()
{
  temperatura = dht.readTemperature();
  F = (temperatura*9/5)+ 32;
  Blynk.virtualWrite(V0, F);
}


//-------------------------------------------------------------------------------

//essa função envia para os pinos virtuais(onde esta inserido uma tela lcd) a umidade local em porcentagem. 


void enviapot()
{
  valor = dht.readHumidity();
  Blynk.virtualWrite(V1, valor);
  
}

//--------------------------------------------------------------------------------

//essa função faz a leitura do sensor tanto a umidade quanto a temperatura e envia esse resultado para os pino virtuais 5 e 6.

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {  // se umidade ou temperatura não for um número[isnan(is not a number)], então envia uma mensagem no serial dizendo que houve falha, se for um numero, continua-se a a transmitir os valores para os pinos virtuais.
    Serial.println("Falha ao ler o sensor DHT11");
    return;
  }


  Blynk.virtualWrite(V5, h);  //V5 para o umidificador
  Blynk.virtualWrite(V6, t);  //V6 para a temperatura
}

//--------------------------------------------------------------------------------

//iniciando a comunicação com o sensor de temperatura, bem como a comunicação serial e validando as credenciais.


void setup()
{
  Serial.begin(9600); 
  Blynk.begin(auth, ssid, pass);

  dht.begin();


// timer.setInterval serve para que não inunde o servidor de comandos e atualizações das funções, apenas de um em um segundo.

  timer.setInterval(1000L, sendSensor);
  timer.setInterval(1000L, enviatemp);
  timer.setInterval(1000L, enviapot);
  timer.setInterval(1000L, controleTemperatura); 
  timer.setInterval(1000L, controleUmidade); 
}

//--------------------------------------------------------------------------------

//iniciando a comunicação com o servidor

void loop()
{
 timer.run(); //iniciando o tempo setado do sensor DHT11
  if (Blynk.connected())
   {
    Blynk.run(); 
   } else Blynk.connect(); // Blynk.connect serve para caso o esp se desconecte, e então ele fica no loop tentando conectar-se a rede
}
