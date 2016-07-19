
#define SMS Serial2
//SoftwareSerial SMS = SoftwareSerial(6,7);

const int GSM_ON = 9;
const int GSM_CHECK = A8;
int data[1];

void setup()
{
  Serial.begin(9600);
  SMS.begin(19200);
  delay(10);
  pinMode(GSM_ON, OUTPUT);
  turnGSMOn();
  Serial.write("\nTest Start\n");
  sendSMS();
  //handleClock();
}

void sendSMS()
{
  SMS.print("AT+CMGF=1\r");
  delay(100);
  SMS.println("AT+CMGS=\"+351964292976\"");
  delay(100);
  SMS.println("Testing");
  delay(100);
  SMS.println((char)26);
  delay(100);
  SMS.println();
  delay(5000);
}

void handleClock(){
  /*
   * data[0] = horas
   * data[1] = minutos
   */
  SMS.flush();
  delay(1000);
  SMS.println("AT+CCLK?");
  delay(1000);
  String timeStr = "";
  while(SMS.available()){
    timeStr += (char)SMS.read();
  }
  timeStr = timeStr.substring(29, 34);
  data[0] = timeStr.substring(0, 2).toInt();
  data[1] = timeStr.substring(3).toInt();
  Serial.println("Hora: " + (String)data[0] + ":" + (String)data[1]);
}

void turnGSMOn(){
  Serial.println("Verificando se modulo de SMS está ligado!");
  int nMedidas = 10;
  int medidas[nMedidas];
  for(int i = 0; i < nMedidas;i++){
    medidas[i] = analogRead(GSM_CHECK);
    delay(500);
  }
  int somaMedidas = 0;
  for(int i = 0; i < nMedidas;i++){
    somaMedidas += medidas[i];
  }
  int mediaMedidas = somaMedidas / nMedidas;
  if(mediaMedidas>500){
    Serial.println("Ligando modulo SMS!");
    digitalWrite(GSM_ON, HIGH);
    delay(3000);
    digitalWrite(GSM_ON, LOW);
    delay(5000);
  }
}

void loop()
{
  while (SMS.available())
  {
    char rxData = (int)SMS.read();
    Serial.print(String(rxData));
  }

  while (Serial.available())
  {
    char rxData1 = (int)Serial.read();
    SMS.print(String(rxData1));
  }
}
