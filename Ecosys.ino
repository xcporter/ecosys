#include <Time.h>
#include <TimeLib.h>

#include <DHT.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN A0
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
//pin setup
const int keypad = A1;
const int heat = 7;
const int fan = 9;
const int light = 11;

//variables
bool heatactive = true;
bool lightactive = false;
bool fanactive = false;
int kpin = 0;
bool fanmode = false;
int fanduty = 175;
int lightduty = 50;
int lighttime = 13;
int sunrise = 6;
int lightfade = 20;
float currenttemp;
int targettemp = 81;
int currenthum = 0;
int targethum = 70;
int minT = 200;
int maxT = 0;
int screenState = 0;
int cursorState = 0;
int analogBtn = 0;
int setHour = 0;
int setMin = 0;
char keyPress;
char lastKeyValue;
long debounceDelay = 40;

// Functions
//    *hardware input
char readkeypad() {
  char k;
  int kpin;
  kpin = analogRead(keypad);
  if (kpin <= 30) {
    k = 'U';
  }
  else if (kpin >= 30 && kpin <= 55) {
    k = 'B';
  }
  else if  (kpin >= 55 && kpin <= 80) {
    k = 'D';
  }
  else if (kpin >= 80 && kpin <= 100) {
    k = 'E';
  }
  return k;
}

void acceptInput() {
  delay(10);
  lastKeyValue = keyPress;
  keyPress = readkeypad();
}

//    *display/interface
void displaySimple() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.cp437(true);
  display.setCursor(0, 0);
  display.println("Temp:");
  display.println(String(currenttemp, 2) + " F");
  display.println("Humidity:");
  display.println(String(currenthum) + "%");
  display.display();
}

void displayMain() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.cp437(true);
  display.setCursor(0, 0);

  display.print("Temp:" + String(currenttemp, 1) + "F");
  display.setCursor(69, 0);
  display.println("Humid:" + String(currenthum) + "%");
  display.drawLine(0, 12, display.width() - 1, 12, WHITE);

  display.setCursor(0, 18);
  display.println("Target:");
  display.setCursor(0, 29);
  display.println("Temp: " + String(targettemp) + "F");
  display.setCursor(0, 40);
  display.println("Hum: " + String(targethum) + "%");

  display.setCursor(64, 22);
  display.println("max: " + String(maxT));
  display.setCursor(64, 32);
  display.println("min: " + String(minT));
  display.setCursor(20, 52);
  display.println("menu");
  display.setCursor(74, 52);
  display.println("reset");
  display.display();
}

void displayMenu(int whichMenu) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.drawLine(0, 12, display.width() - 1, 12, WHITE);
  display.setCursor(100, 0);
  display.println("--->");
  display.setCursor(0, 2);
  if (whichMenu == 1) {
    display.println("MENU:");

    display.setCursor(0, 18);
    display.println("Heat is active: " + String(heatactive));
    display.setCursor(0, 31);
    display.println("Set current time:");
    display.setTextSize(2);
    display.setCursor(0, 45);
    display.println(String(setHour) + ":" + String(setMin));
    display.setCursor(60, 45);
    if (minute() < 10) {
      display.println(String(hour()) + ":0" + String(minute()));
    } else {
      display.println(String(hour()) + ":" + String(minute()));
    }
  } else if (whichMenu == 2) {
    display.println("LIGHT:");

    display.setCursor(0, 16);
    display.println("is active: " + String(lightactive));
    display.setCursor(0, 25);
    display.println("duty: " + String(lightduty));
    display.setCursor(0, 35);
    display.println("sunrise: " + String(sunrise));
    display.setCursor(0, 45);
    display.println("day length:" + String(lighttime) + " hours");
    display.setCursor(0, 55);
    display.println("fade:" + String(lightfade) + " minutes");

  } else if (whichMenu == 3) {
    display.println("FAN:");

    display.setCursor(0, 16);
    display.println("is active: " + String(fanactive));
    display.setCursor(0, 26);
    display.println("duty: " + String(fanduty));
    display.setCursor(0, 36);
    display.println("mode: " + String(fanmode));
  }
  display.display();

}

void drawCursor(int screen, int item) {
  if (screen == 1) {
    if (item == 1) {
      //Target temp
      display.drawLine(0, 38, 24, 38, WHITE);
    }   else if (item == 2) {
      //Target Humidity
      display.drawLine(0, 49, 20, 49, WHITE);
    }   else if (item == 3) {
      //Menu
      display.drawLine(20, 63, 43, 63, WHITE);
    }   else if (item == 4) {
      //Reset
      display.drawLine(72, 63, 102, 63, WHITE);
    }   else if (item == 5) {
      //Set Temp
      display.drawLine(35, 38, 50, 38, WHITE);
    }   else if (item == 6) {
      //Set Humidity
      display.drawLine(29, 49, 46, 49, WHITE);
    }
  } else if (screen == 2) {
    if (item == -1) {
      //next menu
      display.drawLine(100, 8, 115, 8, WHITE);
    } else if (item == 1) {
      //heat is active
      display.drawLine(0, 26, 87, 26, WHITE);
    } else if (item == 2) {
      //current time
      display.drawLine(0, 40, 100, 40, WHITE);
    } else if (item == 3) {
      // set* heat is active
      display.drawLine(95, 26, 101, 26, WHITE);
    } else if (item == 4) {
      //set* current time hour
      display.drawLine(0, 60, 18, 60, WHITE);
    } else if (item == 5) {
      //set* current time min
      display.drawLine(33, 60, 45, 60, WHITE);
    }
  } else if (screen == 3) {
    if (item == -1) {
      //next menu
      display.drawLine(100, 8, 115, 8, WHITE);
    } else if (item == 1) {
      //light is active
      display.drawLine(0, 24, 56, 24, WHITE);
    } else if (item == 2) {
      //duty
      display.drawLine(0, 34, 26, 34, WHITE);
    } else if (item == 3) {
      //sunrise
      display.drawLine(0, 43, 42, 43, WHITE);
    } else if (item == 4) {
      //daylength
      display.drawLine(0, 54, 62, 54, WHITE);
    } else if (item == 5) {
      //fadeout
      display.drawLine(0, 63, 26, 63, WHITE);
    } else if (item == 6) {
      //set* light is active
      display.drawLine(65, 24, 71, 24, WHITE);
    } else if (item == 7) {
      //set* duty
      display.drawLine(36, 34, 48, 34, WHITE);
    } else if (item == 8) {
      //set* sunrise
      display.drawLine(53, 43, 59, 43, WHITE);
    } else if (item == 9) {
      //set* daylength
      display.drawLine(66, 54, 105, 54, WHITE);
    } else if (item == 10) {
      //set* fadeout
      display.drawLine(31, 63, 85, 63, WHITE);
    }

  } else if (screen == 4) {
    if (item == -1) {
      //next menu
      display.drawLine(100, 8, 115, 8, WHITE);
    } else if (item == 1) {
      //fan is active
      display.drawLine(0, 25, 55, 25, WHITE);
    } else if (item == 2) {
      //duty
      display.drawLine(0, 36, 28, 36, WHITE);
    } else if (item == 3) {
      //fan mode
      display.drawLine(0, 46, 28, 46, WHITE);
    } else if (item == 4) {
      //set* fan is active
      display.drawLine(65, 25, 70, 25, WHITE);
    } else if (item == 5) {
      //set* duty
      display.drawLine(32, 36, 42, 36, WHITE);
    } else if (item == 6) {
      //set* fan mode
      display.drawLine(32, 46, 42, 46, WHITE);
    }
  }
  display.display();
}

void tempWarning() {
  display.setCursor(104, 18);
  display.setTextSize(4);
  display.println("!");
  display.display();
}

void interface() {
  if (screenState < 2) {
    if (keyPress == 'B' && lastKeyValue == 'B' && screenState > 0) {
      screenState--;
    } else if (keyPress == 'E') {
      screenState++;
    }
  } else if (screenState == 2) {
    if (keyPress == 'B') {
      cursorState = 0;
      displayMain();
      if (lastKeyValue == 'B') {
        screenState--;
      }
    } else if (cursorState >= 0 && cursorState < 4) {
      if (keyPress == 'D') {
        cursorState++;
      } else if (keyPress == 'U') {
        cursorState--;
      } else if (keyPress == 'E') {
        if (cursorState == 1) {
          cursorState = 5;
        } else if (cursorState == 2) {
          cursorState = 6;
        } else if (cursorState == 3) {
          screenState = 3;
          cursorState = 0;
        }
      }
    } else if (cursorState == 4 && keyPress == 'E') {
      minT = 200;
      maxT = 0;
      cursorState = 0;
      fanactive = false;
      fanduty = 175;
    } else if (cursorState == 5) {
      if (keyPress == 'D') {
        targettemp--;
      } else if (keyPress == 'U') {
        targettemp++;
      }
    } else if (cursorState == 6) {
      if (keyPress == 'D') {
        targethum--;
      } else if (keyPress == 'U') {
        targethum++;
      }
    }
  } else if (screenState == 3) {
    if (keyPress == 'B') {
      cursorState = 0;
      if (lastKeyValue == 'B') {
        screenState--;
      }
    } else if (keyPress == 'E') {
      if (cursorState == 0) {
        cursorState = -1;
      } else if (cursorState == -1) {
        screenState++;
      } else if (cursorState == 1) {
        cursorState = 3;
      } else if (cursorState == 2) {
        cursorState = 4;
      } else if (cursorState == 4) {
        cursorState = 5;
      } else if (cursorState == 5) {
        setTime(setHour, setMin, 0, 0, 0, 0);
        cursorState = 0;
        setHour = 0;
        setMin = 0;
      }
    } else if (keyPress == 'D' && cursorState < 2) {
      cursorState++;
    } else if (keyPress == 'U' && cursorState > 0 && cursorState < 3) {
      cursorState--;
    } else if (cursorState == 3) {
      if (keyPress == 'U' || keyPress == 'D') {
        heatactive = !heatactive;
      }
    } else if (cursorState == 4) {
      if (keyPress == 'D') {
        if (setHour > 0) {
          setHour--;
        } else {
          setHour = 23;
        }
      } else if (keyPress == 'U') {
        if (setHour < 23) {
          setHour++;
        } else {
          setHour = 0;
        }
      }
    } else if (cursorState == 5) {
      if (keyPress == 'D') {
        if (setMin > 0) {
          setMin--;
        } else {
          setMin = 59;
        }
      } else if (keyPress == 'U') {
        if (setMin < 59) {
          setMin++;
        } else {
          setMin = 0;
        }
      }
    }
  } else if (screenState == 4) {
    if (keyPress == 'B') {
      cursorState = 0;
      if (lastKeyValue == 'B') {
        screenState--;
      }
    } else if (keyPress == 'E') {
      if (cursorState == 0) {
        cursorState = -1;
      } else if (cursorState == -1) {
        screenState++;
      } else if (cursorState == 1) {
        cursorState = 6;
      } else if (cursorState == 2) {
        cursorState = 7;
        keyPress = ' ';
      } else if (cursorState == 3) {
        cursorState = 8;
        keyPress = ' ';
      } else if (cursorState == 4) {
        cursorState = 9;
        keyPress = ' ';
      } else if (cursorState == 5) {
        cursorState = 10;
        keyPress = ' ';
      }
    } else if (keyPress == 'D' && cursorState < 5) {
      cursorState++;
    } else if (keyPress == 'U' && cursorState > 0 && cursorState < 6) {
      cursorState--;
    } else if (cursorState == 6) {
      if (keyPress == 'U' || keyPress == 'D') {
        lightactive = !lightactive;
      }
    } else if (cursorState == 7) {
      if (keyPress == 'D' && lightduty > 0) {
        lightduty = lightduty - 5 ;
      } else if (keyPress == 'U' && lightduty < 255) {
        lightduty = lightduty + 5;
      }
    } else if (cursorState == 8) {
      if (keyPress == 'D' && sunrise > 0) {
        sunrise--;
      } else if (keyPress == 'U' && sunrise < 24) {
        sunrise++;
      }
    } else if (cursorState == 9) {
      if (keyPress == 'D') {
        lighttime--;
      } else if (keyPress == 'U') {
        lighttime++;
      }
    } else if (cursorState == 10) {
      if (keyPress == 'D') {
        lightfade--;
      } else if (keyPress == 'U') {
        lightfade++;
      }
    }
  } else if (screenState == 5) {
    if (keyPress == 'B') {
      cursorState = 0;
      if (lastKeyValue == 'B') {
        screenState--;
      }
    } else if (keyPress == 'E') {
      if (cursorState == 0) {
        cursorState = -1;
      } else if (cursorState == -1) {
        screenState = 2;
      } else if (cursorState == 1) {
        cursorState = 4;
      } else if (cursorState == 2) {
        cursorState = 5;
      } else if (cursorState == 3) {
        cursorState = 6;
      }
    } else if (keyPress == 'D' && cursorState < 3) {
      cursorState++;
    } else if (keyPress == 'U' && cursorState > 0 && cursorState < 4) {
      cursorState--;
    } else if (cursorState == 4) {
      if (keyPress == 'U' || keyPress == 'D') {
        fanactive = !fanactive;
      }
    } else if (cursorState == 5) {
      if (keyPress == 'D' && fanduty > 0) {
        fanduty = fanduty - 5;
      } else if (keyPress == 'U' && fanduty < 255) {
        fanduty = fanduty + 5;
      }
    } else if (cursorState == 6) {
      if (keyPress == 'U' || keyPress == 'D') {
        fanmode = !fanmode;
      }
    }
  }
}

//Execute
void setup() {
  pinMode(keypad, INPUT_PULLUP);
  pinMode(heat, OUTPUT);
  pinMode(light, OUTPUT);
  pinMode(fan, OUTPUT);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  dht.begin();
  display.clearDisplay();
}

void loop() {
  //Read Sensors
  currenttemp = (((dht.readTemperature()) * 9) / 5) + 32;
  currenthum = dht.readHumidity();

  Serial.print(currenttemp);
  Serial.println(" degrees");
  Serial.print(currenthum);
  Serial.println(" humid");

  //Establish max and min
  if (currenttemp > maxT) {
    maxT = int(currenttemp);
  }
  if (currenttemp < minT) {
    minT = int(currenttemp);
  }

  //Hardware Control
  if (heatactive) {
    if (currenttemp <= targettemp - 2) {
      digitalWrite(heat, HIGH);
    } else if (currenttemp >= targettemp) {
      digitalWrite(heat, LOW);
    }
  } else {
    digitalWrite(heat, LOW);
  }
//timing triggers and fade in ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (lightactive) {
    if (hour() == sunrise) {
      for (int i = lightfade; i >= 0; i--) {
        if(minute() == lightfade - i) {
          analogWrite(light, (int(lightduty/lightfade))*(lightfade - i));
        }
      }
    } else if (hour() == ((sunrise+lighttime)%23)) {
      for (int i = lightfade; i >= 0; i--) {
        if(minute() == lightfade - i) {
          analogWrite(light, (int(lightduty/lightfade)*i));
        }
      }
    }
  } else {
    analogWrite(light, 0);
    }
  
  if (fanactive) {
      if (fanmode) {
        if (currenthum > targethum) {
          analogWrite(fan, fanduty);
        } else {
          analogWrite(fan, 0);
        }      
      } else {
        analogWrite(fan, fanduty);
      }
  } else {
    analogWrite(fan, 0);
  }

  //Overheating Evacuation and Shutdown Routine
  if (maxT > 95) {
    tempWarning();
    heatactive = false;
    lightactive = false;
    fanactive = true;
    fanduty = 255;
    fanmode = false;
  } 

  //Initialize interface on Keypress
  if (analogRead(keypad) < 100) {
    acceptInput();
    interface();
  }

  //Display Screen Changes
  if (screenState == 0) {
    display.clearDisplay();
    display.display();
  } else if (screenState == 1) {
    displaySimple();
  } else if (screenState == 2) {
    displayMain();
  } else if (screenState == 3) {
    displayMenu(1);
  } else if (screenState == 4) {
    displayMenu(2);
  } else if (screenState == 5) {
    displayMenu(3);
  }

  //Display Cursor
  if (screenState == 2) {
    if (cursorState == 0) {
      displayMain();
    } else {
      drawCursor(1, cursorState);
    }
  } else if (screenState == 3) {
    if (cursorState == 0) {
      displayMenu(1);
    } else {
      drawCursor(2, cursorState);
    }
  } else if (screenState == 4) {
    if (cursorState == 0) {
      displayMenu(2);
    } else {
      drawCursor(3, cursorState);
    }
  } else if (screenState == 5) {
    if (cursorState == 0) {
      displayMenu(3);
    } else {
      drawCursor(4, cursorState);
    }
  }
}
