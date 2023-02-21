#include <stdio.h>
#include <string.h>


#define DEBUG true
#define MODE_1A

#define DTR_PIN 9
#define RI_PIN 8

#define LTE_PWRKEY_PIN 5
#define LTE_RESET_PIN 6
#define LTE_FLIGHT_PIN 7

String token = "pvkm5NtxE5zmj-pR7uAHROx_9b3Sp0Mt";
String from_usb = "";
String loc = "";
String longitude = "";
String lattitude = ""; 

String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  Serial1.println(command);
  
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (Serial1.available())
    {
      char c = Serial1.read();
      response += c;
    }
  }
  if (debug)
  {
    SerialUSB.print(response);
  }
  return response;
}

int check_signal(void)
{
  while(1)
  {
    String sig = sendData("AT+CSQ",3000,DEBUG);
    int i=0;
    String strength;
    while(sig[i]!=':')i++;

    String loc_2 = sig.substring(i+2);

    i=0;
    while(loc_2[i]!=',')i++;

    strength = loc_2.substring(0,i);

    int strength_1 = strength.toInt();
    SerialUSB.println(strength_1);

    return strength_1;
  }
}

void gpsLocation(String local)
{
   while(1)
  {
    int i=0;
    while(local[i]!=':')i++;

    String loc_2 = local.substring(i+2);

    i=0;
    while(loc_2[i]!=',')i++;

    lattitude = loc_2.substring(0,i);
    SerialUSB.println(lattitude);

    int j = i+3;
    int k = j;

    while(loc_2[k]!=',')k++;

    longitude = loc_2.substring(j,k);
    SerialUSB.println(longitude); 

    return;
    
  }
}

String conversion(String local)
{
  if(local[0]=='0'){
      String str_end = local.substring(3);

      String str_init = local.substring(1,3);

      int val = str_init.toInt();
 
      double deci_val = str_end.toDouble() ;

      double new_value = val+(deci_val / 60);
      SerialUSB.println(new_value);

      String final_val = String(new_value,5);

      return final_val;
   }
  else if(local[0]!='0')
  {
      String str_end = local.substring(2);

      String str_init = local.substring(0,2);

      int val = str_init.toInt();
 
      double deci_val = str_end.toDouble() ;

      double new_value = val+(deci_val / 60);
      SerialUSB.println(new_value);

      String final_val = String(new_value,5);

      return final_val;
  }
}

void map_loc(String lat1,String lon1)
{
    String lat = conversion(lat1);
    String lon = conversion(lon1);

    SerialUSB.print("lattitude = ");SerialUSB.println(lat);
    SerialUSB.print("longitude = ");SerialUSB.println(lon);
    String http_str = "AT+HTTPPARA=\"URL\",\"https://fra1.blynk.cloud/external/api/batch/update?token=" + token + "&V9=" + lon + "&V9=" + lat + "\"\r\n";
    SerialUSB.println(http_str);
    sendData("AT+HTTPINIT\r\n", 3000, DEBUG);
    sendData(http_str, 3000, DEBUG);
    sendData("AT+HTTPACTION=0\r\n", 3000, DEBUG);
    sendData("AT+HTTPTERM\r\n", 3000, DEBUG);
}

void lattitude_update(String lat1)
{
    String lat = conversion(lat1);

    SerialUSB.print("lattitude = ");SerialUSB.println(lat);
    String http_str = "AT+HTTPPARA=\"URL\",\"https://fra1.blynk.cloud/external/api/batch/update?token=" + token + "&V6=" + lat + "\"\r\n";
    SerialUSB.println(http_str);
    sendData("AT+HTTPINIT\r\n", 3000, DEBUG);
    sendData(http_str, 3000, DEBUG);
    sendData("AT+HTTPACTION=0\r\n", 3000, DEBUG);
    sendData("AT+HTTPTERM\r\n", 3000, DEBUG);
}

void longitude_update(String lon1)
{
    String lon = conversion(lon1);

    SerialUSB.print("longitude = ");SerialUSB.println(lon);
    String http_str = "AT+HTTPPARA=\"URL\",\"https://fra1.blynk.cloud/external/api/batch/update?token=" + token + "&V5=" + lon + "\"\r\n";
    SerialUSB.println(http_str);
    sendData("AT+HTTPINIT\r\n", 3000, DEBUG);
    sendData(http_str, 3000, DEBUG);
    sendData("AT+HTTPACTION=0\r\n", 3000, DEBUG);
    sendData("AT+HTTPTERM\r\n", 3000, DEBUG);
}

void call(String lat1,String lon1)
{

  String lat = conversion(lat1);
  String lon = conversion(lon1);
  
  float latFloat = lat.toFloat();
  float lonFloat = lon.toFloat();

  // if(latFloat < 47.162724 && latFloat > 47.162600 && lonFloat < 27.605855 && lonFloat > 27.605700)
  // {'
    if(mai_mic(lat, "47.16272") == 1 && mai_mare(lat, "47.16260") == 1 && mai_mic(lon, "27.60585") == 1 && mai_mare(lon, "27.60570") == 1) {
    // if(mai_mic("47.16271", "47.16272") == 1 && mai_mare("47.16261", "47.16260") == 1 && mai_mic("27.60584", "27.60585") == 1 && mai_mare("27.60571", "27.60570") == 1) {      
    sendData("AT\r\n", 3000, DEBUG);
    sendData("AT+CMGF=1\r\n", 3000, DEBUG);
    sendData("AT+CSDVC=1\r\n", 3000, DEBUG);
    sendData("AT+CLVL=5\r\n", 3000, DEBUG);
    sendData("ATD+40746317049;\r\n", 3000, DEBUG);
    sendData("ATD+CHUP\r\n", 3000, DEBUG);  
  }
}

int mai_mic(String string1, String string2)
{
  for(int i =0; i < 8; i++ ) {
    if(string1[i] != '.')
    {
      if (string1[i] > string2[i])
        return 0;
    }
  }
  return 1;
}

int mai_mare(String string1, String string2)
{
  for(int i =0; i < 8; i++ ) {
    if(string1[i] != '.')
    {
      if (string1[i] < string2[i])
        return 0;
    }
  }
  return 1;
}

void setup(){
  SerialUSB.begin(115200);

  delay(100);

  Serial1.begin(115200);

  pinMode(LTE_RESET_PIN, OUTPUT);
  digitalWrite(LTE_RESET_PIN, LOW);

  pinMode(LTE_PWRKEY_PIN, OUTPUT);
  digitalWrite(LTE_RESET_PIN, LOW);
  delay(100);
  digitalWrite(LTE_PWRKEY_PIN, HIGH);
  delay(2000);
  digitalWrite(LTE_PWRKEY_PIN, LOW);

  pinMode(LTE_FLIGHT_PIN, OUTPUT);
  digitalWrite(LTE_FLIGHT_PIN, LOW);

  SerialUSB.println("Maduino Zero 4G Test Start!");

  SerialUSB.println(sendData("AT+CGMM\r\n", 3000, DEBUG));
  sendData("AT+CPIN?\r\n",3000,DEBUG);
  
  sendData("AT+COPS?\r\n",3000,DEBUG);

  sendData("AT+CNUM\r\n",3000,DEBUG);

  sendData("AT+CGPS=1",3000,DEBUG);
  delay(60000);

  
}

void loop(){

    if(check_signal()>=10)
    {
      loc = sendData("AT+CGPSINFO\r\n",3000,DEBUG);
      gpsLocation(loc);
      if(lattitude!=""&& longitude!="")
      {
        map_loc(lattitude,longitude);
        lattitude_update(lattitude);
        longitude_update(longitude);
        call(lattitude,longitude);
      }
      delay(5000);  
    }    
}