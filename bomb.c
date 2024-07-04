#include <LiquidCrystal.h>

// Pinos de conexão do LCD
const int buttonPin1 = 7;  
const int buttonPin2 = 8;  
const int buttonPin3 = 9;  
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;
unsigned long button1PressTime = 0;
unsigned long button2PressTime = 0;
unsigned long button1Elapsed = 0;
unsigned long button2Elapsed = 0;
unsigned long startTime;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

enum MenuState {
  MAIN_MENU,
  CONFIG_MENU,
  CONFIG_ARM_HOURS,
  CONFIG_ARM_MINUTES,
  CONFIG_ARM_SECONDS,
  CONFIG_DISARM_HOURS,
  CONFIG_DISARM_MINUTES,
  CONFIG_DISARM_SECONDS,
  CRONOMETRO_PARTIDA,
  ARMING_BOMB,
  DISARMING_BOMB,
  BOMB_ARMED,
  START_ARM_BOMB,
  START_DISARM_BOMB,
  CRONOMETRO_DESARME,
  CT_VENCEU,
  TR_VENCEU
};

MenuState currentState = MAIN_MENU;
int armHours = 0, armMinutes = 0, armSeconds = 10;
int disarmHours = 0, disarmMinutes = 0, disarmSeconds = 10;
int* currentSetting = &armHours;

byte fullBlock[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

void setup() {
  pinMode(buttonPin1, INPUT_PULLUP); 
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  lcd.begin(16, 2); 
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  lcd.createChar(0, fullBlock); // Cria o caractere personalizado para a barra de progresso

  showMainMenu();
}

void loop() {
  buttonState1 = digitalRead(buttonPin1); 
  buttonState2 = digitalRead(buttonPin2); 
  buttonState3 = digitalRead(buttonPin3); 

  switch (currentState) {
    case MAIN_MENU:
      if (buttonState1 == LOW) {
        currentState = CRONOMETRO_PARTIDA;
        startTime = millis();
        showCronometroPartida();
        delay(200); 
      }
      if (buttonState2 == LOW) {
        currentState = CONFIG_MENU;
        showConfigMenu();
        delay(200);
      }
      break;

    case CONFIG_MENU:
      if (buttonState1 == LOW) {
        currentState = CONFIG_ARM_HOURS;
        showConfigArmTime();
        delay(200);
      }
      if (buttonState2 == LOW) {
        currentState = CONFIG_DISARM_HOURS;
        showConfigDisarmTime();
        delay(200);
      }
      if (buttonState3 == LOW) {
        currentState = MAIN_MENU;
        showMainMenu();
        delay(200);
      }
      break;

    case CONFIG_ARM_HOURS:
    case CONFIG_ARM_MINUTES:
    case CONFIG_ARM_SECONDS:
      if (buttonState1 == LOW) {
        incrementTimeSetting();
        showConfigArmTime();
        delay(200);
      }
      if (buttonState2 == LOW) {
        decrementTimeSetting();
        showConfigArmTime();
        delay(200);
      }
      if (buttonState3 == LOW) {
        advanceArmConfigSetting();
        delay(200);
      }
      break;

    case CONFIG_DISARM_HOURS:
    case CONFIG_DISARM_MINUTES:
    case CONFIG_DISARM_SECONDS:
      if (buttonState1 == LOW) {
        incrementTimeSetting();
        showConfigDisarmTime();
        delay(200);
      }
      if (buttonState2 == LOW) {
        decrementTimeSetting();
        showConfigDisarmTime();
        delay(200);
      }
      if (buttonState3 == LOW) {
        advanceDisarmConfigSetting();
        delay(200);
      }
      break;

    case CRONOMETRO_PARTIDA:
      if (buttonState1 == LOW) {
        button1PressTime = millis();
        currentState = START_ARM_BOMB;
        showStartArmBomb();
        delay(200);
      } else {
        updateCronometroPartida();
      }
      break;

    case START_ARM_BOMB:
      if (buttonState1 == LOW) {
        if (button1PressTime == 0) {
          button1PressTime = millis();
        } else {
          button1Elapsed = millis() - button1PressTime;
          showProgress("Armando", button1Elapsed, 7000);
          if (button1Elapsed >= 7000) {
            currentState = ARMING_BOMB;
            armBomb();
          }
        }
      } else {
        button1PressTime = 0;
        currentState = CRONOMETRO_PARTIDA;
        showCronometroPartida();
      }
      break;

    case BOMB_ARMED:
      if (buttonState2 == LOW) {
        button2PressTime = millis();
        currentState = START_DISARM_BOMB;
        showStartDisarmBomb();
        delay(200);
      } else {
        updateCronometroDesarme();
      }
      break;

    case START_DISARM_BOMB:
      if (buttonState2 == LOW) {
        if (button2PressTime == 0) {
          button2PressTime = millis();
        } else {
          button2Elapsed = millis() - button2PressTime;
          showProgress("Desarmar", button2Elapsed, 10000);
          if (button2Elapsed >= 10000) {
            currentState = DISARMING_BOMB;
            disarmBomb();
          }
        }
      } else {
        button2PressTime = 0;
        currentState = BOMB_ARMED;
        showCronometroDesarme();
      }
      break;

    case CT_VENCEU:
      if (buttonState1 == LOW) {
        currentState = MAIN_MENU;
        showMainMenu();
        delay(200);
      }
      break;

    case TR_VENCEU:
      if (buttonState1 == LOW) {
        currentState = MAIN_MENU;
        showMainMenu();
        delay(200);
      }
      break;

    case ARMING_BOMB:
    case DISARMING_BOMB:
      // Nessas etapas, a função armBomb() ou disarmBomb() cuida da lógica
      break;
  }
}

void showMainMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1: Start Game");
  lcd.setCursor(0, 1);
  lcd.print("2: Config");
}

void showConfigMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1: Arm Time");
  lcd.setCursor(0, 1);
  lcd.print("2: Disarm Time");
}

void showConfigArmTime() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Arm Time");
  showCurrentArmTimeSetting();
}

void showConfigDisarmTime() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set Disarm Time");
  showCurrentDisarmTimeSetting();
}

void showCurrentArmTimeSetting() {
  lcd.setCursor(0, 1);
  lcd.print(armHours);
  lcd.print(":");
  lcd.print(armMinutes);
  lcd.print(":");
  lcd.print(armSeconds);
  lcd.print(" ");
}

void showCurrentDisarmTimeSetting() {
  lcd.setCursor(0, 1);
  lcd.print(disarmHours);
  lcd.print(":");
  lcd.print(disarmMinutes);
  lcd.print(":");
  lcd.print(disarmSeconds);
  lcd.print(" ");
}

void showCronometroPartida() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tempo Arm.");
  updateCronometroPartida();
}

void updateCronometroPartida() {
  unsigned long elapsedTime = millis() - startTime;
  int totalSeconds = (armHours * 3600) + (armMinutes * 60) + armSeconds - (elapsedTime / 1000);
  
  if (totalSeconds <= 0) {
    currentState = CT_VENCEU;
    showCTVenceu();
    return;
  }

  int displayHours = totalSeconds / 3600;
  int displayMinutes = (totalSeconds % 3600) / 60;
  int displaySeconds = totalSeconds % 60;

  lcd.setCursor(0, 1);
  if (displayHours < 10) lcd.print("0");
  lcd.print(displayHours);
  lcd.print(":");
  if (displayMinutes < 10) lcd.print("0");
  lcd.print(displayMinutes);
  lcd.print(":");
  if (displaySeconds < 10) lcd.print("0");
  lcd.print(displaySeconds);
}

void showStartArmBomb() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Armando: 00:07");
}

void showStartDisarmBomb() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Desarmar: 00:10");
}

void updateCronometroDesarme() {
  unsigned long elapsedTime = millis() - startTime;
  int totalSeconds = (disarmHours * 3600) + (disarmMinutes * 60) + disarmSeconds - (elapsedTime / 1000);

  if (totalSeconds <= 0) {
    currentState = TR_VENCEU;
    showTRVenceu();
    return;
  }

  int displayHours = totalSeconds / 3600;
  int displayMinutes = (totalSeconds % 3600) / 60;
  int displaySeconds = totalSeconds % 60;

  lcd.setCursor(0, 1);
  if (displayHours < 10) lcd.print("0");
  lcd.print(displayHours);
  lcd.print(":");
  if (displayMinutes < 10) lcd.print("0");
  lcd.print(displayMinutes);
  lcd.print(":");
  if (displaySeconds < 10) lcd.print("0");
  lcd.print(displaySeconds);
}

void incrementTimeSetting() {
  switch (currentState) {
    case CONFIG_ARM_HOURS:
      armHours++;
      break;
    case CONFIG_ARM_MINUTES:
      armMinutes++;
      break;
    case CONFIG_ARM_SECONDS:
      armSeconds++;
      break;
    case CONFIG_DISARM_HOURS:
      disarmHours++;
      break;
    case CONFIG_DISARM_MINUTES:
      disarmMinutes++;
      break;
    case CONFIG_DISARM_SECONDS:
      disarmSeconds++;
      break;
  }
}

void decrementTimeSetting() {
  switch (currentState) {
    case CONFIG_ARM_HOURS:
      if (armHours > 0) armHours--;
      break;
    case CONFIG_ARM_MINUTES:
      if (armMinutes > 0) armMinutes--;
      break;
    case CONFIG_ARM_SECONDS:
      if (armSeconds > 0) armSeconds--;
      break;
    case CONFIG_DISARM_HOURS:
      if (disarmHours > 0) disarmHours--;
      break;
    case CONFIG_DISARM_MINUTES:
      if (disarmMinutes > 0) disarmMinutes--;
      break;
    case CONFIG_DISARM_SECONDS:
      if (disarmSeconds > 0) disarmSeconds--;
      break;
  }
}

void advanceArmConfigSetting() {
  switch (currentState) {
    case CONFIG_ARM_HOURS:
      currentState = CONFIG_ARM_MINUTES;
      break;
    case CONFIG_ARM_MINUTES:
      currentState = CONFIG_ARM_SECONDS;
      break;
    case CONFIG_ARM_SECONDS:
      currentState = CONFIG_MENU;
      break;
  }
  showConfigArmTime();
}

void advanceDisarmConfigSetting() {
  switch (currentState) {
    case CONFIG_DISARM_HOURS:
      currentState = CONFIG_DISARM_MINUTES;
      break;
    case CONFIG_DISARM_MINUTES:
      currentState = CONFIG_DISARM_SECONDS;
      break;
    case CONFIG_DISARM_SECONDS:
      currentState = CONFIG_MENU;
      break;
  }
  showConfigDisarmTime();
}

void armBomb() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bomb Armed");
  delay(2000);
  currentState = BOMB_ARMED;
  startTime = millis();
  showCronometroDesarme();
}

void disarmBomb() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bomb Disarmed");
  delay(2000);
  currentState = CT_VENCEU;
  showCTVenceu();
}

void showProgress(const char* message, unsigned long elapsedTime, unsigned long totalTime) {
  int progress = (elapsedTime * 16) / totalTime; // Calcula o progresso em termos de 16 quadrados
  unsigned long remainingTime = (totalTime - elapsedTime) / 1000; // Calcula o tempo restante em segundos

  int remainingMinutes = remainingTime / 60;
  int remainingSeconds = remainingTime % 60;

  lcd.setCursor(0, 0);
  lcd.print(message);
  lcd.print(": ");
  lcd.print("00:");
  if (remainingSeconds < 10) {
    lcd.print("0");
  }
  lcd.print(remainingSeconds);
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    if (i < progress) {
      lcd.write(byte(0)); // Usa o caractere personalizado para preencher a barra de progresso
    } else {
      lcd.print(' ');
    }
  }
}

void showCronometroDesarme() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tempo Desarme:");
  updateCronometroDesarme();
}

void showCTVenceu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CT Venceu");
  lcd.setCursor(0, 1);
  lcd.print("Aperte 1 p/ menu");
}

void showTRVenceu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TR Venceu");
  lcd.setCursor(0, 1);
  lcd.print("Aperte 1 p/ menu");
}
