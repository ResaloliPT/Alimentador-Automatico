/* I/O do projeto
   1 butao - Up Menu
   2 butao - Down Menu
   3 butao - Ok Menu/Back Menu
   4 keypad - Escrever
   5 LCD - Apresentar dados
   6 Switch - Ligar/Desligar Corrente
*/


//Biblioteccas
//Por ordem
//Wire (Controlar I2C)
//EEPROM (Guardar/Usar dados na EEPROM)
//Keypad para usar numeros
//Ecra LCD
//SoftwareSerial (Comunicaçao)
//Wire Comunicaçao I2C
//Balança
#include <EEPROM.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <HX711.h>

//Defenicoes
HX711 balanca(20, 11);

//Defenir posicoes da EEPROM
const int nIDS = 3; // Quantidade de Posiçoes
const int primeiraConfigID = 0; // Posiçao que quarda o valor da primeira configuração (true ou false)
const int startAutoID = 1; // Posiçao que quarda o valor do auto-start (true ou false)
const int escala = 2;


//Variaveis
const byte ROWS = 4; // N de Linhas no teclado
const byte COLS = 3; // N de colunas no teclado
char keys[ROWS][COLS] = { // Maping do teclado
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

bool isRunning = false;
bool isReseting = false; // Se o arduino vai apagar os dados todos
int menusStatus[6] = {1, 0, 0, 0, 0, 0}; // Posicao do menu
String al1Time = "";
String al2Time = "";
bool al1ck = false;
bool al2ck = false;
double al1KG = 0.0;
double al2KG = 0.0;
boolean buttonActive = false;
boolean longPressActive = false;
long buttonTimer = 0;
long longPressTime = 250;
int data[2];

//TODO Change PINOUT
//Arduino PinOut
const int butao1 = 30; // Pinout do butao up Menu
const int butao2 = 31; // Pinout do butao down menu
const int butao3 = 32; // Pinout do ok/back menu
byte rowPins[ROWS] = {5, 4, 3, 2}; // Pinout das linhas do keypad
byte colPins[COLS] = {8, 7, 6}; // Pinout das colunas do keypad

//LiquidCrystal_I2C(uint8_t lcd_Addr, uint8_t En, uint8_t Rw, uint8_t Rs, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t backlighPin, t_backlighPol pol = POSITIVE )
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Conectar I2C no LCD

//Keypad(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols)
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setMenuStatus(int i, int c, int e, int bt, int al1 , int al2) {
  menusStatus[0] = i;
  menusStatus[1] = c;
  menusStatus[2] = e;
  menusStatus[3] = bt;
  menusStatus[4] = al1;
  menusStatus[5] = al2;
}

void calibrarBalanca(){
  balanca.set_scale();
  balanca.tare();
  lcd.setCursor(0, 0); // Muda a posição do cursor no LCD
  lcd.print("Meta algo que");
  lcd.setCursor(2, 1); // Muda a posição do cursor no LCD
  lcd.print("saiba o peso!"); // Apresenta a senha no LCD
  while (handleButao() != 1){}
  balanca.set_scale(balanca.get_units(10)/3);
}

void configurarBT() { // Reestabelece uma nova senha
  randomSeed(analogRead(A0)); // Faz a leitura do pin A0 e fas uma semente para generar numeros aleatórios
  int randomINT = random(1000, 9999); // Cria um numero aleatório de 4 digitos
  lcd.clear(); // Limpa o LCD
  Serial2.println("$$$"); // Muda o módulo bluetooth para modo de comandos
  Serial2.println(); // Manda uma linha nova
  delay(100); // O micro-controlador espera por 100 milésimas
  Serial2.print("SP," + (String)randomINT); // Muda a senha do módulo bluetooth
  delay(100); // O micro-controlador espera por 100 milésimas
  Serial2.println("---"); // Muda o módulo bluetooth para modo normal
  lcd.setCursor(0, 0); // Muda a posição do cursor no LCD
  lcd.print("Bluetooth Pass: " + randomINT); // Apresenta a senha no LCD

  bool BTConnected = false; // Guarda se o módulo está ligado ou nao
  String stringBT = ""; // Guarda o que o módulo enviar para o micro-controlador

  while (!BTConnected) { // Enquanto o bluetooth não estiver ligado
    while (Serial2.available()) { // Enquanto estiver coisas para serem lidas do módulo
      stringBT = stringBT + (String)((char)Serial2.read()); // Faz a leitura do módulo e adiciona á fraze
    }

    if (stringBT.indexOf("CONNECT")) { // Se a frase conter "CONNECT"
      BTConnected == true; // Guarda na variavel que o módulo de bluetooth está ligado a um dispositivo
    }
    delay(50); // O micro-controlador espera por 50 milésimas
  }
}

void configAl1(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Al1   [ ]   ");
  lcd.setCursor(0, 1);
  lcd.print("   :      .  Kg ");
  
  lcd.blink();
  int butaoSel = handleButao();
  if(butaoSel == 1){
    al1ck = true;
    lcd.print("X");
  }else if(butaoSel == 2){
    al1ck = false;
    lcd.print("O");
  }

  String timetemp = "";
  char keyPress;
  int t = 0;
tt1:
  if(t != 0){
    lcd.setCursor(2, 1);
  }else{
    lcd.setCursor(1, 1);
  }
tt2:
  keyPress = handleKeypad();
  if(keyPress == '#' || keyPress == '*'){
    goto tt1;
  }else{
    lcd.print(keyPress);
    timetemp += keyPress;
    if(t == 0){
      t++;
      goto tt2;
    }
  }
  lcd.print(":");
  timetemp += ":";
  t = 0;
tt3:
  if(t != 0){
    lcd.setCursor(5, 1);
  }else{
    lcd.setCursor(4, 1);
  }
tt4:
  keyPress = handleKeypad();
  if(keyPress == '#' || keyPress == '*'){
    goto tt3;
  }else{
    lcd.print(keyPress);
    timetemp += keyPress;
    if(t == 0){
      t++;
      goto tt4;
    }
  }
  al1Time = timetemp;
  
  t = 0;
  String al1kgtemp = "";
tt5:
  if(t != 0){
    lcd.setCursor(9, 1);
  }else{
    lcd.setCursor(8, 1);
  }
tt6:
  keyPress = handleKeypad();
  if(keyPress == '#' || keyPress == '*'){
    goto tt5;
  }else{
    lcd.print(keyPress);
    al1kgtemp += keyPress;
    if(t == 0){
      t++;
      goto tt6;
    }
  }
  lcd.print(".");
  al1kgtemp += ".";
  t = 0;
tt7:
  if(t != 0){
    lcd.setCursor(12, 1);
  }else{
    lcd.setCursor(11, 1);
  }
tt8:
  keyPress = handleKeypad();
  if(keyPress == '#' || keyPress == '*'){
    goto tt7;
  }else{
    lcd.print(keyPress);
    al1kgtemp += keyPress;
    if(t == 0){
      t++;
      goto tt8;
    }
  }
  al1KG = al1kgtemp.toFloat();
}

void configAl2(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    Al2   [ ]   ");
  lcd.setCursor(0, 1);
  lcd.print("   :      .  Kg ");
  
  lcd.blink();
  int butaoSel = handleButao();
  if(butaoSel == 1){
    al2ck = true;
    lcd.print("X");
  }else if(butaoSel == 2){
    al2ck = false;
    lcd.print("O");
  }

  String timetemp = "";
  char keyPress;
  int t = 0;
tt9:
  if(t != 0){
    lcd.setCursor(2, 1);
  }else{
    lcd.setCursor(1, 1);
  }
tt10:
  keyPress = handleKeypad();
  if(keyPress == '#' || keyPress == '*'){
    goto tt9;
  }else{
    lcd.print(keyPress);
    timetemp += keyPress;
    if(t == 0){
      t++;
      goto tt10;
    }
  }
  lcd.print(":");
  timetemp += ":";
  t = 0;
tt11:
  if(t != 0){
    lcd.setCursor(5, 1);
  }else{
    lcd.setCursor(4, 1);
  }
tt12:
  keyPress = handleKeypad();
  if(keyPress == '#' || keyPress == '*'){
    goto tt11;
  }else{
    lcd.print(keyPress);
    timetemp += keyPress;
    if(t == 0){
      t++;
      goto tt12;
    }
  }
  al2Time = timetemp;
  
  t = 0;
  String al2kgtemp = "";
tt13:
  if(t != 0){
    lcd.setCursor(9, 1);
  }else{
    lcd.setCursor(8, 1);
  }
tt14:
  keyPress = handleKeypad();
  if(keyPress == '#' || keyPress == '*'){
    goto tt13;
  }else{
    lcd.print(keyPress);
    al2kgtemp += keyPress;
    if(t == 0){
      t++;
      goto tt14;
    }
  }
  lcd.print(".");
  al2kgtemp += ".";
  t = 0;
tt15:
  if(t != 0){
    lcd.setCursor(12, 1);
  }else{
    lcd.setCursor(11, 1);
  }
tt16:
  keyPress = handleKeypad();
  if(keyPress == '#' || keyPress == '*'){
    goto tt15;
  }else{
    lcd.print(keyPress);
    al2kgtemp += keyPress;
    if(t == 0){
      t++;
      goto tt16;
    }
  }
  al2KG = al2kgtemp.toFloat();
}

void menuConfigurar() { // Desenha o menu seleção de configuração
  while (true) {
    if (menusStatus[1] == 5) {
      menusStatus[1] == 1;
    } else if (menusStatus[1] == 0) {
      menusStatus[1] == 4;
    }

    switch (menusStatus[1]) { // Dependendo do valor na segunda posiçao(começa no 0)
      case 1: // Se o valor for 1
        lcd.clear(); // Limpa o LCD
        lcd.setCursor(0, 0); // Muda a posiçao do Cursor no LCD
        lcd.print("->Bluetooth"); // Escreve no LCD
        lcd.setCursor(0, 1); // Muda a posiçao do Cursor no LCD
        lcd.print("  Primeiro Ali."); // Escreve no LCD
        break; // Sai do Switch

      case 2: // Se o valor for 2
        lcd.clear(); // Limpa o LCD
        lcd.setCursor(0, 0); // Muda a posiçao do Cursor no LCD
        lcd.print("->Primeiro Ali."); // Escreve no LCD
        lcd.setCursor(0, 1); // Muda a posiçao do Cursor no LCD
        lcd.print("  Segundo Ali."); // Escreve no LCD
        break; // Sai do Switch

      case 3: // Se o valor for 3
        lcd.clear(); // Limpa o LCD
        lcd.setCursor(0, 0); // Muda a posiçao do Cursor no LCD
        lcd.print("->Segundo Ali."); // Escreve no LCD
        lcd.setCursor(0, 1); // Muda a posiçao do Cursor no LCD
        lcd.print("  Bluetooth"); // Escreve no LCD
        break; // Sai do Switch

      case 4: // Se o valor for 4
        lcd.clear(); // Limpa o LCD
        lcd.setCursor(0, 0); // Muda a posiçao do Cursor no LCD
        lcd.print("->Hard Reset"); // Escreve no LCD
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
      } else if (menusStatus[4] == 4) {
        hardReset();
        setMenuStatus(1, 0, 0, 0, 0 , 0);
        menuIniciar();
      }
    } else if (butaoSel == 4) {
      menusStatus[1]++;
    }

  }
}

void menuIniciar() { // Desenha o menu Inicial
  while (true) {
    if (menusStatus[0] == 3) {
      menusStatus[0] == 1;
    } else if (menusStatus[0] == 0) {
      menusStatus[0] == 3;
    }
    
    switch (menusStatus[0]) { // Dependendo do valor na primeira posiçao
      case 1: // Se o valor for 1
        lcd.clear(); // Limpa o LCD
        lcd.setCursor(0, 0); // Muda a posiçao do Cursor no LCD
        lcd.print("->Configurar"); // Escreve no LCD
        lcd.setCursor(0, 1); // Muda a posiçao do Cursor no LCD
        lcd.print("  Executar"); // Escreve no LCD
        break;

      case 2: // Se o valor for 2
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
      }
    } else if (butaoSel == 4) {
      menusStatus[0]++;
    }
  }
}

void hardReset() { // Apaga os dados todos da eeprom
  Serial.println("Apagando Memoria");
  for (int i = 0; i < nIDS; i++) { // Caminha por todos os enderecos da eeprom
    EEPROM.write(i, 0); // Escreve o valor 0 na posicao da eeprom
  }
}

void getTimeGSM() { // Funcao para obter a hora
  Serial.println("Obter Tempo NTP"); // Linha para Debug

  Serial1.println("AT=CNTP=\"2.pt.pool.ntp.org\",-1,1,2");
  for(int i = 0;i<10;i++){
    delay(500);
  }
  String timeStr = "";
  while(Serial1.available()){
    timeStr += (char)Serial1.read();
  }
  Serial1.print("AT+CCLK=\"" + timeStr.substring(timeStr.indexOf(",\"")+2, timeStr.indexOf(",\"")+19) + "-01\"");
}

String centrarStr(String str) { // Sentra a string no LCD
  while (str.length() <= 16 && str.length() + 2 <= 16) { // Enquanto tiver espaco no lcd
    str = " " + (String)str + " "; // Adiciona espacos nos lados da string
  }
  return str; // Retorna o valor da string
}

void handleClock(){
  /*
   * data[0] = horas
   * data[1] = minutos
   */

  Serial1.println("AT+CCLK");

  String timeStr = "";
  while(Serial1.available()){
    timeStr += (char)Serial1.read();
  }

  data[0] = timeStr.substring(timeStr.indexOf(",") + 1, timeStr.indexOf(",") + 3).toInt();
  data[1] = timeStr.substring(timeStr.indexOf(",") + 3, timeStr.indexOf(",") + 5).toInt();
}

void exec() {
  Serial.println("Executar");
    lcd.clear();
    lcd.setCursor(0, 0);
    handleClock();
    lcd.print("Time: " + (String)data[0] + ":" + (String)data[1]);
    lcd.setCursor(0, 1);
    lcd.print(centrarStr("Executando"));
}

void mandarSMS(String numero, String sms){
  Serial1.println("AT+CMGF=1");
  delay(500);
  Serial1.println("AT+CMGS=\"" + numero + "\"");
  delay(100);
  Serial1.println(sms);
  delay(100);
  Serial1.write(0x26);
}

void setup() {
  //COMs
  //COM Com Computador
  Serial.begin(9600);
  //COM Com GSM
  Serial1.begin(115200);
  //COM Com Bluetooth
  Serial2.begin(115200);
  delay(1000);

  Serial.println("COMs Started!"); //Debug Line

  getTimeGSM(); // Sicronisa o relogio local com o do servidor

  lcd.begin(16, 2);

  //Defenir saidas e Entradas
  // Entradas
  pinMode(butao1, INPUT);
  pinMode(butao1, INPUT);
  pinMode(butao1, INPUT);

  //Check primeira configuração
  if (EEPROM.read(primeiraConfigID) == 0) {
    EEPROM.write(primeiraConfigID, 1);
    Serial.println("Going Config Mode"); //Debug Line
    setMenuStatus(0, 1, 0, 0, 0, 0); // Muda o menu
    menuConfigurar(); // Desenha o menu seleçao de Configuração
  } else if (EEPROM.read(startAutoID) == 1) {
    Serial.println("Going Auto-Execute Mode"); //Debug Line
    setMenuStatus(0, 0, 1, 0, 0, 0); // Muda o menu
    exec(); // Desenha o menu Inicial
  } else {
    Serial.println("Going Start Menu Mode"); //Debug Line
    setMenuStatus(1, 0, 0, 0, 0, 0); // Muda o menu
    menuIniciar(); // Desenha o menu Inicial
  }

  if (digitalRead(butao1) && digitalRead(butao2) && digitalRead(butao3)) {
    isReseting = true;
  }

  Serial.println("Setup Ended!"); //Debug Line
  calibrarBalanca();
}


/*
    al1ck/true
    al2ck/false
    al1ck/get
    al2ck/get
    al1t/get
    al2t/get
*/
void handleBT(String str) {
  while (str.length() > 0) {
    if (str.indexOf("al1/1d")) {
      str.replace("al1/1d", "");
      //dosar(1);
    }
    if (str.indexOf("al2/1d")) {
      str.replace("al2/1d", "");
      //dosar(1);
    }
    if (str.indexOf("al1t/")) {
      al1Time = str.substring(str.indexOf("al1t/") + 5 , str.indexOf("al1t/") + 11);
      str.replace(str.substring(str.indexOf("al1t/"), str.indexOf("al1t/") + 11), "");
    }
    if (str.indexOf("al2t/")) {
      al2Time = str.substring(str.indexOf("al2t/") + 5 , str.indexOf("al2t/") + 11);
      str.replace(str.substring(str.indexOf("al2t/"), str.indexOf("al2t/") + 11), "");
    }
    if (str.indexOf("al1ck/get")) {
      Serial2.println(al1ck);
      str.replace(str.substring(str.indexOf("al1t/get"), str.indexOf("al1t/get") + 9), "");
    } else {
      if ((bool)str.substring(str.indexOf("al1ck/") + 6 , str.indexOf("al1t/get") + 11) == true) {
        al1ck = true;
        str.replace(str.substring(str.indexOf("al1t/"), str.indexOf("al1t/get") + 10), "");
      } else {
        al1ck = false;
        str.replace(str.substring(str.indexOf("al1t/"), str.indexOf("al1t/get") + 11), "");
      }
    }
    if (str.indexOf("al2ck/get")) {
      Serial2.println(al2ck);
      str.replace(str.substring(str.indexOf("al2t/get"), str.indexOf("al2t/get") + 9), "");
    } else {
      if ((bool)str.substring(str.indexOf("al2ck/") + 6 , str.indexOf("al2t/get") + 11) == true) {
        al2ck = true;
        str.replace(str.substring(str.indexOf("al2t/"), str.indexOf("al2t/get") + 10), "");
      } else {
        al2ck = false;
        str.replace(str.substring(str.indexOf("al2t/"), str.indexOf("al2t/get") + 11), "");
      }
    }
  }
}

char handleKeypad() {
  while (true) {
    if (kpd.getKeys()) {
      for (int i = 0; i < LIST_MAX; i++) {
        if (kpd.key[i].stateChanged) {
          switch (kpd.key[i].kstate) {
            case HOLD:
              return kpd.key[i].kchar;
              break;
          }
        }
      }
    }
    delay(10);
  }
}

int handleButao() {
  /* 1 up
     2 down
     3 ok
     4 back
  */
  while (true) {
    if (digitalRead(butao1)){
      return 1;
    } else if (digitalRead(butao3)) {
      if (buttonActive == false) {
        buttonActive = true;
        buttonTimer = millis();
      }
      if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
        longPressActive = true;
        return 4;
       } else {
          if (buttonActive == true) {
            if (longPressActive == true) {
            longPressActive = false;
          }
          buttonActive = false;
        }
      return 3;
      }
    } else if (digitalRead(butao2)) {
      return 2;
    }
  }
}

void loop() {
  if (isReseting) {
    hardReset();
  }
  if (Serial2.available() > 0) {
    String btSerial = "";
    while (Serial2.available() > 0) {
      btSerial = (String)Serial2.read();
    }
    handleBT(btSerial);
  }
  if (digitalRead(butao1) && digitalRead(butao2) && digitalRead(butao3) && isRunning == false) {
    isRunning = true;
    while(isRunning){
      if (digitalRead(butao1) && digitalRead(butao2) && digitalRead(butao3) && isRunning == true) {
        isRunning = false;
        break;
      }
    }
  }
}

