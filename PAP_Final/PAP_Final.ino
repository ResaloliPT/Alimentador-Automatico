/* Bibliotecas
   Wire.h I2C
   LiquidCrystal_I2C.h LCD
   HX711.h Balança
   Keypad.h Teclado
   EEPROM.h Memoria do Arduino
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//#include <HX711.h>
#include <Keypad.h>
#include <EEPROM.h>

/* Definições
   Serviço de Bluetooth
   Serviço de SMS
*/
#define bluetooth Serial3
#define SMS Serial2
#define debug Serial

/* Variaveis
   motoresAl1 - Pins para Motor do alimentador 1
   motoresAl2 - Pins para Motor do alimentador 2
   GSM_ON - Pin para ligar/desligar modulo GSM
   GSM_CHECK - Pin para verificar se o modulo de SMS está ligado
   nivelAgua  - Pin para verificar nivel de agua
   1 butao - Up Menu
   2 butao - Down Menu
   3 butao - Ok Menu/Back Menu
   ROWS - N de Linhas no teclado
   COLS - N de colunas no teclado
   keys - Teclas do teclado
   rowPins - Pins das linhas do taclado
   colPins -  Pins das colunas do taclado
   al1Time - Hora para alimentar do primeiro aliementador
   al2Time - Hora para alimentar
   al1ck - De quando a quando ou naquela hora do alimentador 1
   al2ck - De quando a quando ou naquela hora do alimentador 2
   al1KG - Quantidade de Raçao do alimentador 1
   al2KG - Quantidade de Raçao do alimentador 2
   buttonActive - Se o butao 3 está ativo
   longPressActive - Se o butao 3 esta ativo por muito tempo
   buttonTimer - Tempo que o butao 3 está ativo
   longPressTime - Tempo minimo para ativar o butao no modo LongPress
   menusStatus[6] - Posiçao do menu
   data[2] - Hora e minuto
   nIDS - Numero de IDs na memoria
   primeiraConfigID - Se a primeira config ja foi feita
   startAutoID - Se vai auto Executar
   al1Done - Se ja deu a dose do dia
   al2Done = Se ja deu a dose do dia
   tempo - Hora para o Arduino
   alerted - se ja foi alertado pela agua
*/

const int motoresAl1[2] = {48, 49};
const int motoresAl2[2] = {50, 51};
const int GSM_ON = 9;
const int GSM_CHECK = A8;
const int nivelAgua = A10;
//const int butao1 = 43;
//const int butao2 = 45;
//const int butao3 = 47;
//const byte ROWS = 4;
//const byte COLS = 3;
//char keys[ROWS][COLS] = {
//  {'1', '2', '3'},
//  {'4', '5', '6'},
//  {'7', '8', '9'},
//  {'*', '0', '#'}
//};
//byte rowPins[ROWS] = {38, 36, 34, 32};
//byte colPins[COLS] = {39, 37, 35};
int nDosesAl1 = 3;
int nDosesAl2 = 3;
String al1Time = "";
String al2Time = "";
bool al1ck = false;
bool al2ck = false;
double al1KG = 0.0;
double al2KG = 0.0;
//boolean buttonActive = false;
//boolean longPressActive = false;
//long buttonTimer = 0;
//long longPressTime = 250;
//int menusStatus[6] = {1, 0, 0, 0, 0, 0};
int data[2];
const int nIDS = 8;
const int primeiraConfigID = 0;
const int startAutoID = 1;
const int al1ckID = 2;
const int al2ckID = 3;
const int al1TimeID = 4;
const int al2TimeID = 5;
const int al1KGID = 6;
const int al2KGID = 7;
const String tempo = "16/07/20,11:50:00-01";
bool alerted = false;

/* Declarações
   LCD
   Balança
   Teclado
*/
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
//HX711 balanca(20, 11);
//Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

/* Funções
   setup() Executado uma vez quando o Arduino Inicia
   Loop() Execudato Repetidamente enquanto o Arduino se encontra ligado
   pinModes() Defenição das entradas e saidas
   turnONSMS() Liga o modulo de SMS
   startSMS() Inicia o Modulo SMS
   startComms() Inicia as Várias maneira de Comunicação
   calibrarBalanca() Calibração da Balança
   configAl1() Configurçao do primeiro alimentador
   configAl2() Configurçao do segundo alimentador
   handleButao() Retorna o butao carregado
   handleKeypad() Retorna a tecla do teclado carregada
   handleBT() Dependendo da mensagen do Bluetoothe configura algo
   numOfDidits() Retorna a quantidade de numero em uma frase
   testeLCD() Testa o backlight do lcd e texto
   setMenuStatus() Posiçao do Menu
   menuConfigurar() Menu para configurar o Projeto
   menuIniciar() Menu Inicial
   hardReset() Apaga memoria
   handleClock() Vai buascar a hora
   centrarStr() Centra a String no LCD
   sendSMS() Manda SMS
   rotacao() Controla rotaçao dos motores
   ckNivelAgua() Verifica nivel de agua
   dosar() Fornece uma dose
*/
void setup() {
  startComms();
  pinModes();
  debug.println("Comunicações Iniciadas!");
  SMS.println("AT+CCLK=\"" + tempo + "\"");
  //calibrarBalanca();
  debug.println("Setup Pronto!");

  /*if (EEPROM.read(primeiraConfigID) == 0) {
    EEPROM.write(primeiraConfigID, 1);
    debug.println("Going Config Mode"); //Debug Line
    setMenuStatus(0, 1, 0, 0, 0, 0); // Muda o menu
    menuConfigurar(); // Desenha o menu seleçao de Configuração
    } else if (EEPROM.read(startAutoID) == 1) {
    loadMemory();
    debug.println("Going Auto-Execute Mode"); //Debug Line
    setMenuStatus(0, 0, 1, 0, 0, 0); // Muda o menu
    exec(); // Desenha o menu Inicial
    } else {
    loadMemory();
    debug.println("Going Start Menu Mode"); //Debug Line
    setMenuStatus(1, 0, 0, 0, 0, 0); // Muda o menu
    menuIniciar(); // Desenha o menu Inicial
    }*/
}

/* I/O do projeto
   1 butao - Up Menu
   2 butao - Down Menu
   3 butao - Ok Menu/Back Menu
   4 keypad - Escrever
   5 LCD - Apresentar dados
   6 Switch - Ligar/Desligar Corrente
*/

void loop() {
  exec();
  String btSerial = "";
  while (bluetooth.available()) {
    delay(50);
    char rxData = (int)bluetooth.read();
    btSerial = btSerial + (String)rxData;
  }
  if (btSerial.length() > 3) {
    debug.println("BT: " + btSerial);
    handleBT(btSerial);
  } else {
    delay(1000);
  }
}

void pinModes() {
  /*pinMode(butao1, INPUT_PULLUP);
    pinMode(butao2, INPUT_PULLUP);
    pinMode(butao3, INPUT_PULLUP);*/
  pinMode(GSM_ON, OUTPUT);
  pinMode(GSM_CHECK, INPUT);
  pinMode(nivelAgua, INPUT);
  pinMode(motoresAl1[0], OUTPUT);
  pinMode(motoresAl1[1], OUTPUT);
  pinMode(motoresAl2[0], OUTPUT);
  pinMode(motoresAl2[1], OUTPUT);
}

void turnONSMS() {
  debug.println("Verificando se modulo de SMS está ligado!");
  int nMedidas = 10;
  int medidas[nMedidas];
  for (int i = 0; i < nMedidas; i++) {
    medidas[i] = analogRead(GSM_CHECK);
    delay(500);
  }
  int somaMedidas = 0;
  for (int i = 0; i < nMedidas; i++) {
    somaMedidas += medidas[i];
  }
  int mediaMedidas = somaMedidas / nMedidas;
  if (mediaMedidas > 500) {
    debug.println("Ligando modulo SMS!");
    digitalWrite(GSM_ON, HIGH);
    delay(3000);
    digitalWrite(GSM_ON, LOW);
    delay(5000);
  }
}

void startSMS() {
  debug.println("Iniciando comunicação SMS!");
  SMS.begin(19200);
  delay(10);
  pinMode(GSM_ON, OUTPUT);
  turnONSMS();
  delay(50);
  debug.println("SMS Pronto!");
}

void startBluetooth() {
  debug.println("Iniciando comunicação Bluetooth!");
  bluetooth.begin(115200);
  bluetooth.print("$$$");
  delay(50);
  bluetooth.println("SU,9600");
  delay(50);
  bluetooth.begin(9600);
  bluetooth.println("---");
  delay(50);
  debug.println("Bluetooth Pronto!");
}

void startComms() {
  debug.begin(9600);
  startBluetooth();
  startSMS();
  //lcd.begin(16, 2);
  //testeLCD();
  delay(5000);
}

/*void printLCD(String str, int col, int row) {
  lcd.setCursor(row, col);
  lcd.print(str);
  }*/

/*void calibrarBalanca() {
  balanca.set_scale();
  balanca.tare();
  lcd.setCursor(0, 0); // Muda a posição do cursor no LCD
  lcd.print("Meta algo que");
  lcd.setCursor(2, 1); // Muda a posição do cursor no LCD
  lcd.print("saiba o peso!"); // Apresenta a senha no LCD
  while (handleButao() != 1) {}
  balanca.set_scale(balanca.get_units(10) / 3);
  }*/

/*void configAl1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Al1   [ ]   ");
  lcd.setCursor(0, 1);
  lcd.print("   :      .  Kg ");
  lcd.setCursor(11, 0);
  lcd.blink();
  int butaoSel = handleButao();
  if (butaoSel == 1) {
    al1ck = true;
    lcd.print("X");
    debug.println("Config AL2: Checked");
  } else if (butaoSel == 2) {
    al1ck = false;
    lcd.print("O");
    debug.println("Config AL2: NChecked");
  }

  String timetemp = "";
  char keyPress;
  int t = 0;
  tt1:
  if (t != 0) {
    lcd.setCursor(2, 1);
  } else {
    lcd.setCursor(1, 1);
  }
  tt2:
  keyPress = handleKeypad();
  if (keyPress == '#' || keyPress == '*') {
    goto tt1;
  } else {
    lcd.print(keyPress);
    timetemp += keyPress;
    if (t == 0) {
      t++;
      goto tt2;
    }
  }
  lcd.print(":");
  timetemp += ":";
  t = 0;
  tt3:
  if (t != 0) {
    lcd.setCursor(5, 1);
  } else {
    lcd.setCursor(4, 1);
  }
  tt4:
  keyPress = handleKeypad();
  if (keyPress == '#' || keyPress == '*') {
    goto tt3;
  } else {
    lcd.print(keyPress);
    timetemp += keyPress;
    if (t == 0) {
      t++;
      goto tt4;
    }
  }
  al1Time = timetemp;
  debug.println("Config AL2: Time Set(" + al1Time + ")");

  t = 0;
  String al1kgtemp = "";
  tt5:
  if (t != 0) {
    lcd.setCursor(9, 1);
  } else {
    lcd.setCursor(8, 1);
  }
  tt6:
  keyPress = handleKeypad();
  if (keyPress == '#' || keyPress == '*') {
    goto tt5;
  } else {
    lcd.print(keyPress);
    al1kgtemp += keyPress;
    if (t == 0) {
      t++;
      goto tt6;
    }
  }
  lcd.print(".");
  al1kgtemp += ".";
  t = 0;
  tt7:
  if (t != 0) {
    lcd.setCursor(12, 1);
  } else {
    lcd.setCursor(11, 1);
  }
  tt8:
  keyPress = handleKeypad();
  if (keyPress == '#' || keyPress == '*') {
    goto tt7;
  } else {
    lcd.print(keyPress);
    al1kgtemp += keyPress;
    if (t == 0) {
      t++;
      goto tt8;
    }
  }
  al1KG = al1kgtemp.toFloat();
  debug.println("Config AL2: KGs Set(" + al1kgtemp + ")");
  }*/

/*void configAl2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Al2   [ ]   ");
  lcd.setCursor(0, 1);
  lcd.print("   :      .  Kg ");
  lcd.setCursor(11, 0);
  lcd.blink();
  int butaoSel = handleButao();
  if (butaoSel == 1) {
    al2ck = true;
    lcd.print("X");
    debug.println("Config AL2: Checked");
  } else if (butaoSel == 2) {
    al2ck = false;
    lcd.print("O");
    debug.println("Config AL2: NChecked");
  }

  String timetemp = "";
  char keyPress;
  int t = 0;
  tt9:
  if (t != 0) {
    lcd.setCursor(2, 1);
  } else {
    lcd.setCursor(1, 1);
  }
  tt10:
  keyPress = handleKeypad();
  if (keyPress == '#' || keyPress == '*') {
    goto tt9;
  } else {
    lcd.print(keyPress);
    timetemp += keyPress;
    if (t == 0) {
      t++;
      goto tt10;
    }
  }
  lcd.print(":");
  timetemp += ":";
  t = 0;
  tt11:
  if (t != 0) {
    lcd.setCursor(5, 1);
  } else {
    lcd.setCursor(4, 1);
  }
  tt12:
  keyPress = handleKeypad();
  if (keyPress == '#' || keyPress == '*') {
    goto tt11;
  } else {
    lcd.print(keyPress);
    timetemp += keyPress;
    if (t == 0) {
      t++;
      goto tt12;
    }
  }
  al2Time = timetemp;
  debug.println("Config AL2: Time Set(" + al2Time + ")");
  t = 0;
  String al2kgtemp = "";
  tt13:
  if (t != 0) {
    lcd.setCursor(9, 1);
  } else {
    lcd.setCursor(8, 1);
  }
  tt14:
  keyPress = handleKeypad();
  if (keyPress == '#' || keyPress == '*') {
    goto tt13;
  } else {
    lcd.print(keyPress);
    al2kgtemp += keyPress;
    if (t == 0) {
      t++;
      goto tt14;
    }
  }
  lcd.print(".");
  al2kgtemp += ".";
  t = 0;
  tt15:
  if (t != 0) {
    lcd.setCursor(12, 1);
  } else {
    lcd.setCursor(11, 1);
  }
  tt16:
  keyPress = handleKeypad();
  if (keyPress == '#' || keyPress == '*') {
    goto tt15;
  } else {
    lcd.print(keyPress);
    al2kgtemp += keyPress;
    if (t == 0) {
      t++;
      goto tt16;
    }
  }
  al2KG = al2kgtemp.toFloat();
  debug.println("Config AL2: KGs Set(" + al2kgtemp + ")");
  }*/

/*int handleButao() {
  /* 1 up
     2 down
     3 ok
     4 back
*/
/* while (true) {
   if (digitalRead(butao1)) {
     debug.println("Butao: UP");
     return 1;
   } else if (digitalRead(butao3)) {
     if (buttonActive == false) {
       buttonActive = true;
       buttonTimer = millis();
     }
     if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
       longPressActive = true;
       debug.println("Butao: Back");
       return 4;
     } else {
       if (buttonActive == true) {
         if (longPressActive == true) {
           longPressActive = false;
         }
         buttonActive = false;
       }
       debug.println("Butao: OK");
       return 3;
     }
   } else if (digitalRead(butao2)) {
     debug.println("Butao: DOWN");
     return 2;
   }
  }
  }*/

/*char handleKeypad() {
  while (true) {
    if (kpd.getKeys()) {
      for (int i = 0; i < LIST_MAX; i++) {
        if (kpd.key[i].stateChanged) {
          switch (kpd.key[i].kstate) {
            case HOLD:
              char pressed = kpd.key[i].kchar;
              debug.println("KPD: " + pressed);
              return pressed;
              break;
          }
        }
      }
    }
    delay(10);
  }
  }*/

void handleBT(String buff) {
  String str = buff;
  debug.println("BT: " + str);
  while (str.length() > 0) {
    if (str.substring(0, 4).indexOf("al1/") > -1) {
      if (str.substring(0, 6).indexOf("al1/1d") > -1) {
        str = str.substring(6);
        debug.println("Alimentador 1 dar 1 Dose!");
        dosar(1);
      }
      if (str.substring(0, 6).indexOf("al1/ck") > -1) {
        if (str.substring(0, 9).indexOf("al1/ck/s/") > -1) {
          debug.println("Defenir valor check do Alimentador 1: " + str.substring(9, 10));
          al1ck = (bool)str.substring(9, 10).toInt();
          str = str.substring(10);
        }
      }
      if (str.substring(0, 6).indexOf("al1/ti") > -1) {
        if (str.substring(0, 9).indexOf("al1/ti/s/") > -1) {
          int limite = 10 + numOfDidits(str.substring(9, 13));
          if (str.substring(9, 13).indexOf("al1") >= 0 || str.substring(9, 13).indexOf("al2") >= 0 ) {
            debug.println(str.substring(9, 13));
            limite--;
          }
          debug.println("Defenir valor tempo do Alimentador 1: " + str.substring(9, limite));
          al1Time = str.substring(9, limite);
          str = str.substring(limite);
        }
      }
      if (str.substring(0, 6).indexOf("al1/gm") > -1) {
        if (str.substring(0, 9).indexOf("al1/gm/s/") > -1) {
          int limite = 10 + numOfDidits(str.substring(9, 13));
          if (str.substring(9, 13).indexOf("al1") >= 0 || str.substring(9, 13).indexOf("al2") >= 0 ) {
            debug.println(str.substring(9, 13));
            limite--;
          }
          debug.println("Defenir valor peso do Alimentador 1: " + str.substring(9, limite));
          al1KG = str.substring(9, limite).toFloat();
          str = str.substring(limite);
        }
      }
    } else if (str.substring(0, 4).indexOf("al2/") > -1) {
      if (str.substring(0, 6).indexOf("al2/1d") > -1) {
        str = str.substring(6);
        debug.println("Alimentador 2 dar 1 Dose!");
        dosar(2);
      }
      if (str.substring(0, 6).indexOf("al2/ck") > -1) {
        if (str.substring(0, 9).indexOf("al2/ck/s/") > -1) {
          debug.println("Defenir valor check do Alimentador 2: " + str.substring(9, 10));
          al2ck = (bool)str.substring(9, 10).toInt();
          str = str.substring(10);
        }
      }
      if (str.substring(0, 6).indexOf("al2/ti") > -1) {
        if (str.substring(0, 9).indexOf("al2/ti/s/") > -1) {
          int limite = 10 + numOfDidits(str.substring(9, 13));
          if (str.substring(9, 13).indexOf("al1") >= 0 || str.substring(9, 13).indexOf("al2") >= 0 ) {
            debug.println(str.substring(9, 13));
            limite--;
          }
          debug.println("Defenir valor tempo do Alimentador 2: " + str.substring(9, limite));
          al2Time = str.substring(9, limite);
          str = str.substring(limite);
        }
      }
      if (str.substring(0, 6).indexOf("al2/gm") > -1) {
        if (str.substring(0, 9).indexOf("al2/gm/s/") > -1) {
          int limite = 10 + numOfDidits(str.substring(9, 13));
          if (str.substring(9, 13).indexOf("al1") >= 0 || str.substring(9, 13).indexOf("al2") >= 0 ) {
            debug.println(str.substring(9, 13));
            limite--;
          }
          debug.println("Defenir valor peso do Alimentador 2: " + str.substring(9, limite));
          al2KG = str.substring(9, limite).toFloat();
          str = str.substring(limite);
        }
      }
    }
    if (str.substring(0, 5).indexOf("al/al") > -1) {
      debug.println("Resetar Alarmes e Doses");
      alerted = false;
      nDosesAl1 = 3;
      nDosesAl2 = 3;
      str = str.substring(5);
    } else {
      str = "";
    }
  }
  bluetooth.flush();
}

int numOfDidits(String str) {
  int num = 0;
  for (int i = 0; i < str.length(); i++) {
    if ((char)str[i] == '0' || '1' || '2' || '3' || '4' || '5' || '6' || '7' || '8' || '9') {
      num++;
    }
    delay(10);
  }
  return num;
}

/*void testeLCD() {
  printLCD("0123456789abcdef", 0, 0);
  printLCD("0123456789abcdef", 0, 1);
  for (int i = 0; i < 100; i++) {
    lcd.noBacklight();
    delay(10);
    lcd.backlight();
    delay(10);
  }
  }*/

/*void setMenuStatus(int i, int c, int e, int bt, int al1 , int al2) {
  menusStatus[0] = i;
  menusStatus[1] = c;
  menusStatus[2] = e;
  menusStatus[3] = bt;
  menusStatus[4] = al1;
  menusStatus[5] = al2;
  }*/

/*void menuConfigurar() { // Desenha o menu seleção de configuração
  while (true) {
    if (menusStatus[1] == 4) {
      menusStatus[1] == 1;
    } else if (menusStatus[1] == 0) {
      menusStatus[1] == 3;
    }

    switch (menusStatus[1]) { // Dependendo do valor na segunda posiçao(começa no 0)
      case 1: // Se o valor for 1
        debug.println("Bluetooth");
        lcd.clear(); // Limpa o LCD
        lcd.setCursor(0, 0); // Muda a posiçao do Cursor no LCD
        lcd.print("->Bluetooth"); // Escreve no LCD
        lcd.setCursor(0, 1); // Muda a posiçao do Cursor no LCD
        lcd.print("  Primeiro Ali."); // Escreve no LCD
        break; // Sai do Switch

      case 2: // Se o valor for 2
        debug.println("Primeiro Ali.");
        lcd.clear(); // Limpa o LCD
        lcd.setCursor(0, 0); // Muda a posiçao do Cursor no LCD
        lcd.print("->Primeiro Ali."); // Escreve no LCD
        lcd.setCursor(0, 1); // Muda a posiçao do Cursor no LCD
        lcd.print("  Segundo Ali."); // Escreve no LCD
        break; // Sai do Switch

      case 3: // Se o valor for 3
        debug.println("Segundo Ali.");
        lcd.clear(); // Limpa o LCD
        lcd.setCursor(0, 0); // Muda a posiçao do Cursor no LCD
        lcd.print("->Segundo Ali."); // Escreve no LCD
        lcd.setCursor(0, 1); // Muda a posiçao do Cursor no LCD
        lcd.print("  Bluetooth"); // Escreve no LCD
        break; // Sai do Switch
    }

    int butaoSel = handleButao();
    if (butaoSel == 1) {
      menusStatus[1]++;
    } else if (butaoSel == 2) {
      menusStatus[1]--;
    } else if (butaoSel == 3) {
      if (menusStatus[1] == 1) {
        setMenuStatus(0, 0, 0, 1, 0 , 0);
        configAl1();
      } else if (menusStatus[2] == 2) {
        setMenuStatus(0, 0, 0, 0, 1 , 0);
        configAl2();
      } else if (menusStatus[3] == 3) {
        setMenuStatus(0, 0, 0, 0, 0 , 1);
      }
    } else if (butaoSel == 4) {
      menusStatus[1]++;
    }

  }
  }*/

/*void menuIniciar() { // Desenha o menu Inicial
  while (true) {
    if (menusStatus[0] == 3) {
      menusStatus[0] == 1;
    } else if (menusStatus[0] == 0) {
      menusStatus[0] == 3;
    }

    switch (menusStatus[0]) { // Dependendo do valor na primeira posiçao
      case 1: // Se o valor for 1
        debug.println("Configurar");
        lcd.clear(); // Limpa o LCD
        lcd.setCursor(0, 0); // Muda a posiçao do Cursor no LCD
        lcd.print("->Configurar"); // Escreve no LCD
        lcd.setCursor(0, 1); // Muda a posiçao do Cursor no LCD
        lcd.print("  Executar"); // Escreve no LCD
        break;

      case 2: // Se o valor for 2
        debug.println("Executar");
        lcd.clear(); // Limpa o LCD
        lcd.setCursor(0, 0); // Muda a posiçao do Cursor no LCD
        lcd.print("  Config"); // Escreve no LCD
        lcd.setCursor(0, 1); // Muda a posiçao do Cursor no LCD
        lcd.print("->Executar"); // Escreve no LCD
        break;
    }

    int butaoSel = handleButao();
    if (butaoSel == 1) {
      menusStatus[0]++;
    } else if (butaoSel == 2) {
      menusStatus[0]--;
    } else if (butaoSel == 3) {
      if (menusStatus[0] == 1) {
        setMenuStatus(0, 1, 0, 0, 0 , 0);
        menuConfigurar();
      } else if (menusStatus[0] == 2) {
        setMenuStatus(0, 0, 1, 0, 0 , 0);
        exec();
        break;
      }
    } else if (butaoSel == 4) {
      menusStatus[0]++;
    }
  }
  }*/

int count = 0;
void exec() {
  count++;
  debug.println("Executando: " + (String)count + "/20");
  //lcd.clear();
  //lcd.setCursor(0, 0);
  if (count == 20) {
    handleClock();
  }
  if (alerted) {} else {
    if (ckNivelAgua() < 30) {
      debug.println("Agua Baixa (SMS)");
      sendSMS("Al Agua");
      alerted = true;
    }
    if (nDosesAl1 < 1) {
      debug.println("Al1 alimento Baixo (SMS)");
      sendSMS("Al1 Comida");
      alerted = true;
    }
    if (nDosesAl2 < 1) {
      debug.println("Al2 alimento Baixo (SMS)");
      sendSMS("Al2 Comida");
      alerted = true;
    }
  }
  bool al1Done = false;
  bool al2Done = false;
  if (al1ck) {
    if (al1Time.indexOf((String)data[0] + ":" + (String)data[1]) >=0) {
      if (nDosesAl1 - 1 > -1 && al1Done == false) {
        dosar(1);
        al1Done = true;
      }
    } else {
      al1Done = false;
    }
  }

  if (al1ck) {
    if (al2Time.indexOf((String)data[0] + ":" + (String)data[1]) >=0) {
      if (nDosesAl2 - 1 > -1 && al2Done == false) {
        dosar(2);
        al2Done = true;
      }
    } else {
      al2Done = false;
    }
  }
  if (count == 20) {
    debug.println("Hora: " + (String)data[0] + ":" + (String)data[1]);
    count = 0;
  }
  //lcd.print("Hora: " + (String)data[0] + ":" + (String)data[1]);
  //lcd.setCursor(0, 1);
  //lcd.print(centrarStr("Executando"));
}

void handleClock() {
  /*
     data[0] = horas
     data[1] = minutos
  */
  SMS.flush();
  delay(1000);
  SMS.println("AT+CCLK?");
  delay(1000);
  String timeStr = "";
  while (SMS.available()) {
    timeStr += (char)SMS.read();
  }
  timeStr = timeStr.substring(29, 34);
  data[0] = timeStr.substring(0, 2).toInt();
  data[1] = timeStr.substring(3).toInt();
}

String centrarStr(String str) { // Sentra a string no LCD
  while (str.length() <= 16 && str.length() + 2 <= 16) { // Enquanto tiver espaco no lcd
    str = " " + (String)str + " "; // Adiciona espacos nos lados da string
  }
  return str; // Retorna o valor da string
}

void loadMemory() {
  al1Time = EEPROM.read(al1TimeID);
  al2Time = EEPROM.read(al2TimeID);
  al1ck = EEPROM.read(al1ckID);
  al2ck = EEPROM.read(al2ckID);
  al1KG = EEPROM.read(al1KGID);
  al2KG = EEPROM.read(al2KGID);
}

void sendSMS(String txt)
{
  SMS.print("AT+CMGF=1\r");
  delay(100);
  SMS.println("AT+CMGS=\"+351964292976\"");
  delay(100);
  SMS.println(txt);
  delay(100);
  SMS.println((char)26);
  delay(100);
  SMS.println();
  delay(5000);
}


void rotacao(int al, int dir) {
  if (al == 1) {
    digitalWrite(motoresAl1[0], LOW);
    if (dir == 0) {
      digitalWrite(motoresAl1[0], LOW);
      digitalWrite(motoresAl1[1], LOW);
    } else if (dir == 1) {
      digitalWrite(motoresAl1[1], LOW);
      digitalWrite(motoresAl1[0], HIGH);
    } else if (dir == 2) {
      digitalWrite(motoresAl1[1], HIGH);
      digitalWrite(motoresAl1[0], HIGH);
    }
  } else if (al == 2) {
    digitalWrite(motoresAl2[0], LOW);
    if (dir == 0) {
      digitalWrite(motoresAl2[0], LOW);
      digitalWrite(motoresAl2[1], LOW);
    } else if (dir == 1) {
      digitalWrite(motoresAl2[1], LOW);
      digitalWrite(motoresAl2[0], HIGH);
    } else if (dir == 2) {
      digitalWrite(motoresAl2[1], HIGH);
      digitalWrite(motoresAl2[0], HIGH);
    }
  }
}

int ckNivelAgua() {
  int temp = analogRead(nivelAgua);
  temp = map(temp, 340, 550, 0, 100);
  return temp;
}

void dosar(int al) {
  if (al == 1) {
    nDosesAl1--;
    debug.println("AL1 Doses Restantes: " + (String)nDosesAl1);
    rotacao(1, 1);
    delay(5000);
    rotacao(1, 2);
    delay(5000);
    rotacao(1, 0);
  } else if (al == 2) {
    nDosesAl2--;
    debug.println("AL2 Doses Restantes: " + (String)nDosesAl2);
    rotacao(2, 1);
    delay(5000);
    rotacao(2, 2);
    delay(5000);
    rotacao(2, 0);
  }
}
