#include <WiFi.h>
#include <HTTPClient.h>
#include "display.h"
#include "IODriver.h"

const char* ssid = "aeriane";
const char* password = "gembloux5030";



HTTPClient http;


void startWebMonitoring() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://four.aeriane.com/start_monitoring");
    // http.begin("http://aeriane.pythonanywhere.com/start_monitoring");


    int httpResponseCode = http.POST("");

    if (httpResponseCode > 0) {
      Serial.println("Monitoring started on server");
    } else {
      Serial.print("Error starting monitoring: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}



void sendEmail(const String &errorMessage, const String &cookProfile, bool cookError) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // URL of the server endpoint
   http.begin("https://four.aeriane.com/send_email");
    // http.begin("http://aeriane.pythonanywhere.com/send_email");


    // Set the content type to application/json
    http.addHeader("Content-Type", "application/json");
    // Create JSON payload
    String jsonPayload = "{\"cook_error\": " + String(cookError ? "true" : "false") +
                     ", \"cook_profile\": \"" + cookProfile + "\"" +
                     ", \"error_message\": \"" + errorMessage + "\"}";

    Serial.println("JSON Payload:");
    Serial.println(jsonPayload);
    // Send POST request with JSON payload
    int httpResponseCode = http.POST(jsonPayload);

    // Handle the response
    if (httpResponseCode > 0) {
      Serial.print("Email request sent to server. Response code: ");
      Serial.println(httpResponseCode);
      Serial.println("Server response: " + http.getString());
    } else {
      Serial.print("Error sending email request. HTTP response code: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Close connection
  } else {
    Serial.println("WiFi not connected.");
  }
}


void sendProfilData(const String &profilName,
                    float target_temperature,
                    float measured_temperature,
                    int actual_phase,
                    int number_of_phase,
                    int profil_duration,
                    float T1_temp,
                    int T1_duration,
                    int nR1,
                    float T2_temp,
                    int T2_duration,
                    int nR2,
                    float T3_temp,
                    int T3_duration,
                    int nR3,
                    bool setPointReached)
{


  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin("https://four.aeriane.com/update_profile_data");
    // http.begin("http://aeriane.pythonanywhere.com/update_profile_data");

    http.addHeader("Content-Type", "application/json"); // Specify content type

    // Create JSON payload
    String jsonPayload = "{";
    jsonPayload += "\"profil_name\":\"" + profilName + "\",";
    jsonPayload += "\"target_temperature\":" + String(target_temperature) + ",";
    jsonPayload += "\"measured_temperature\":" + String(measured_temperature) + ",";
    jsonPayload += "\"actual_phase\":" + String(actual_phase) + ",";
    jsonPayload += "\"number_of_phase\":" + String(number_of_phase) + ",";
    jsonPayload += "\"profil_duration\":" + String(profil_duration) + ",";

    // Table Variables for each phase
    jsonPayload += "\"T1_temp\":" + String(T1_temp) + ",";
    jsonPayload += "\"T1_duration\":" + String(T1_duration) + ",";
    jsonPayload += "\"nR1\":" + String(nR1) + ",";
    jsonPayload += "\"T2_temp\":" + String(T2_temp) + ",";
    jsonPayload += "\"T2_duration\":" + String(T2_duration) + ",";
    jsonPayload += "\"nR2\":" + String(nR2) + ",";
    jsonPayload += "\"T3_temp\":" + String(T3_temp) + ",";
    jsonPayload += "\"T3_duration\":" + String(T3_duration) + ",";
    jsonPayload += "\"nR3\":" + String(nR3)+ ",";
    jsonPayload += "\"set_point_is_reached\":" + String(setPointReached ? "true" : "false");
    jsonPayload += "}";

    // Debugging: Print JSON payload
    Serial.println("Sending JSON payload:");
    Serial.println(jsonPayload);

    // Send POST request with JSON payload
    int httpResponseCode = http.POST(jsonPayload);

    // Check response code
    if (httpResponseCode > 0) {
      Serial.println("POST request sent successfully.");
      Serial.println(httpResponseCode); // HTTP response code
      Serial.println(http.getString()); // Server response for debugging
    } else {
      Serial.println("Error sending POST request.");
      Serial.println(httpResponseCode);
    }

    http.end(); // End HTTP request
  } else {
    Serial.println("WiFi not connected.");
  }
}

void initWifi() {
  WiFi.begin(ssid, password);
  lcd.clear();
  lcd.setCursor(0, 0); //
  lcd.print("WIFI ");
  lcd.setCursor(0, 1); //
  lcd.print("connecting ...");
  Serial.println("Connecting to WiFi...");
  delay(2000);
}


void ReconnectIfWifiDisconnected()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.reconnect();
        Serial.println("Reconnecting to WiFi...");
    }
}



void RssiDisplay() {
    int rssi = -100;

    if (WiFi.status() == WL_CONNECTED) {
        rssi = WiFi.RSSI();
    }

    // Set custom characters
    byte RfSymbol[] = {0x1F, 0x15, 0x0E, 0x04, 0x04, 0x04, 0x04, 0x04};
    byte Col1[8] = {0};
    byte Col2[8] = {0};

    if (rssi < -87 || rssi == 0) {
        byte tempCol1[] = {0x00, 0x00, 0x00, 0x11, 0x0A, 0x04, 0x0A, 0x11};
        byte tempCol2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        memcpy(Col1, tempCol1, 8);
        memcpy(Col2, tempCol2, 8);
    }
    else if (rssi < -80) {
        byte tempCol1[] = {0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10};
        byte tempCol2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        memcpy(Col1, tempCol1, 8);
        memcpy(Col2, tempCol2, 8);
    }
    else if (rssi < -70) {
        byte tempCol1[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x14, 0x14};
        byte tempCol2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        memcpy(Col1, tempCol1, 8);
        memcpy(Col2, tempCol2, 8);
    }
    else if (rssi < -67) {
        byte tempCol1[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x15, 0x15};
        byte tempCol2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        memcpy(Col1, tempCol1, 8);
        memcpy(Col2, tempCol2, 8);
    }
    else if (rssi < -60) {
        byte tempCol1[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x15, 0x15};
        byte tempCol2[] = {0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10};
        memcpy(Col1, tempCol1, 8);
        memcpy(Col2, tempCol2, 8);
    }
    else if (rssi < -50) {
        byte tempCol1[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x15, 0x15};
        byte tempCol2[] = {0x00, 0x00, 0x04, 0x14, 0x14, 0x14, 0x14, 0x14};
        memcpy(Col1, tempCol1, 8);
        memcpy(Col2, tempCol2, 8);
    }
    else {
        byte tempCol1[] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x15, 0x15};
        byte tempCol2[] = {0x01, 0x01, 0x05, 0x15, 0x15, 0x15, 0x15, 0x15};
        memcpy(Col1, tempCol1, 8);
        memcpy(Col2, tempCol2, 8);
    }

    // Create custom characters
    lcd.createChar(0, RfSymbol);
    lcd.createChar(1, Col1);
    lcd.createChar(2, Col2);


    lcd.setCursor(14, 1);
    lcd.write(0);

    lcd.setCursor(15, 1);
    lcd.write(1);

    lcd.setCursor(16, 1);
    lcd.write(2);
}


