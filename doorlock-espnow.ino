//10번보드의 코드

#include <ESP8266WiFi.h>
#include <espnow.h>

#define relay D3

byte board[] = {0x40,0x91,0x51,0x4D,0xDC,0x51};

unsigned long t = 0;
bool is_relay_on = false;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  pinMode(relay,OUTPUT);
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
  if(is_relay_on){
    if(millis() - t > 100){
      is_relay_on = false;
      digitalWrite(relay,LOW);
    }
  }
  /*
  if(millis() - t > 2000){
    t = millis();
    //2초마다 한번씩 하겠다!
    esp_now_send(board, (uint8_t *) &data, sizeof(data));
  }
  */
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
    //응답한다!
    Serial.println("문열어라!");
    char data = '0';
    esp_now_send(board, (uint8_t *) &data, sizeof(data));
    //릴레이를 제어하고
    digitalWrite(relay,HIGH);
    is_relay_on = true;
    t = millis();
    //delay(100);
    //digitalWrite(relay,LOW);
  }
}
