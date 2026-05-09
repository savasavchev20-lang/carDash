// ============================================================
//  GPRS Functions — add these to OBD2_Complete.ino
//  
//  SETUP: Replace YOUR_SERVER_URL with your deployed server
//  e.g. "mycarapp.railway.app"
// ============================================================

#define GPRS_APN        "internet"        // Vivacom APN
#define SERVER_HOST     "YOUR_SERVER_URL" // e.g. mycarapp.railway.app
#define SERVER_PORT     80
#define GPRS_INTERVAL   3000              // send data every 3 seconds

bool gprsReady = false;
unsigned long lastGPRS = 0;

// ============================================================
//  Init GPRS — call once after initGSM() succeeds
// ============================================================
bool initGPRS() {
  String r;

  // Attach to GPRS
  r = gsmSend("AT+CGATT=1", 2000);

  // Set APN
  r = gsmSend("AT+CSTT=\"" GPRS_APN "\",\"\",\"\"", 2000);
  if (r.indexOf("OK") == -1) {
    Serial.println("GPRS APN failed");
    return false;
  }

  // Bring up wireless connection
  r = gsmSend("AT+CIICR", 5000);

  // Get IP address
  r = gsmSend("AT+CIFSR", 2000);
  Serial.print("GPRS IP: "); Serial.println(r);
  if (r.indexOf("ERROR") != -1 || r.length() < 5) {
    Serial.println("GPRS no IP");
    return false;
  }

  Serial.println("GPRS Ready!");
  return true;
}

// ============================================================
//  Send data to web dashboard via HTTP GET
// ============================================================
void sendGPRSUpdate(bool crash = false) {
  if (millis() - lastGPRS < GPRS_INTERVAL) return;
  lastGPRS = millis();

  // Build URL query string
  String url = "/update?";
  url += "rpm="  + String(rpm > 0  ? rpm  : 0);
  url += "&spd=" + String(speed_kph > 0 ? speed_kph : 0);
  url += "&cool="+ String(coolant > 0 ? coolant : 0);
  url += "&thr=" + String(throttle > 0 ? throttle : 0);
  url += "&vbat="+ String(voltage > 0  ? voltage  : 0.0, 1);
  url += "&lat=" + String(gpsLat, 6);
  url += "&lon=" + String(gpsLon, 6);
  url += "&gps=" + String(gpsValid  ? 1 : 0);
  url += "&gsm=" + String(gsmReady  ? 1 : 0);
  url += "&obd=" + String(elmReady  ? 1 : 0);
  url += "&crash="+ String(crash    ? 1 : 0);

  Serial.print("GPRS sending: "); Serial.println(url);

  // Open TCP connection
  String r = gsmSend("AT+CIPSTART=\"TCP\",\"" SERVER_HOST "\"," + String(SERVER_PORT), 5000);
  if (r.indexOf("CONNECT") == -1 && r.indexOf("OK") == -1) {
    Serial.println("GPRS connect failed");
    gsmSend("AT+CIPCLOSE", 1000);
    return;
  }
  delay(500);

  // Build HTTP request
  String req = "GET " + url + " HTTP/1.1\r\n";
  req += "Host: " SERVER_HOST "\r\n";
  req += "Connection: close\r\n\r\n";

  // Send data length first
  gsmSend(("AT+CIPSEND=" + String(req.length())).c_str(), 1000);
  delay(500);

  // Send the actual request
  Serial1.print(req);
  delay(2000);

  // Close connection
  gsmSend("AT+CIPCLOSE", 1000);
  Serial.println("GPRS update sent!");
}

// ============================================================
//  HOW TO INTEGRATE INTO YOUR MAIN CODE:
//
//  1. Add this to setup() after initGSM() succeeds:
//
//     if (gsmReady) {
//       lcd.setCursor(0, 2); lcd.print("Starting GPRS...    ");
//       gprsReady = initGPRS();
//       lcd.setCursor(0, 3);
//       lcd.print(gprsReady ? "GPRS Ready!         " : "GPRS Failed         ");
//       delay(1000);
//     }
//
//  2. Add this to loop() after OBD data is fetched:
//
//     if (gprsReady) sendGPRSUpdate();
//
//  3. In the crash detection block, change sendSMS to also send GPRS:
//
//     if (gsmReady) sendSMS(msg);
//     if (gprsReady) sendGPRSUpdate(true);  // crash=true
//
// ============================================================
