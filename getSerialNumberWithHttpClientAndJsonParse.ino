//「まいどアリ〜〜」顧客UUID取得部分（Json parse）をリファクタリングしてみた。

#include <M5Stack.h>
#include <WiFi.h>
#include "ArduinoJson.h"
#include <HTTPClient.h>

//WIFIのID
const char *ssid = "yourWiFiSSID";
//WIFIのパースワード設定、オープンネットワークの場合は空白で良い
const char *password = "YourPassword";
//まいどアリ〜〜の一意なUUIDを取得するためのAPIのURL
const char *apiServer = "https://maidoary.herokuapp.com/customers/new";

// セットアップ
void setup()
{

  // M5Stack objectの初期化
  M5.begin();

  Serial.begin(115200);
  delay(10);


  // text の設定
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(3);
  M5.Lcd.println("Connecting to WIFI");

  //
  WiFi.begin(ssid, password);
  //接続済　ではない場合
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    M5.Lcd.fillScreen(BLACK);
    //点を表示する
    M5.Lcd.printf(".");
  }
  //接続できた場合
  M5.Lcd.clear(GREEN);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.println("Wi-Fi connected!\n\n\nPress Button C to get UUID");
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(3);

}

void getUUID(){
  
  //WIFIの接続状態を確認する
  //WIFI接続している場合
  if ((WiFi.status() == WL_CONNECTED))
  {
    HTTPClient http;

    //APIサーバに接続開始
    http.begin(apiServer);

    //HTTP Requestを作成する
    int httpCode = http.GET();

    //返り値を確認する
    if (httpCode > 0)
    {
      //レスポンスを文字列として取得する
      String payload = http.getString();
      Serial.println(payload);
      //正常の場合は200
      M5.Lcd.clear(BLACK);
      M5.Lcd.setCursor(0, 0);
      //jsonに変換するためにメモリを確保
      DynamicJsonDocument doc(1024);
      //payloadをjson objectに変換する
      deserializeJson(doc, payload);
      long uuid = doc["uuid"];
      M5.Lcd.println("UUID="+String(uuid));
      long hit = doc["hit"];
      M5.Lcd.print("hit="+String(hit));
      M5.Lcd.setCursor(0, 160);
      M5.Lcd.print("Press button C to get next UUID.");
    }

    else
    {
      M5.Lcd.printf("HTTP request error!");
      return;
    }
    M5.update();
    //リソースを解放する
    http.end();
  }
}

// メインループ
void loop()
{
    M5.begin();
    if (M5.BtnC.wasPressed()) {
    
    M5.Lcd.clear(BLACK);
    M5.Lcd.setCursor(0, 2);
    M5.Lcd.println("Btn C was pressed");
    M5.Lcd.println("getting UUID from Heroku");
    getUUID();
    }
    M5.update();
}
