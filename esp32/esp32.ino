// Если кому в друг окажется полезным, оставляю ссылки на статьи которые читал при написании:
// https://randomnerdtutorials.com/esp32-ds18b20-temperature-arduino-ide/
// https://randomnerdtutorials.com/esp32-esp8266-web-server-http-authentication/
// https://alexgyver.ru/gyverpid/
// 
// By SVSD_VAL

#define STASSID "FiWii-SSID"
#define STAPSK  "Enter-YOUR-PASSSWORD-HERE"

#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
#endif

#include <Ticker.h>  //Ticker Library
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Esp32 online</title>
  
<style>
body {
  font-family: "Segoe UI", -apple-system, BlinkMacSystemFont, Roboto, Oxygen-Sans, Ubuntu, Cantarell, "Helvetica Neue", sans-serif;
  line-height: 1.1;
  color: #333;
  background-color: #fff;
  padding: 0 0;
}

table {
  margin: 0.1em 0;
  border-collapse: collapse;
  border: 0.1em solid #d7d7d7;
}

caption {
  text-align: left;
  font-style: italic;
  padding: 0.25em 0.5em 0.5em 0.5em;
}

.table td, .table th {
 padding: .75rem;
}

th, td {
  padding: 0.25em 0.5em 0.25em 1em;
  vertical-align: text-top;
  text-align: left;
  text-indent: -0.5em;
}

th {
  align: center;
  vertical-align: bottom;
  background-color: #777;
  color: #fff;
}

tr:nth-child(even) th[scope=row] { background-color: #f2f2f2; }
tr:nth-child(odd)  th[scope=row] { background-color: #fff; }
tr:nth-child(even) { background-color: rgba(0, 0, 0, 0.05); }
tr:nth-child(odd)  { background-color: rgba(255, 255, 255, 0.05); }
td:nth-of-type(2)  { font-style: italic; }
th:nth-of-type(3), td:nth-of-type(3) { text-align: right; }

th {  position: sticky;  top: 0;  z-index: 2;}
th[scope=row] {  position: sticky;  left: 0;  z-index: 1;}
th[scope=row] {
  vertical-align: top;
  color: inherit;
  background-color: inherit;
  background: linear-gradient(90deg, transparent 0%, transparent calc(100% - .05em), #d7d7d7 calc(100% - .05em), #d7d7d7 100%);
}

table:nth-of-type(2) th:not([scope=row]):first-child {
  left: 0;
  z-index: 3;
  background: linear-gradient(90deg, #777 0%, #777 calc(100% - .05em), #ccc calc(100% - .05em), #ccc 100%);
}

th[scope=row] + td { min-width: 24em; }
th[scope=row] { min-width: 20em; }

hr {
  display: block;
  margin-top: 0.1em;
  margin-bottom: 0.5em;
  margin-left: auto;
  margin-right: auto;
  border-style: inset;
  border-width: 5px;
}

body { padding-bottom: 50vh; }

.btn:not(:disabled):not(.disabled) { cursor: pointer; }
.btn-primary { color: #fff; background-color: #007bff; border: 2px solid #4CAF50; }
.btn-danger { background-color: #dc3545; color: #fff; border: 2px solid #f44336; }
.btn-danger:hover { background-color: #f44336; color: white; }
.btn-primary:hover { background-color: #00aaff; color: white; }

.btn {
  display: inline-block;
  font-weight: 400;
  text-align: center;
  white-space: nowrap;
  vertical-align: middle;
  user-select: none;
  border: 1px solid transparent;
  padding: 0.375rem 0.75rem;
  font-size: 1rem;
  line-height: 1.5;
  border-radius: 0.25rem;
  transition: color .15s ease-in-out,background-color .15s ease-in-out,border-color .15s ease-in-out,box-shadow .15s ease-in-out;
}
.btn.disabled { opacity: 0.6; cursor: not-allowed; }

</style>  
</head>

<body>
<form method="post" action="/apply">
<div align='center'>
<h2>Управление уф-камерой</h2>
<table width=80% class="thead-light">
 <thead>
   <th>Описание</th>
   <th>Состояние</th>
 </thed>
 
 <tr>
   <td>Состояние установки</td>
   <td><a id="cur_state">N/A</a></td>
 </tr>
 <tr>
   <td>Текущий ШИМ</td>
   <td><a id="cur_pwm">N/A</a></td>
 </tr>
 <tr>
   <td>Кол-во авто-перезапусков</td>
   <td><a id="restart_cnt">N/A</a> / <a id="rst_time">N/A</a></td>
 </tr>
 <tr>
   <td>Текущее время работы</td>
   <td><a id="work_time">N/A</a></td>
 </tr>
 <tr>
   <td>Целевое время работы</td>
   <td><a id="target_worktime">N/A</a></td>
 </tr>
 <tr>
   <td>Дата выемки</td>
   <td><a id='date'>N/A</a></td>
 </tr>
 <tr>
   <td>Текущая температура</td>
   <td><a id="cur_temp">N/A</a></td>
 </tr>
 <tr>
   <td>Целевая температура</td>
   <td><a id="target_temp">N/A</a></td>
 </tr>
 <tr>
   <td>Минимальная температура</td>
   <td><a id="min_temp">N/A</a></td>
 </tr>
 <tr>
   <td>Максимальная температура</td>
   <td><a id="max_temp">N/A</a></td>
 </tr>
</table>
<h3>Управление:</h3>
<hr>
<table width=80% class="thead-light">
 <thead>
   <th>Описание</th>
   <th>Состояние</th>
 </thed>
 <tr>
   <td>Целевое время работы (в часах)</td>
   <td><input id="itarget_worktime" name="itarget_worktime" type=number step=any min=0 value="600"/></td>
 </tr>
 <tr>
   <td>Целевая температура</td>
   <td><input id="itarget_temp" name="itarget_temp" type=number step=any max=100 min=0 value="33.0"/></td>
 </tr>
 <tr>
   <td>Максимальная температура</td>
   <td><input id="imax_temp" name="imax_temp" type=number step=any max=100 min=0 value='33.3' /></td>
 </tr>
 <tr>
   <td>Минимальная температура</td>
   <td><input id="imin_temp" name="imin_temp" type=number step=any max=100 min=0 value='16.1' /></td>
 </tr>
 <tr>
   <td>Дата выемки</td>
   <td><input id='idate' name='idate' type=date /></td>
 </tr>
</table>
<button id='start' name='btn-start' class="btn btn-primary" type="submit">Запустить</button>
<button id='stop' name='btn-stop' class="btn btn-danger" type="submit">Остановить</button>
</div>
</form>


<script>
function update( ) {
  var xhttp = new XMLHttpRequest();
  console.log('2');
  xhttp.onreadystatechange = function() {
    console.log('3');

   if (this.readyState == 4 && this.status == 200) {
      console.log('4');
      let json = this.responseText.replaceAll(/'/g, '"');
      console.log('return\n' + json);
      const obj = JSON.parse(json);

      let symb="☒";
      if ( parseInt(obj.cur_state) == 1 ){
        symb="☑";
      } else if ( parseInt(obj.cur_state) < 0 ){
        symb="⚠";
      }

      document.getElementById("cur_state").innerHTML = symb;
      document.getElementById("cur_temp").innerHTML = obj.cur_temp;
      document.getElementById("work_time").innerHTML = obj.work_time;
      document.getElementById("restart_cnt").innerHTML = obj.restart_cnt;
      document.getElementById("cur_pwm").innerHTML = obj.cur_pwm;

      document.getElementById("max_temp").innerHTML = obj.max_temp;
      document.getElementById("min_temp").innerHTML = obj.min_temp;
      document.getElementById("target_temp").innerHTML = obj.target_temp;
      document.getElementById("target_worktime").innerHTML = obj.target_worktime;
      document.getElementById("date").innerHTML = obj.date;
      document.getElementById("work_time").innerHTML = new Date( obj.work_time * 1000).toISOString().substring(11, 19);
      document.getElementById("rst_time").innerHTML = new Date( obj.rst_time * 1000).toISOString().substring(11, 19);
    }
  };
  xhttp.open("GET", "/get_stat", true);
  xhttp.send();
}

console.log('1');
update();
setInterval(update, 1000) ;
</script>
</body>
</html>
)rawliteral";


const char* http_username = "admin";
const char* http_password = "admin123";

String temperatureC = "";

// Replace with your network credentials
const char* ssid = STASSID;
const char* password = STAPSK;

short cur_state = 0;

uint64_t work_time = 0;
unsigned long target_worktime=0;

unsigned long restart_cnt = 0;

float cur_temp = 0.0f;
float max_temp=16.1f;
float min_temp=10.0f;
float target_temp=15.3f;

String last_date = "...";

int cur_pwm=0;
// Timer variables
unsigned long last_sensor_time = 0;  
unsigned long last_halt_time = 0;
unsigned long last_pid_time = 0;
// Время (мс) которе должно пройти после сбоя (cur_state = -1 ) что бы попытаться запустить повторно
#define restart_delay 600000
// Интервал (мс) считывания показания с датчика температуры
#define sensor_timer_delay 2000
// Интервал (мс) обработки PID'a 
#define pid_timer_delay 200
// Светодиод отображения рабочего состояния.
#define LED LED_BUILTIN
// Пин состояния работы
#define power_pin D8
// Пин ШИМ'а
#define pwm_pin 5
// Стандартный порт http сервера
#define http_port 80

AsyncWebServer server(http_port);

int computePID(float input, float setpoint, float kp, float ki, float kd, float dt, int minOut, int maxOut) {
  float err = setpoint - input;
  static float integral = 0, prevErr = 0;
  integral = constrain(integral + (float)err * dt * ki, minOut, maxOut);
  float D = (err - prevErr) / dt;
  prevErr = err;
  return constrain(err * kp + integral + D * kd, minOut, maxOut);
}
float readDSTemperatureC() {
  sensors.requestTemperatures(); 
  float tempC = sensors.getTempCByIndex(0);
  if(tempC == -127.00) {
    Serial.println("!!Failed to read from DS18B20 sensor!!");
  }
  Serial.print("Temperature Celsius: ");
  Serial.println(tempC); 
  return tempC;
}

// Формат строки возврата текущего состояния.

String formatJson(){
  char s[350];
  unsigned long rst_time = 0;
  if (cur_state== -1) {
    if ( millis() < last_halt_time ) {
      rst_time = (last_halt_time - millis()) / 1000;
    }
  }
  snprintf_P(s, sizeof(s), PSTR("{'cur_state' : %i, 'work_time' : %i, 'cur_temp' : %0.2f, 'restart_cnt' : %i, 'date' : '%s', 'max_temp' : %.2f, 'min_temp': %.2f, 'target_temp' : %.2f, 'target_worktime' : %i, 'cur_pwm' : %i, 'rst_time' : %i }"), cur_state,work_time,cur_temp,restart_cnt,last_date,max_temp,min_temp,target_temp,target_worktime,cur_pwm, rst_time );
  return String(s);
}


Ticker blinker;

void onTimer(){
if ( cur_state ){
    digitalWrite(LED, !digitalRead(LED));
    // Добавляем 1 секунду к работе
    work_time+=1;
  }
}

void wshutdown(int state=0){
  Serial.println("shutdown");
  cur_state= state;
  cur_pwm=0;
  analogWrite(pwm_pin, cur_pwm);
  digitalWrite(LED, 1);  
}

void wstartup(){
  cur_state   = 1;
  restart_cnt = 0;
  cur_pwm = 0;
  work_time = 0;
  analogWrite(pwm_pin, cur_pwm);
  digitalWrite(LED, 0);
  
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();
  pinMode(power_pin  , OUTPUT);
  pinMode(pwm_pin    , OUTPUT);
  pinMode(LED        , OUTPUT);
  digitalWrite(LED   , 1);
  
  // Start up the DS18B20 library
  sensors.begin();
  cur_temp = readDSTemperatureC();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  server.on("/temperaturec", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperatureC.c_str());
  });
  server.on("/get_stat", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "Content-type:application/json;charset=utf-8",  formatJson().c_str());
  });

  server.on("/apply", HTTP_POST, [](AsyncWebServerRequest *request){
    if(!request->authenticate(http_username, http_password))
    {
      Serial.print("request Authentication != Login:");
      Serial.print(http_username);
      Serial.print(" password:");
      Serial.print(http_password);
      return request->requestAuthentication();
    }
          
    int paramsNr = request->params();
    Serial.println(paramsNr);
    int ishalt=0;
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
        if ( p->name() == "btn-stop" ){
          wshutdown(0);
          ishalt=1;
          Serial.println('!Stop');
        }
    }

    if (ishalt==0)
    {
      for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
        Serial.print(p->name());
        Serial.print("->");
        Serial.println(p->value());
        Serial.println();
        
        if ( p->name() == "itarget_worktime" ){
          Serial.print("setup target_worktime = ");
          target_worktime = p->value().toInt();
          Serial.println(target_worktime);
        } else
        if ( p->name() == "itarget_temp" ){
          Serial.print("setup target_temp = ");
          target_temp = p->value().toFloat();
          Serial.println(target_temp);
        } else
        if ( p->name() == "imin_temp" ){
          Serial.print("setup min_temp = ");
          min_temp = p->value().toFloat();
          Serial.println(min_temp);
        } else
        if ( p->name() == "imax_temp" ){
          Serial.print("setup max_temp = ");
          max_temp = p->value().toFloat();
          Serial.println(max_temp);
        } else
        if ( p->name() == "idate" ){
          Serial.print("setup date = ");
          last_date = p->value();
          Serial.println(last_date);
        } else
        if ( p->name() == "btn-start" ){
          Serial.println("starting up");
          wstartup();
        };
      }
    };

    request->redirect("/");
  });

  server.on("/setpin", HTTP_GET, [](AsyncWebServerRequest *request){
    
    int paramsNr = request->params();
    Serial.println(paramsNr);
 
    for(int i=0;i<paramsNr;i++){
 
        AsyncWebParameter* p = request->getParam(i);
        Serial.print(p->name());
        Serial.print("->");
        Serial.println(p->value());
        Serial.println();
        Serial.print("int value: ");
        int x = p->value().toInt();
        Serial.println(x);

        pinMode(x, OUTPUT);  
        if ( p->name() == "pin_on" ){
         Serial.print("set pin on");
         digitalWrite( x , HIGH);
        };
        if ( p->name() == "pin_off" ){
         Serial.print("set pin off");
         digitalWrite( x , LOW);
        };
    }
    
    //request->send_P(200, "text/plain", "OK");
    request->redirect("/");
  });
  
  // Start server
  server.begin();

  pinMode(LED, OUTPUT);
  blinker.attach(1, onTimer);
}
 
void loop(){
  // Получение температуры
  if ( millis() > last_sensor_time ) {
    cur_temp = readDSTemperatureC();
    temperatureC = String(cur_temp);
    last_sensor_time = millis() + sensor_timer_delay;
  }
  // Расчёт PID
  if ( millis() > last_pid_time ) {
    if ( cur_state == 1 ) {
      unsigned long pidTime= millis() - last_pid_time;
      cur_pwm = computePID(  cur_temp , target_temp , 1.0, 2.0, 3.0, pidTime * 0.01f , 0, 1024 ) ;
      analogWrite(pwm_pin , cur_pwm);
    }
    last_pid_time = millis() + pid_timer_delay;
  }
  
  if ( cur_state == 1 )
  {
    // Проверка на минимальную температуру
    if (( cur_temp > max_temp ) || ( cur_temp < min_temp ))
    {
      wshutdown( -1 );
      last_halt_time=millis() + restart_delay;
    }
    // Проверка на время
    if (work_time > target_worktime * 3600)
    {
      wshutdown( 0 );
      last_halt_time=millis() + restart_delay;
    }

  }
  // Перезапуск по времени.
  if ( cur_state == -1 ){
    if (millis() > last_halt_time){
      cur_state=1;
      restart_cnt+=1;
    }
    
  }
  // Указание текущего состояния пина работы.
  if ( cur_state == 1 ){
     digitalWrite(power_pin, 1);
  } else
  {
     digitalWrite(power_pin, 0);
  }
  
}
