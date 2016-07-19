#define bluetooth Serial3
#define debug Serial

String al1Time = "11:11";
String al2Time = "11:11";
bool al1ck = true;
bool al2ck = true;
double al1KG = 0.0;
double al2KG = 0.0;
bool alerted = false;
int nDosesAl1 = 3;
int nDosesAl2 = 3;

void setup() {
  debug.begin(9600);
  startBluetooth();
}

void loop() {
  String btSerial = "";
  while (bluetooth.available()) {
    delay(50);
    char rxData = (int)bluetooth.read();
    btSerial = btSerial + (String)rxData;
  }
  if (btSerial.length() > 3) {
    debug.println("BT: " + btSerial);
    handleBT(btSerial);
  }
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

void handleBT(String buff) {
  String str = buff;
  while(str.length() > 0){
    if(str.substring(0, 4).indexOf("al1/") > -1){
      if (str.substring(0, 6).indexOf("al1/1d") > -1) {
        str = str.substring(6);
        debug.println("Alimentador 1 dar 1 Dose!");
      }
      if (str.substring(0, 6).indexOf("al1/ck") > -1) {
        if(str.substring(0, 9).indexOf("al1/ck/s/") > -1){
          debug.println("Defenir valor check do Alimentador 1: " + str.substring(9, 10));
          al1ck = (bool)str.substring(9, 10).toInt();
          str = str.substring(10);
        }
      }
      if (str.substring(0, 6).indexOf("al1/ti") > -1) {
        if(str.substring(0, 9).indexOf("al1/ti/s/") > -1){
          int limite = 10+numOfDidits(str.substring(9, 13));
          if(str.substring(9, 13).indexOf("al1") >=0 || str.substring(9, 13).indexOf("al2") >=0 ){
            debug.println(str.substring(9, 13));
            limite--;
          }
          debug.println("Defenir valor tempo do Alimentador 1: " + str.substring(9, limite));
          al1Time = str.substring(9, limite);
          str = str.substring(limite);
        }
      }
      if (str.substring(0, 6).indexOf("al1/gm") > -1) {
        if(str.substring(0, 9).indexOf("al1/gm/s/") > -1){
          int limite = 10+numOfDidits(str.substring(9, 13));
          if(str.substring(9, 13).indexOf("al1") >=0 || str.substring(9, 13).indexOf("al2") >=0 ){
            debug.println(str.substring(9, 13));
            limite--;
          }
          debug.println("Defenir valor peso do Alimentador 1: " + str.substring(9, limite));
          al1KG = str.substring(9, limite).toFloat();
          str = str.substring(limite);
        }
      }
    }else if(str.substring(0, 4).indexOf("al2/") > -1){
      if (str.substring(0, 6).indexOf("al2/1d") > -1) {
        str = str.substring(6);
        debug.println("Alimentador 2 dar 1 Dose!");
      }
      if (str.substring(0, 6).indexOf("al2/ck") > -1) {
        if(str.substring(0, 9).indexOf("al2/ck/s/") > -1){
          debug.println("Defenir valor check do Alimentador 2: " + str.substring(9, 10));
          al2ck = (bool)str.substring(9, 10).toInt();
          str = str.substring(10);
        }
      }
      if (str.substring(0, 6).indexOf("al2/ti") > -1) {
        if(str.substring(0, 9).indexOf("al2/ti/s/") > -1){
          int limite = 10+numOfDidits(str.substring(9, 13));
          if(str.substring(9, 13).indexOf("al1") >=0 || str.substring(9, 13).indexOf("al2") >=0 ){
            debug.println(str.substring(9, 13));
            limite--;
          }
          debug.println("Defenir valor tempo do Alimentador 2: " + str.substring(9, limite));
          al2Time = str.substring(9, limite);
          str = str.substring(limite);
        }
      }
      if (str.substring(0, 6).indexOf("al2/gm") > -1) {
        if(str.substring(0, 9).indexOf("al2/gm/s/") > -1){
          int limite = 10+numOfDidits(str.substring(9, 13));
          if(str.substring(9, 13).indexOf("al1") >=0 || str.substring(9, 13).indexOf("al2") >=0 ){
            debug.println(str.substring(9, 13));
            limite--;
          }
          debug.println("Defenir valor peso do Alimentador 2: " + str.substring(9, limite));
          al2KG = str.substring(9, limite).toFloat();
          str = str.substring(limite);
        }
      }
    }else if (str.substring(0, 5).indexOf("al/al") > -1) {
        alerted = false;
        nDosesAl1 = 3;
        nDosesAl2 = 3;
        str = str.substring(5);
      }else{
      str = "";
    }
  }
  bluetooth.flush();
}

int numOfDidits(String str){
  int num = 0;
  for(int i = 0; i < str.length();i++){
    if((char)str[i] == '0'||'1'||'2'||'3'||'4'||'5'||'6'||'7'||'8'||'9'){
      num++;
    }
    delay(10);
  }
  return num;
}

/*
void handleBT(String str) {
  while (str.length() > 0) {
    if (str.indexOf("al1/1d") > 0) {
      str.replace("al1/1d", "");
      debug.println("Alimentador 1 dar 1 Dose!");
      //dosar(1);
    }
    if (str.indexOf("al2/1d") > 0) {
      str.replace("al2/1d", "");
      debug.println("Alimentador 2 dar 1 Dose!");
      //dosar(1);
    }
    if (str.indexOf("al1t/") > 0) {
      al1Time = str.substring(str.indexOf("al1t/") + 5 , str.indexOf("al1t/") + 11);
      str.replace(str.substring(str.indexOf("al1t/"), str.indexOf("al1t/") + 11), "");
      debug.println("Defenido tempo do Alimentador 1 para: " + (String)al1Time);
    }
    if (str.indexOf("al2t/") > 0) {
      al2Time = str.substring(str.indexOf("al2t/") + 5 , str.indexOf("al2t/") + 11);
      str.replace(str.substring(str.indexOf("al2t/"), str.indexOf("al2t/") + 11), "");
      debug.println("Defenido tempo do Alimentador 2 para: " + (String)al2Time);
    }
    if (str.indexOf("al1ck/get") > 0) {
      bluetooth.println(al1ck);
      debug.println("Enviar check do Alimentador 1 para Bluetooth");
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
    if (str.indexOf("al2ck/get") > 0) {
      debug.println("Enviar check do alimentador 2 para Bluetooth");
      bluetooth.println(al2ck);
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
}*/
