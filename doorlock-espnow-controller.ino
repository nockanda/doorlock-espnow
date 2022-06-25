//10번보드의 코드

#include <ESP8266WiFi.h>
#include <espnow.h>

#define btn D3
#define red_led D4
#define green_led D5

byte board[] = {0x84,0xCC,0xA8,0xAE,0xBB,0xED};

unsigned long t = 0;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  pinMode(btn,INPUT_PULLUP);
  pinMode(red_led,OUTPUT);
  pinMode(green_led,OUTPUT);
  WiFi.mode(WIFI_STA); //제일 처음 할일! 와이파이모드를 스테이션 모드로!

  //ESPNOW의 기능을 활성화한다!
  if (esp_now_init() != 0) {
    //ESP NOW 시작 실패!
    Serial.println("ESPNOW 실패!");
    return;
  }else{
     //ESP NOW 시작 성공!
     Serial.println("ESPNOW 성공!");
  }

  //지금 이보드의 역할은 주고 받는것을 다하겠다!
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  //나의 슬레이브를 등록해야겠다!
  esp_now_add_peer(board, ESP_NOW_ROLE_COMBO, 1, NULL, 0);//뒤에 2개는 ID, PW

  //송신완료 콜백함수 등록
  esp_now_register_send_cb(OnDataSent);

  //수신완료 콜백 함수 등록
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(btn) == LOW){
    digitalWrite(red_led,HIGH);
    digitalWrite(green_led,LOW);
    char data = '0';
    esp_now_send(board, (uint8_t *) &data, sizeof(data));
    delay(300);
  }
  /*
  if(millis() - t > 2000){
    t = millis();
    //2초마다 한번씩 하겠다!
    esp_now_send(board, (uint8_t *) &data, sizeof(data));
  }*/
}

//송신콜백함수 원형:
void OnDataSent(uint8_t *mac, uint8_t status) {
   //status가 0일때 송신 완료!
   if(status == 0){
      Serial.println("성공적으로 송신했음!");
   }else{
      Serial.println("송신 실패!!");
   }
}


//수신콜백함수 원형:
void OnDataRecv(uint8_t * mac, uint8_t * data, uint8_t len) {
  if(data[0] == '0'){
    Serial.println("성공적으로 제어가 되었음!");
    digitalWrite(red_led,LOW);
    digitalWrite(green_led,HIGH);
  }
}
