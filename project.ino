#include <SPI.h>
#include <Wire.h>
#include <math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>


#include <time.h>

#define OLED_RESET LED_BUILTIN  //4

Adafruit_SSD1306 display(OLED_RESET);

int arrive=1, mod = 1,submod, var = 0, buz = 0;
double vel,lim = 3.0, num = 306.0;
String lims, unit = "km/h";
unsigned long start,finish,elapsed,lastInterrupt ;

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  pinMode(D7, INPUT);
  pinMode(D6, INPUT);
  pinMode(D5, OUTPUT);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  
  attachInterrupt(0, change1, FALLING);
  attachInterrupt(D4, change2, FALLING);

  digitalWrite(D5, 0);

  display.clearDisplay();
  display.setFont();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  
  display.setCursor(5,0);
  display.println("   --->\n    <---\n");
  display.setCursor(5,39);
  display.print("WN PROJECT");
  display.display();
  delay(4000);

  varial();
  
}

void loop() {
  if(mod == 1) {
    mod1();
  }
  else if(mod == 2) {
    mod2();
  }
}

void mod1() {
  if(var == 1) {
    varial();
    var =0;
  }
  arrive = digitalRead(D7);
  //Serial.println(digitalRead(D6));
  //Serial.println(arrive);
  if(arrive == 0) {
    start = millis();
    Serial.println("-> start");
    callback();
    Serial.println("ended");
    delay(700);
  }

  arrive = digitalRead(D6);
  if(arrive == 0) {
    start = millis();
    Serial.println("<- start");
    callback2();
    Serial.println("ended");
    delay(700);
  }
}

void mod2() {
  if(var == 1){
    display.clearDisplay();
    display.setFont();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("Set limit:");
    display.setFont(&FreeSans24pt7b);
    display.setCursor(0,55);
    lims = String(lim,1);
    if(lims.length() == 3) {
        lims = "0" + lims;
      }
    display.print(lims);
    display.setFont(&FreeSans9pt7b);
    display.setCursor(90,55);
    display.print(unit);
    display.display();
    var = 2;
  }
  if(var == 2) {
      switch (submod) {
      case 1:
        Serial.println("submod 1");
        display.setFont(&FreeSans24pt7b);
        display.setCursor(0,10);
        display.print("_");
        display.display();
        break;
      case 2:
        Serial.println("submod 2");
        display.fillRect(0 ,17 , 27, 2, BLACK);
        display.setCursor(27,10);
        display.print("_");
        display.display();
        break;
      case 3:
        Serial.println("submod 3");
        display.fillRect(26 ,17 , 28, 2, BLACK);
        display.setCursor(63,10);
        display.print("_");
        display.display();
        break;
      case 4:
        Serial.println("submod 4");
        display.fillRect(62 ,17 , 28, 2, BLACK);
        display.setCursor(95,10);
        display.print("_");
        display.display();
        break;
    }
    var = 0;
  }

  if(var == 3) {
    if(submod < 4) {
      display.fillRect(0,20,90,37, BLACK);
      display.setCursor(0,55);
      lims = String(lim,1);
      if(lims.length() == 3) {
        lims = "0" + lims;
      }
      display.print(lims);
      display.display();
    }
    else {
      display.fillRect(90,25,40,33, BLACK);
      display.setFont(&FreeSans9pt7b);
      display.setCursor(90,55);
      display.print(unit);
      display.display();
    }
    var = 0;
  }
  
}

void callback() {
  while(digitalRead(D6)!=0) {
  }
  Serial.println("callback");
  finish = millis();
  elapsed =  finish - start;
  if(elapsed == 0) {
    elapsed = 1;
  }
  vel = num/elapsed;  //dist/time
  
  Serial.println(elapsed);
  Serial.println(vel);
  buzz();
  display.clearDisplay();
  display.setCursor(5,5);
  display.setFont();
  display.setTextSize(2);
  display.println("   <---");
  display.setFont(&FreeSans24pt7b);
  display.setTextSize(1);
  display.setCursor(0,55);
  lims = String(vel,1);
    if(lims.length() == 3) {
        lims = "0" + lims;
      }
  display.println(lims);
  display.setFont(&FreeSans9pt7b);
  display.setCursor(90,55);
  display.print(unit);
  display.display();
 // Serial.println("elapsed: "+elapsed+"speed: "+vel+unit);
}

void callback2() {
  while(digitalRead(D7)!=0) {
  }
  Serial.println("callback2");
  finish = millis();
  elapsed =  finish - start;
  if(elapsed == 0) {
    elapsed = 1;
  }
  vel = num/elapsed;  //dist/time
  Serial.println(elapsed);
  Serial.println(vel);
  buzz();
  display.clearDisplay();
  display.setCursor(5,5);
  display.setFont();
  display.setTextSize(2);
  display.println("   --->");
  display.setFont(&FreeSans24pt7b);
  display.setTextSize(1);
  display.setCursor(0,55);
  lims = String(vel,1);
    if(lims.length() == 3) {
        lims = "0" + lims;
      }
  display.println(lims);
  display.setFont(&FreeSans9pt7b);
  display.setCursor(90,55);
  display.print(unit);
  display.display();
//  Serial.println("elapsed: "+elapsed+"speed: "+vel+unit);
}



void buzz() {
  if(vel>lim) {
    digitalWrite(D5, 1);
    buz = 1;
  }
}

void change2() {
 if(millis() - lastInterrupt > 370)
 {
  if(mod == 1) {
    digitalWrite(D5, 0);
    buz = 0;
  }
  else {
    if(submod<4){
      double now = pow(10,2-submod);
      lim = lim+now;
      if(lim >= 100) {
        lim -= 100;
      }
    }
    else {
      if (unit == "km/h") {
        unit = "m/s";
        num = 85.0;
      }
      else {
        unit = "km/h";
        num = 306.0;
      }
    }
    var = 3;
  }
  lastInterrupt = millis();
 }
}

void change1() {
  
  if(millis() - lastInterrupt > 400)
    {
    if(buz == 1) {
      digitalWrite(D5,0);
      buz = 0;
    }
    else if(mod == 1) {
      Serial.println("change1 submod"+submod);
      mod = 2;
      var = 1;
      submod = 1;
    }
    else if(mod == 2 && submod < 4) {
      submod++;
      var = 2;
      Serial.println(" change1 submod"+submod);
    }
    else {
      var = 1;
      mod = 1;
    }
    lastInterrupt = millis();
    }
}

void varial() {
  display.clearDisplay();
  display.setCursor(5,5);
  display.setFont();
  display.setTextSize(2);
  display.println("   ----");
  display.setFont(&FreeSans24pt7b);
  display.setTextSize(1);
  display.setCursor(0,55);
  display.println("00.0");
  display.setFont(&FreeSans9pt7b);
  display.setCursor(90,55);
  display.print(unit);
  display.display();
}
