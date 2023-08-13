//Library Koneksi
#include <WiFi.h>
#include <HTTPClient.h>

//Set Koneksi WiFi
const char* ssid = "Redmi Note 9";
const char* password = "1234567890";
String serverName = "http://192.168.176.100";

//Rotary
#define outputA 25 //pin DT
#define outputB 33 //pin CLK
int aLastState;
float distance = 0; 
float meter = 0;
int counter;
int aState;


//Relay
int relay = 2;

//Deklarasi 
String checkset;
int setmeter = 0;
int lock = 1;

void setup() {
  //Serial
  Serial.begin(115200);

  //Pinmode
  pinMode(outputA,INPUT);
  pinMode(outputB,INPUT);
  pinMode(relay, OUTPUT);

  
    

  //Relay
  digitalWrite(relay,LOW);
  
  //Rotary
  aLastState = digitalRead(outputA);

  //WiFi
  WiFi.begin(ssid, password);
  Serial.println("Menyambungkan");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Terhubung. IP Local anda: ");
  Serial.println(WiFi.localIP());
}

void(* resetFunc) (void) = 0;

void loop() {  
  if(lock == 1){
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      //URL
      String serverPath = serverName + "/apkweb/action.php?action1=get";
      http.begin(serverPath.c_str());
  
      //Ambil Data
      int httpResponseCode = http.GET();
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
          
        //Explode
        String checkSet = checkset.substring(0,10);
        Serial.print("[SET] data dari web : ");
        Serial.println(payload);

        //Ubah set meter
        setmeter = payload.toInt();

        //Convert String ke Int
        Serial.print("Hasil convert String ke Int : ");
        Serial.println(setmeter);
        
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
        http.end();

        //Lanjut motor RUN jika nilai tidak 0
        delay(1000);
        if(setmeter >= 1){
          lock = 2;
        }
    } else {
      Serial.println("WiFi Tidak Terhubung");
    }
  }

  //Intruksi update menjadi RUN
  if(lock == 2){
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      //URL
      String serverPath = serverName + "/apkweb/run.php";
      http.begin(serverPath.c_str());
    
      //Ambil Data
      int httpResponseCode = http.GET();
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
            
        //Explode
        String checkSet = checkset.substring(0,10);
        Serial.print("[Status] dari web : ");
        Serial.println(payload);
          
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
        http.end();
        //Lanjut ke Rotary
        lock = 3;
    }
  }

  
  //Intruksi motor run
  if(lock == 3){
    //Jika sudah memenuhi set meter
    if(meter >= setmeter ){
      Serial.println("Panjang terpenuhi");
      Serial.print("Motor DC off. Sistem akan direset!");
      Serial.println("");

      //OFF
      digitalWrite(relay,LOW);
     

      //Update jika sudah selesai
      if(WiFi.status()== WL_CONNECTED){
        HTTPClient http;
        //URL
        String serverPath = serverName + "/apkweb/done.php";
        http.begin(serverPath.c_str());
    
        //Ambil Data
        int httpResponseCode = http.GET();
        if (httpResponseCode>0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          String payload = http.getString();
            
          //Explode
          String checkSet = checkset.substring(0,10);
          Serial.print("[Status] dari web : ");
          Serial.println(payload);
          
        } else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
          http.end();
      }
      
      //Reset
      delay(2000);
      resetFunc();
    } else {
      //ON
      digitalWrite(relay,HIGH);
      

      //Rotary
      aState = digitalRead(outputA);
      if(aState != aLastState){
        if(digitalRead(outputB) != aState){
          counter --;
        } else {
          counter++;
        }
          Serial.print("Position : ");
          Serial.println(counter);
    
          distance = counter*0.2978;//konversi ke cm
          meter = (distance/100);//konversi ke m
    
          Serial.print(meter);
          Serial.print(" dari ");
          Serial.print(setmeter);
          Serial.println("meter");
       }
       aLastState = aState;
    }
  }
}
