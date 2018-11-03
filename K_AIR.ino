/*
아두이노와 esp8266또는 wemos d1 mini에 맞춰진 코드입니다.
(wemos d1 mini는 기판 뒷면에 적힌 핀을 보고 연결하셔야합니다.)
빨간색, 초록색, 파란색, Fan핀은 다른핀으로 바꿔서 사용해주세요.
RGB LED는 공통양극(애노드)을 사용했습니다.

Made by Cyrome
*/


#include <ESP8266WiFi.h>
#include <PubSubClient.h>

int Rpin = 16; //빨간 핀
int Gpin = 14; //초록 핀
int Bpin = 12; //파랑 핀
int Fan = 4;
char ssid[] = "Chrome's Router"; //공유기 SSID
char password[] = "min10072003"; // 공유기 비번
const char* mqtt_server = "192.168.31.140"; //mqtt 서버주소(RPI IP)

WiFiClient espClient;
PubSubClient client(espClient);
 
void setup_wifi() { //wifi 연결 함수
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  while (WiFi.status() != WL_CONNECTED) { //연결 상태가 아닐시 반복
    WiFi.begin(ssid, password); //wifi 연결
    delay(500); //0.5초 딜레이
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
 
void reconnect() { //MQTT 연결 함수
  while (!client.connected()) { //연결되지 않았을 경우 반복
    Serial.print("MQTT 연결중..");
    if (client.connect("rpi3bserver")) { //연결되었을 경우
      Serial.println("연결됨");
      
      client.subscribe("/arduino/kair/fan1"); //MQTT 서버 재구독
      client.publish("/", "test msg"); //테스트 메세지
       
    } else { //실패했을경우
      Serial.print("실패, rc=");
      Serial.print(client.state());
      Serial.println("0.5초후 다시 연결");
      delay(500);
    }
  }
}
 
void callback(char* topic, byte* payload, unsigned int length) { //메시지 받는 함수
  Serial.print("메세지값 [");
  Serial.print(topic);
  Serial.print("] ");
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg +=(char)payload[i]; //msg에 한글자씩 추가
  }
  Serial.print(msg);
  Serial.println();
  if(msg =="on"){
    digitalWrite(Fan, HIGH);
  }
  if(msg =="off"){
    digitalWrite(Fan, LOW);
  }
}
 
void setup() {
  Serial.begin(115200);
  pinMode(Rpin ,OUTPUT);  
  pinMode(Gpin ,OUTPUT);  
  pinMode(Bpin ,OUTPUT);
  pinMode(Fan, OUTPUT);
  digitalWrite(Fan, LOW);

  setup_wifi(); //와이파이 연결
  client.setServer(mqtt_server, 1883); //서버연결
  client.setCallback(callback); //메세지 받기
}

void low() //낮음 함수(초록)              
{
   digitalWrite(Rpin,HIGH);
   digitalWrite(Gpin,LOW);
   digitalWrite(Bpin,HIGH);
}

void med() //중간 함수(노랑)         
{
   digitalWrite(Rpin,LOW);
   digitalWrite(Gpin,LOW);
   digitalWrite(Bpin,HIGH);
}

void high() //높음 함수(빨강)             
{
   digitalWrite(Rpin,LOW);
   digitalWrite(Gpin,HIGH);
   digitalWrite(Bpin,HIGH);
}

void loop() {
  if (!client.connected()) {
    Serial.print(":::::");
    reconnect(); //재연결
  }
  client.loop();
  if(analogRead(A0)<160){ //160보다 작은경우
    low(); //낮음 함수
  } else if(analogRead(A0)<200 && analogRead(A0)>=160) { //160이상 200미만인 경우
    med(); //중간 함수
  } else if(analogRead(A0)>=200) { //200이상인 경우
    high(); //높음 함수
  }
  char szBuf[128];
  itoa(analogRead(A0), szBuf, 10); //int를 char로 변경
  client.publish("/arduino/kair/mq1", szBuf); //값 송출
  delay(2000); //라즈베리파이 서버라서 값을 너무 많이 받아오면 서버가 멈추는 현상때문에 2초 딜레이
}
