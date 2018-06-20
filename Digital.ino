 /*
 * 201605021932
 * Este sketch e a primeira versao de comando de uma carro de
 * controle remoto por arduino.
 * A ideia e utilizar o arduino para realizar as mesmas funcoes
 * do controle remoto padrao e poder adicionar novas funcionalidades
 * que nao se encontram na versao original de fabrica
 * O controle remoto padrao foi substituido por uma bluetooth
 * shield.
 * 
 * FUNCIONAMENTO GERAL
 * 
 * Sera utilizado um aplicativo android para enviar os comandos para
 * o arduino. Estes comandos serao interpretados e suas respectivas
 * funcoes executadas.
 * 
 * Os comandos serao enviados no formato de byte;
 * 01 byte sera suficiente para todas as operacoes
 * Cada funcao do controle enviara o bit correspondente do comando
 * como 1 e 0 para parar cancelar a operacao. 
 * EX: o bit 1 representa o comando para andar para frente. 
 * Ao apertar o botao para frente no app sera enviado 00000001
 * marcando como 1 a funcao para frente. Ao soltar o botao do app
 * sera enviado o byte 00000000 que cancelara as operacoes.
 * 
 * bit 1 - para frente
 * bit 2 - para tras
 * bit 3 - para esquerda
 * bit 4 - para a direita
 * bit 5 - farol
 * bit 6 - buzina
 * 
 * 
 * FUNCIONALIDADES:
 *  - para frente;
 *  - para tras;
 *  - esquerda;
 *  - direita;
 *  - buzina;
 *  - farol e lanterna.
 */


/*
 * Para que a serial do Arduino continue livre para DEBUG, utiliza-
 * remos uma serial via software, utilizando os pinos 7,8 que não
 * são PWM.
 */
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

/*
 * Nesta versao estamos utilizando um motorshield que controla ate 02
 * motores DC atraves dos pinos IN1, IN2,IN3,IN4.
 */

// Pinos de controle do motorshield LN298D
// Os pinos escolhidos sao PWM uma possivel futura funcionalidade.
int IN1 = 9; // para frente roxo
int IN2 = 8; // para tras cinza
int IN3 = 10; // vira esquerda branco
int IN4 = 11; // vira direita preto 


/*
 * rightState e leftState serao utilizadas para saber se o carro
 * estava com a direcao acionada. Estes estados servirao para 
 * realizar a estabilizacao da direcao via software, caso o carro
 * nao possua uma estabilizacao mecanica. Isto e, a direcao volta ao
 * centro automaticamente.
 */
byte rightState = 0;
byte leftState = 0;

byte command;  // nesta variavel sera guardado o byte enviado pelo
               // aplicativo
               
byte front, back, left, right; // estas variaveis receberao os bits
                               // de direcao correspondentes

bool stability; // controle de estabilizacao
byte stabilityTimeLeft; // variavel de tempo de estabilizacao da Esquerda
byte stabilityTimeRight; // variavel de tempo de estabilizacao da Direita

void setup()
{

 //Configurando os pinos de saida do motorshield 
 pinMode(IN1, OUTPUT);
 pinMode(IN2, OUTPUT);
 pinMode(IN3, OUTPUT);
 pinMode(IN4, OUTPUT);

// desativando todas as ligacoes dos motores
 digitalWrite(IN1,HIGH);
 digitalWrite(IN2,HIGH);
 analogWrite(IN3,255);
 analogWrite(IN4,255);

stability = 1;
stabilityTimeLeft = 10; 
stabilityTimeRight = 16; 

// Ativando a serial de conexao com o bluetooth.    
  mySerial.begin(9600);
}

void loop() 
{  

 /* Se nada for recebido pela serial, o estado do carrinho continua*/ 
  if (mySerial.available())
    {
    command = mySerial.read(); // recebe o byte da serial

    /*
     * Utilizando a operacao &, somente o bit correspondente e retor-
     *nara e sera transformado em 0 ou 1 pela operacao 'bool'
     */
    front = bool(command & 1);     //bit 1
    back = bool(command & 2);      //bit 2
    left = bool(command & 4);      //bit 3
    right = bool(command & 8);     //bit 4
    
    //Se o comando de direcao for enviado, sera ativado o estado
    // correspondente, direita ou esquerda
    if(right)
        rightState = 1;

    if(left)
        leftState = 1;

    
    //Neste ponto os motores serao acionados.
    analogWrite(IN3,left);
    analogWrite(IN4,right); 
    digitalWrite(IN1,front);
    digitalWrite(IN2,back);


/* Se a estabilizacao por software estiver ativada as funcionalidades
 * serao executadas.
 * O motor da direcao serao ativados por um breve periodo de tempo e 
 * desativado logo em seguida.
 */
    if(stability){
      // estaBilizacao para a direita.
      if((!right) && (rightState)){
          analogWrite(IN3,0);
          delay(stabilityTimeRight);  
          analogWrite(IN3,255);
          rightState = 0;
        }

      //estabilizacao para a esquerda.
      if((!left) && (leftState)){
          analogWrite(IN4,0);
          delay(stabilityTimeLeft);  
          analogWrite(IN4,255);
          leftState = 0;
        }
    } //if(stability){


/*
 * Acionamento das luzes. O farol tera 3 niveis. Como esta com PWM
 * sera acionado em multiplos de 85 indicado pelo lightsState
 */ 
    
  } //if (mySerial.available())
  
 } // void loop ()
