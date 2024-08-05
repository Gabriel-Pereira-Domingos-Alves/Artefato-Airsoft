#include <LiquidCrystal.h>


// Definição dos pinos
const int buttonPin1 = 7;
const int buttonPin2 = 8;
const int buttonPin3 = 9;
// Relé
const int relePin = 6;
bool isExploding = false;

// Variáveis de estado dos botões
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;

enum DominacaoState {
  NONE,
  AZUL,
  AMARELO
};

// Variáveis de tempo
unsigned long button1PressTime = 0;
unsigned long button2PressTime = 0;
unsigned long button1Elapsed = 0;
unsigned long button2Elapsed = 0;
unsigned long button3PressTime = 0;
unsigned long button3Elapsed = 0;

unsigned long azulDominacaoTempo = 0;
unsigned long amareloDominacaoTempo = 0;
unsigned long dominioStartTime = 0;
DominacaoState lastDominacaoState = NONE;

unsigned long startTime;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Inicializa o LCD

enum MenuState {
  // Variaveis do menu e Bomba
  GAME_MODE_MENU,
  GAME_OPTION_MENU,
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
  TR_VENCEU,
  // Variáveis do modo Conquista
  DOMINACAO_CONFIG,
  DOMINACAO_CONFIG_DURACAO_HOURS,
  DOMINACAO_CONFIG_DURACAO_MINUTES,
  DOMINACAO_CONFIG_DURACAO_SECONDS,
  DOMINACAO_CONFIG_TEMPO_DOMINAR_HOURS,
  DOMINACAO_CONFIG_TEMPO_DOMINAR_MINUTES,
  DOMINACAO_CONFIG_TEMPO_DOMINAR_SECONDS,
  DOMINACAO_START,
  DOMINACAO_RUNNING,
  DOMINACAO_DOMINATING_AZUL,
  DOMINACAO_DOMINATING_AMARELO,
  DOMINACAO_VENCEU,
  // Variáveis do modo Dominacao
  DOMINACAO_TEMPO_START,
  DOMINACAO_TEMPO_CONFIG,
  DOMINACAO_TEMPO_CONFIG_DURACAO_HOURS,
  DOMINACAO_TEMPO_CONFIG_DURACAO_MINUTES,
  DOMINACAO_TEMPO_CONFIG_DURACAO_SECONDS,
  DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_HOURS,
  DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_MINUTES,
  DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_SECONDS,
  DOMINACAO_TEMPO_RUNNING,
  DOMINACAO_TEMPO_DOMINATING_AZUL,
  DOMINACAO_TEMPO_DOMINATING_AMARELO,
  DOMINACAO_TEMPO_VENCEU
};


MenuState currentState = GAME_MODE_MENU;
int armHours = 0, armMinutes = 0, armSeconds = 10;
int disarmHours = 0, disarmMinutes = 0, disarmSeconds = 10;
int* currentSetting = &armHours;

// Variáveis do modo Dominacao
DominacaoState currentDominacaoState = NONE;
int dominacaoDuracaoHours = 0, dominacaoDuracaoMinutes = 5, dominacaoDuracaoSeconds = 0;
int dominacaoTempoDominarHours = 0, dominacaoTempoDominarMinutes = 0, dominacaoTempoDominarSeconds = 10;

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

const char* gameModes[] = {"Bomba", "Conquista", "Dominacao"};
int currentGameModeIndex = 0;
const int numGameModes = sizeof(gameModes) / sizeof(gameModes[0]);

void setup() {
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);

  pinMode(relePin, OUTPUT);

  lcd.begin(16, 2);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);

  lcd.createChar(0, fullBlock); // Cria o caractere personalizado para a barra de progresso

  showGameModeMenu();
}

void loop() {
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);

  // Verificar se o botão 3 é pressionado por 5 segundos
  if (buttonState3 == LOW) {
    if (button3PressTime == 0) {
      button3PressTime = millis();
    } else {
      button3Elapsed = millis() - button3PressTime;
      if (button3Elapsed >= 5000) {
        // Colocar o delay de 2 segundos antes de voltar ao menu
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Saindo...");
        delay(2000);
        currentState = GAME_MODE_MENU;
        showGameModeMenu();
        delay(200); // Debounce
      }
    }
  } else {
    button3PressTime = 0; // Reset the press time if the button is released
  }

  switch (currentState) {
    case GAME_MODE_MENU:
      handleGameModeMenu();
      break;
    case GAME_OPTION_MENU:
      handleGameOptionMenu();
      break;
    case CONFIG_MENU:
      handleConfigMenu();
      break;
    case CONFIG_ARM_HOURS:
    case CONFIG_ARM_MINUTES:
    case CONFIG_ARM_SECONDS:
      handleConfigArm();
      break;
    case CONFIG_DISARM_HOURS:
    case CONFIG_DISARM_MINUTES:
    case CONFIG_DISARM_SECONDS:
      handleConfigDisarm();
      break;
    case CRONOMETRO_PARTIDA:
      handleCronometroPartida();
      break;
    case START_ARM_BOMB:
      handleStartArmBomb();
      break;
    case BOMB_ARMED:
      handleBombArmed();
      break;
    case START_DISARM_BOMB:
      handleStartDisarmBomb();
      break;
    case CT_VENCEU:
    case TR_VENCEU:
      handleVictory();
      break;
    case ARMING_BOMB:
    case DISARMING_BOMB:
      // Essas etapas são controladas pelas funções armBomb() ou disarmBomb()
      break;
    case DOMINACAO_CONFIG:
      handleDominacaoConfig();
      break;
    case DOMINACAO_CONFIG_DURACAO_HOURS:
    case DOMINACAO_CONFIG_DURACAO_MINUTES:
    case DOMINACAO_CONFIG_DURACAO_SECONDS:
      handleDominacaoConfigDuracao();
      break;
    case DOMINACAO_CONFIG_TEMPO_DOMINAR_HOURS:
    case DOMINACAO_CONFIG_TEMPO_DOMINAR_MINUTES:
    case DOMINACAO_CONFIG_TEMPO_DOMINAR_SECONDS:
      handleDominacaoConfigTempoDominar();
      break;
    case DOMINACAO_START:
      handleDominacaoStart();
      break;
    case DOMINACAO_RUNNING:
      handleDominacaoRunning();
      break;
    case DOMINACAO_DOMINATING_AZUL:
      handleDominacaoDominatingAzul();
      break;
    case DOMINACAO_DOMINATING_AMARELO:
      handleDominacaoDominatingAmarelo();
      break;
    case DOMINACAO_VENCEU:
      handleDominacaoVenceu();
      break;
      // Modo Dominacao tempo
  case DOMINACAO_TEMPO_START:
    handleDominacaoTempoStart();
    break;
  case DOMINACAO_TEMPO_CONFIG:
    handleDominacaoTempoConfig();
    break;
  case DOMINACAO_TEMPO_CONFIG_DURACAO_HOURS:
  case DOMINACAO_TEMPO_CONFIG_DURACAO_MINUTES:
  case DOMINACAO_TEMPO_CONFIG_DURACAO_SECONDS:
    handleDominacaoTempoConfigDuracao();
    break;
  case DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_HOURS:
  case DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_MINUTES:
  case DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_SECONDS:
    handleDominacaoTempoConfigTempoDominar();
    break;
  case DOMINACAO_TEMPO_RUNNING:
    handleDominacaoTempoRunning();
    break;
  case DOMINACAO_TEMPO_DOMINATING_AZUL:
    handleDominacaoTempoDominatingAzul();
    break;
  case DOMINACAO_TEMPO_DOMINATING_AMARELO:
    handleDominacaoTempoDominatingAmarelo();
    break;
  case DOMINACAO_TEMPO_VENCEU:
    handleDominacaoTempoVenceu();
    break;
  }
}

void handleGameModeMenu() {
  if (buttonState1 == LOW) {
    currentGameModeIndex = (currentGameModeIndex + 1) % numGameModes;
    showGameModeMenu();
    delay(200);
  }
  if (buttonState2 == LOW) {
    currentGameModeIndex = (currentGameModeIndex - 1 + numGameModes) % numGameModes;
    showGameModeMenu();
    delay(200);
  }
  if (buttonState3 == LOW) {
    currentState = GAME_OPTION_MENU;
    showGameOptionMenu();
    delay(200);
  }
}

void handleGameOptionMenu() {
  if (buttonState1 == LOW) {
    if (currentGameModeIndex == 0) {
      currentState = CRONOMETRO_PARTIDA;
      startTime = millis();
      showCronometroPartida();
    } else if (currentGameModeIndex == 1) {
      currentState = DOMINACAO_START;
      startTime = millis();
      showDominacaoStart();
    } else if (currentGameModeIndex == 2) {
      currentState = DOMINACAO_TEMPO_RUNNING;
      startTime = millis();
      dominioStartTime = millis();  // Inicializa o tempo de domínio
      lastDominacaoState = NONE;
      showDominacaoTempoRunning();
    }
    delay(200);
  }
  if (buttonState2 == LOW) {
    if (currentGameModeIndex == 0) {
      currentState = CONFIG_MENU;
      showConfigMenu();
    } else if (currentGameModeIndex == 1) {
      currentState = DOMINACAO_CONFIG;
      showDominacaoConfig();
    } else if (currentGameModeIndex == 2) {
      currentState = DOMINACAO_TEMPO_CONFIG;
      showDominacaoTempoConfig();
    }
    delay(200);
  }
  if (buttonState3 == LOW) {
    currentState = GAME_MODE_MENU;
    showGameModeMenu();
    delay(200);
  }
}


void handleConfigMenu() {
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
    currentState = GAME_OPTION_MENU;
    showGameOptionMenu();
    delay(200);
  }
}

void handleConfigArm() {
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
}

void handleConfigDisarm() {
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
}

void handleCronometroPartida() {
  if (buttonState1 == LOW) {
    button1PressTime = millis();
    currentState = START_ARM_BOMB;
    showStartArmBomb();
    delay(200);
  } else {
    updateCronometroPartida();
  }
}

void handleStartArmBomb() {
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
}

void handleBombArmed() {
  if (buttonState2 == LOW) {
    button2PressTime = millis();
    currentState = START_DISARM_BOMB;
    showStartDisarmBomb();
    delay(200);
  } else {
    updateCronometroDesarme();
  }
}

void handleStartDisarmBomb() {
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
}

void handleVictory() {
  if (buttonState1 == LOW) {
    stopRele();

    currentState = GAME_MODE_MENU;
    showGameModeMenu();
    delay(200);
  }
}

// CONFIGURACOES DE DOMINACAO

void handleDominacaoConfig() {
  if (buttonState1 == LOW) {
    currentState = DOMINACAO_CONFIG_DURACAO_HOURS;
    showDominacaoConfigDuracao();
    delay(200);
  }
  if (buttonState2 == LOW) {
    currentState = DOMINACAO_CONFIG_TEMPO_DOMINAR_HOURS;
    showDominacaoConfigTempoDominar();
    delay(200);
  }
  if (buttonState3 == LOW) {
    currentState = GAME_OPTION_MENU;
    showGameOptionMenu();
    delay(200);
  }
}

void handleDominacaoConfigDuracao() {
  if (buttonState1 == LOW) {
    incrementDominacaoDuracao();
    showDominacaoConfigDuracao();
    delay(200);
  }
  if (buttonState2 == LOW) {
    decrementDominacaoDuracao();
    showDominacaoConfigDuracao();
    delay(200);
  }
  if (buttonState3 == LOW) {
    advanceDominacaoDuracaoConfig();
    delay(200);
  }
}

void handleDominacaoConfigTempoDominar() {
  if (buttonState1 == LOW) {
    incrementDominacaoTempoDominar();
    showDominacaoConfigTempoDominar();
    delay(200);
  }
  if (buttonState2 == LOW) {
    decrementDominacaoTempoDominar();
    showDominacaoConfigTempoDominar();
    delay(200);
  }
  if (buttonState3 == LOW) {
    advanceDominacaoTempoDominarConfig();
    delay(200);
  }
}

void handleDominacaoStart() {
  currentState = DOMINACAO_RUNNING;
  showDominacaoRunning();
}

void handleDominacaoRunning() {
  unsigned long elapsedTime = millis() - startTime;
  int remainingTime = (dominacaoDuracaoHours * 3600) + (dominacaoDuracaoMinutes * 60) + dominacaoDuracaoSeconds - (elapsedTime / 1000);

  if (remainingTime <= 0) {
    currentState = DOMINACAO_VENCEU;
    showDominacaoVencedor();
    return;
  }

  if (buttonState1 == LOW) {
    button1PressTime = millis();
    currentState = DOMINACAO_DOMINATING_AZUL;
    handleDominacaoDominatingAzul();  // Correção aqui
    delay(200);
  }

  if (buttonState2 == LOW) {
    button2PressTime = millis();
    currentState = DOMINACAO_DOMINATING_AMARELO;
    handleDominacaoDominatingAmarelo();  // Correção aqui
    delay(200);
  }

  int displayHours = remainingTime / 3600;
  int displayMinutes = (remainingTime % 3600) / 60;
  int displaySeconds = remainingTime % 60;

  lcd.setCursor(0, 1);
  lcd.print("Tempo: ");
  if (displayHours < 10) lcd.print("0");
  lcd.print(displayHours);
  lcd.print(":");
  if (displayMinutes < 10) lcd.print("0");
  lcd.print(displayMinutes);
  lcd.print(":");
  if (displaySeconds < 10) lcd.print("0");
  lcd.print(displaySeconds);
}


void handleDominacaoDominatingAzul() {
  if (buttonState1 == LOW) {
    if (button1PressTime == 0) {
      button1PressTime = millis();
    } else {
      button1Elapsed = millis() - button1PressTime;
      showProgress("Dominando", button1Elapsed, (dominacaoTempoDominarHours * 3600 + dominacaoTempoDominarMinutes * 60 + dominacaoTempoDominarSeconds) * 1000);
      if (button1Elapsed >= (dominacaoTempoDominarHours * 3600 + dominacaoTempoDominarMinutes * 60 + dominacaoTempoDominarSeconds) * 1000) {
        currentDominacaoState = AZUL;
        showDominacaoDominatedMessage("Azul");
        currentState = DOMINACAO_RUNNING;
        showDominacaoRunning();
        delay(200);
      }
    }
  } else {
    button1PressTime = 0;
    currentState = DOMINACAO_RUNNING;
    showDominacaoRunning();
  }
}

void handleDominacaoDominatingAmarelo() {
  if (buttonState2 == LOW) {
    if (button2PressTime == 0) {
      button2PressTime = millis();
    } else {
      button2Elapsed = millis() - button2PressTime;
      showProgress("Dominando", button2Elapsed, (dominacaoTempoDominarHours * 3600 + dominacaoTempoDominarMinutes * 60 + dominacaoTempoDominarSeconds) * 1000);
      if (button2Elapsed >= (dominacaoTempoDominarHours * 3600 + dominacaoTempoDominarMinutes * 60 + dominacaoTempoDominarSeconds) * 1000) {
        currentDominacaoState = AMARELO;
        showDominacaoDominatedMessage("Amarelo");
        currentState = DOMINACAO_RUNNING;
        showDominacaoRunning();
        delay(200);
      }
    }
  } else {
    button2PressTime = 0;
    currentState = DOMINACAO_RUNNING;
    showDominacaoRunning();
  }
}

void showDominacaoVencedor() {
  isExploding = true;
  toggleRele(20000, 700);

  lcd.clear();
  lcd.setCursor(0, 0);
  if (currentDominacaoState == AZUL) {
    lcd.print("Azul Venceu!");
  } else if (currentDominacaoState == AMARELO) {
    lcd.print("AMARELO Venceu!");
  } else {
    lcd.print("Ninguem Venceu!");
  }
  lcd.setCursor(0, 1);
  lcd.print("Aperte 3 p/ menu");

  currentDominacaoState = NONE;
}

void handleDominacaoVenceu() {
  if (buttonState3 == LOW) {
    stopRele();
    currentState = GAME_MODE_MENU;
    showGameModeMenu();
    delay(200);
  }
}

void showGameModeMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(gameModes[currentGameModeIndex]);
  lcd.setCursor(0, 1);
  lcd.print("1: Next 2: Prev");
}

void showGameOptionMenu() {
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

void incrementDominacaoDuracao() {
  switch (currentState) {
    case DOMINACAO_CONFIG_DURACAO_HOURS:
      dominacaoDuracaoHours++;
      break;
    case DOMINACAO_CONFIG_DURACAO_MINUTES:
      dominacaoDuracaoMinutes++;
      if (dominacaoDuracaoMinutes >= 60) {
        dominacaoDuracaoMinutes = 0;
        dominacaoDuracaoHours++;
      }
      break;
    case DOMINACAO_CONFIG_DURACAO_SECONDS:
      dominacaoDuracaoSeconds++;
      if (dominacaoDuracaoSeconds >= 60) {
        dominacaoDuracaoSeconds = 0;
        dominacaoDuracaoMinutes++;
        if (dominacaoDuracaoMinutes >= 60) {
          dominacaoDuracaoMinutes = 0;
          dominacaoDuracaoHours++;
        }
      }
      break;
  }
}

void decrementDominacaoDuracao() {
  switch (currentState) {
    case DOMINACAO_CONFIG_DURACAO_HOURS:
      if (dominacaoDuracaoHours > 0) dominacaoDuracaoHours--;
      break;
    case DOMINACAO_CONFIG_DURACAO_MINUTES:
      if (dominacaoDuracaoMinutes > 0) {
        dominacaoDuracaoMinutes--;
      } else if (dominacaoDuracaoHours > 0) {
        dominacaoDuracaoMinutes = 59;
        dominacaoDuracaoHours--;
      }
      break;
    case DOMINACAO_CONFIG_DURACAO_SECONDS:
      if (dominacaoDuracaoSeconds > 0) {
        dominacaoDuracaoSeconds--;
      } else if (dominacaoDuracaoMinutes > 0) {
        dominacaoDuracaoSeconds = 59;
        dominacaoDuracaoMinutes--;
        if (dominacaoDuracaoMinutes <= 0 && dominacaoDuracaoHours > 0) {
          dominacaoDuracaoMinutes = 59;
          dominacaoDuracaoHours--;
        }
      }
      break;
  }
}

void advanceDominacaoDuracaoConfig() {
  switch (currentState) {
    case DOMINACAO_CONFIG_DURACAO_HOURS:
      currentState = DOMINACAO_CONFIG_DURACAO_MINUTES;
      break;
    case DOMINACAO_CONFIG_DURACAO_MINUTES:
      currentState = DOMINACAO_CONFIG_DURACAO_SECONDS;
      break;
    case DOMINACAO_CONFIG_DURACAO_SECONDS:
      currentState = DOMINACAO_CONFIG;
      break;
  }
  showDominacaoConfigDuracao();
}

void incrementDominacaoTempoDominar() {
  switch (currentState) {
    case DOMINACAO_CONFIG_TEMPO_DOMINAR_HOURS:
      dominacaoTempoDominarHours++;
      break;
    case DOMINACAO_CONFIG_TEMPO_DOMINAR_MINUTES:
      dominacaoTempoDominarMinutes++;
      if (dominacaoTempoDominarMinutes >= 60) {
        dominacaoTempoDominarMinutes = 0;
        dominacaoTempoDominarHours++;
      }
      break;
    case DOMINACAO_CONFIG_TEMPO_DOMINAR_SECONDS:
      dominacaoTempoDominarSeconds++;
      if (dominacaoTempoDominarSeconds >= 60) {
        dominacaoTempoDominarSeconds = 0;
        dominacaoTempoDominarMinutes++;
        if (dominacaoTempoDominarMinutes >= 60) {
          dominacaoTempoDominarMinutes = 0;
          dominacaoTempoDominarHours++;
        }
      }
      break;
  }
}

void decrementDominacaoTempoDominar() {
  switch (currentState) {
    case DOMINACAO_CONFIG_TEMPO_DOMINAR_HOURS:
      if (dominacaoTempoDominarHours > 0) dominacaoTempoDominarHours--;
      break;
    case DOMINACAO_CONFIG_TEMPO_DOMINAR_MINUTES:
      if (dominacaoTempoDominarMinutes > 0) {
        dominacaoTempoDominarMinutes--;
      } else if (dominacaoTempoDominarHours > 0) {
        dominacaoTempoDominarMinutes = 59;
        dominacaoTempoDominarHours--;
      }
      break;
    case DOMINACAO_CONFIG_TEMPO_DOMINAR_SECONDS:
      if (dominacaoTempoDominarSeconds > 0) {
        dominacaoTempoDominarSeconds--;
      } else if (dominacaoTempoDominarMinutes > 0) {
        dominacaoTempoDominarSeconds = 59;
        dominacaoTempoDominarMinutes--;
        if (dominacaoTempoDominarMinutes <= 0 && dominacaoTempoDominarHours > 0) {
          dominacaoTempoDominarMinutes = 59;
          dominacaoTempoDominarHours--;
        }
      }
      break;
  }
}

void advanceDominacaoTempoDominarConfig() {
  switch (currentState) {
    case DOMINACAO_CONFIG_TEMPO_DOMINAR_HOURS:
      currentState = DOMINACAO_CONFIG_TEMPO_DOMINAR_MINUTES;
      break;
    case DOMINACAO_CONFIG_TEMPO_DOMINAR_MINUTES:
      currentState = DOMINACAO_CONFIG_TEMPO_DOMINAR_SECONDS;
      break;
    case DOMINACAO_CONFIG_TEMPO_DOMINAR_SECONDS:
      currentState = DOMINACAO_CONFIG;
      break;
  }
  showDominacaoConfigTempoDominar();
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

  isExploding = true;
  toggleRele(30000, 700);
}

void showTRVenceu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TR Venceu");
  lcd.setCursor(0, 1);
  lcd.print("Aperte 1 p/ menu");

  isExploding = true;
  toggleRele(30000, 700);
}

void showDominacaoConfig() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1: Duracao");
  lcd.setCursor(0, 1);
  lcd.print("2: Tempo Dominar");
}

void showDominacaoConfigDuracao() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Duracao:");
  lcd.setCursor(0, 1);
  lcd.print(dominacaoDuracaoHours);
  lcd.print(":");
  lcd.print(dominacaoDuracaoMinutes);
  lcd.print(":");
  lcd.print(dominacaoDuracaoSeconds);
}

void showDominacaoConfigTempoDominar() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tempo Dominar:");
  lcd.setCursor(0, 1);
  lcd.print(dominacaoTempoDominarHours);
  lcd.print(":");
  lcd.print(dominacaoTempoDominarMinutes);
  lcd.print(":");
  lcd.print(dominacaoTempoDominarSeconds);
}

void showDominacaoStart() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dominacao");
  lcd.setCursor(0, 1);
  lcd.print("Tempo: ");
  lcd.print(dominacaoDuracaoHours);
  lcd.print(":");
  lcd.print(dominacaoDuracaoMinutes);
  lcd.print(":");
  lcd.print(dominacaoDuracaoSeconds);
}

void showDominacaoRunning() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dominio:");
  switch (currentDominacaoState) {
    case NONE:
      lcd.print("Ninguem");
      break;
    case AZUL:
      lcd.print("Azul");
      break;
    case AMARELO:
      lcd.print("Amarelo");
      break;
  }
  showDominacaoRunningTime();
}

void showDominacaoRunningTime() {
  unsigned long elapsedTime = millis() - startTime;
  int remainingTime = (dominacaoDuracaoHours * 3600) + (dominacaoDuracaoMinutes * 60) + dominacaoDuracaoSeconds - (elapsedTime / 1000);

  int displayHours = remainingTime / 3600;
  int displayMinutes = (remainingTime % 3600) / 60;
  int displaySeconds = remainingTime % 60;

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

void showDominacaoDominatedMessage(const char* team) {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (strcmp(team, "Azul") == 0) {
    lcd.print("Azul");
  } else {
    lcd.print("Amarelo");
  }
  lcd.setCursor(0, 1);
  lcd.print("Dominou");
  delay(1500);  // Espera por 1.5 segundos
}


void showDominacaoVenceu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dominacao");
  lcd.setCursor(0, 1);
  lcd.print("Aperte 1 p/ menu");
}

// Menu Tempo Dominado
void showDominacaoTempoConfig() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1: Duracao");
  lcd.setCursor(0, 1);
  lcd.print("2: Tempo Dominar");
}

void showDominacaoTempoConfigDuracao() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Duracao:");
  lcd.setCursor(0, 1);
  lcd.print(dominacaoDuracaoHours);
  lcd.print(":");
  lcd.print(dominacaoDuracaoMinutes);
  lcd.print(":");
  lcd.print(dominacaoDuracaoSeconds);
}

void showDominacaoTempoConfigTempoDominar() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tempo Dominar:");
  lcd.setCursor(0, 1);
  lcd.print(dominacaoTempoDominarHours);
  lcd.print(":");
  lcd.print(dominacaoTempoDominarMinutes);
  lcd.print(":");
  lcd.print(dominacaoTempoDominarSeconds);
}

// Handlers Tempo Dominado
void handleDominacaoTempoStart() {
  if (buttonState1 == LOW) {
    currentState = DOMINACAO_TEMPO_RUNNING;
    startTime = millis();
    dominioStartTime = millis();  // Inicializa o tempo de domínio
    lastDominacaoState = NONE;
    showDominacaoTempoRunning();
    delay(200);
  }
  if (buttonState2 == LOW) {
    currentState = DOMINACAO_TEMPO_CONFIG;
    showDominacaoTempoConfig();
    delay(200);
  }
}

void handleDominacaoTempoConfig() {
  if (buttonState1 == LOW) {
    currentState = DOMINACAO_TEMPO_CONFIG_DURACAO_HOURS;
    showDominacaoTempoConfigDuracao();
    delay(200);
  }
  if (buttonState2 == LOW) {
    currentState = DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_HOURS;
    showDominacaoTempoConfigTempoDominar();
    delay(200);
  }
  if (buttonState3 == LOW) {
    currentState = GAME_OPTION_MENU;
    showGameOptionMenu();
    delay(200);
  }
}
// Handlers Config tempo
void handleDominacaoTempoConfigDuracao() {
  if (buttonState1 == LOW) {
    incrementDominacaoTempoDuracao();
    showDominacaoTempoConfigDuracao();
    delay(200);
  }
  if (buttonState2 == LOW) {
    decrementDominacaoTempoDuracao();
    showDominacaoTempoConfigDuracao();
    delay(200);
  }
  if (buttonState3 == LOW) {
    advanceDominacaoTempoDuracaoConfig();
    delay(200);
  }
}

void handleDominacaoTempoConfigTempoDominar() {
  if (buttonState1 == LOW) {
    incrementDominacaoTempoTempoDominar();
    showDominacaoTempoConfigTempoDominar();
    delay(200);
  }
  if (buttonState2 == LOW) {
    decrementDominacaoTempoTempoDominar();
    showDominacaoTempoConfigTempoDominar();
    delay(200);
  }
  if (buttonState3 == LOW) {
    advanceDominacaoTempoTempoDominarConfig();
    delay(200);
  }
}

void advanceDominacaoTempoDuracaoConfig() {
  switch (currentState) {
    case DOMINACAO_TEMPO_CONFIG_DURACAO_HOURS:
      currentState = DOMINACAO_TEMPO_CONFIG_DURACAO_MINUTES;
      break;
    case DOMINACAO_TEMPO_CONFIG_DURACAO_MINUTES:
      currentState = DOMINACAO_TEMPO_CONFIG_DURACAO_SECONDS;
      break;
    case DOMINACAO_TEMPO_CONFIG_DURACAO_SECONDS:
      currentState = DOMINACAO_TEMPO_CONFIG;
      break;
  }
  showDominacaoTempoConfigDuracao();
}

void advanceDominacaoTempoTempoDominarConfig() {
  switch (currentState) {
    case DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_HOURS:
      currentState = DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_MINUTES;
      break;
    case DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_MINUTES:
      currentState = DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_SECONDS;
      break;
    case DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_SECONDS:
      currentState = DOMINACAO_TEMPO_CONFIG;
      break;
  }
  showDominacaoTempoConfigTempoDominar();
}

//Funçoes de incremetar o tempo:

void incrementDominacaoTempoDuracao() {
  switch (currentState) {
    case DOMINACAO_TEMPO_CONFIG_DURACAO_HOURS:
      dominacaoDuracaoHours++;
      break;
    case DOMINACAO_TEMPO_CONFIG_DURACAO_MINUTES:
      dominacaoDuracaoMinutes++;
      if (dominacaoDuracaoMinutes >= 60) {
        dominacaoDuracaoMinutes = 0;
        dominacaoDuracaoHours++;
      }
      break;
    case DOMINACAO_TEMPO_CONFIG_DURACAO_SECONDS:
      dominacaoDuracaoSeconds++;
      if (dominacaoDuracaoSeconds >= 60) {
        dominacaoDuracaoSeconds = 0;
        dominacaoDuracaoMinutes++;
        if (dominacaoDuracaoMinutes >= 60) {
          dominacaoDuracaoMinutes = 0;
          dominacaoDuracaoHours++;
        }
      }
      break;
  }
}

void decrementDominacaoTempoDuracao() {
  switch (currentState) {
    case DOMINACAO_TEMPO_CONFIG_DURACAO_HOURS:
      if (dominacaoDuracaoHours > 0) dominacaoDuracaoHours--;
      break;
    case DOMINACAO_TEMPO_CONFIG_DURACAO_MINUTES:
      if (dominacaoDuracaoMinutes > 0) {
        dominacaoDuracaoMinutes--;
      } else if (dominacaoDuracaoHours > 0) {
        dominacaoDuracaoMinutes = 59;
        dominacaoDuracaoHours--;
      }
      break;
    case DOMINACAO_TEMPO_CONFIG_DURACAO_SECONDS:
      if (dominacaoDuracaoSeconds > 0) {
        dominacaoDuracaoSeconds--;
      } else if (dominacaoDuracaoMinutes > 0) {
        dominacaoDuracaoSeconds = 59;
        dominacaoDuracaoMinutes--;
        if (dominacaoDuracaoMinutes <= 0 && dominacaoDuracaoHours > 0) {
          dominacaoDuracaoMinutes = 59;
          dominacaoDuracaoHours--;
        }
      }
      break;
  }
}

void incrementDominacaoTempoTempoDominar() {
  switch (currentState) {
    case DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_HOURS:
      dominacaoTempoDominarHours++;
      break;
    case DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_MINUTES:
      dominacaoTempoDominarMinutes++;
      if (dominacaoTempoDominarMinutes >= 60) {
        dominacaoTempoDominarMinutes = 0;
        dominacaoTempoDominarHours++;
      }
      break;
    case DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_SECONDS:
      dominacaoTempoDominarSeconds++;
      if (dominacaoTempoDominarSeconds >= 60) {
        dominacaoTempoDominarSeconds = 0;
        dominacaoTempoDominarMinutes++;
        if (dominacaoTempoDominarMinutes >= 60) {
          dominacaoTempoDominarMinutes = 0;
          dominacaoTempoDominarHours++;
        }
      }
      break;
  }
}

void decrementDominacaoTempoTempoDominar() {
  switch (currentState) {
    case DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_HOURS:
      if (dominacaoTempoDominarHours > 0) dominacaoTempoDominarHours--;
      break;
    case DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_MINUTES:
      if (dominacaoTempoDominarMinutes > 0) {
        dominacaoTempoDominarMinutes--;
      } else if (dominacaoTempoDominarHours > 0) {
        dominacaoTempoDominarMinutes = 59;
        dominacaoTempoDominarHours--;
      }
      break;
    case DOMINACAO_TEMPO_CONFIG_TEMPO_DOMINAR_SECONDS:
      if (dominacaoTempoDominarSeconds > 0) {
        dominacaoTempoDominarSeconds--;
      } else if (dominacaoTempoDominarMinutes > 0) {
        dominacaoTempoDominarSeconds = 59;
        dominacaoTempoDominarMinutes--;
        if (dominacaoTempoDominarMinutes <= 0 && dominacaoTempoDominarHours > 0) {
          dominacaoTempoDominarMinutes = 59;
          dominacaoTempoDominarHours--;
        }
      }
      break;
  }
}


// Dominacao game functions

void handleDominacaoTempoDominatingAzul() {
  if (buttonState1 == LOW) {
    if (button1PressTime == 0) {
      button1PressTime = millis();
    } else {
      button1Elapsed = millis() - button1PressTime;
      showProgress("Dominando", button1Elapsed, (dominacaoTempoDominarHours * 3600 + dominacaoTempoDominarMinutes * 60 + dominacaoTempoDominarSeconds) * 1000);
      if (button1Elapsed >= (dominacaoTempoDominarHours * 3600 + dominacaoTempoDominarMinutes * 60 + dominacaoTempoDominarSeconds) * 1000) {
        if (lastDominacaoState != AZUL) {
          if (lastDominacaoState == AMARELO) {
            amareloDominacaoTempo += millis() - dominioStartTime;
          }
          dominioStartTime = millis();
          lastDominacaoState = AZUL;  // Atualização de lastDominacaoState
        }
        currentDominacaoState = AZUL;
        showDominacaoTempoDominatedMessage("Azul");
        currentState = DOMINACAO_TEMPO_RUNNING;
        showDominacaoTempoRunning();
        delay(200);
      }
    }
  } else {
    button1PressTime = 0;
    currentState = DOMINACAO_TEMPO_RUNNING;
    showDominacaoTempoRunning();
  }
}

void handleDominacaoTempoDominatingAmarelo() {
  if (buttonState2 == LOW) {
    if (button2PressTime == 0) {
      button2PressTime = millis();
    } else {
      button2Elapsed = millis() - button2PressTime;
      showProgress("Dominando", button2Elapsed, (dominacaoTempoDominarHours * 3600 + dominacaoTempoDominarMinutes * 60 + dominacaoTempoDominarSeconds) * 1000);
      if (button2Elapsed >= (dominacaoTempoDominarHours * 3600 + dominacaoTempoDominarMinutes * 60 + dominacaoTempoDominarSeconds) * 1000) {
        if (lastDominacaoState != AMARELO) {
          if (lastDominacaoState == AZUL) {
            azulDominacaoTempo += millis() - dominioStartTime;
          }
          dominioStartTime = millis();
          lastDominacaoState = AMARELO;  // Atualização de lastDominacaoState
        }
        currentDominacaoState = AMARELO;
        showDominacaoTempoDominatedMessage("Amarelo");
        currentState = DOMINACAO_TEMPO_RUNNING;
        showDominacaoTempoRunning();
        delay(200);
      }
    }
  } else {
    button2PressTime = 0;
    currentState = DOMINACAO_TEMPO_RUNNING;
    showDominacaoTempoRunning();
  }
}

void handleDominacaoTempoRunning() {
  unsigned long elapsedTime = millis() - startTime;
  int remainingTime = (dominacaoDuracaoHours * 3600) + (dominacaoDuracaoMinutes * 60) + dominacaoDuracaoSeconds - (elapsedTime / 1000);

  if (remainingTime <= 0) {
    if (lastDominacaoState == AZUL) {
      azulDominacaoTempo += millis() - dominioStartTime;
    } else if (lastDominacaoState == AMARELO) {
      amareloDominacaoTempo += millis() - dominioStartTime;
    }
    currentState = DOMINACAO_TEMPO_VENCEU;
    showDominacaoTempoVencedor();
    return;
  }

  if (buttonState1 == LOW) {
    button1PressTime = millis();
    currentState = DOMINACAO_TEMPO_DOMINATING_AZUL;
    handleDominacaoTempoDominatingAzul();
    delay(200);
  }

  if (buttonState2 == LOW) {
    button2PressTime = millis();
    currentState = DOMINACAO_TEMPO_DOMINATING_AMARELO;
    handleDominacaoTempoDominatingAmarelo();
    delay(200);
  }

  int displayHours = remainingTime / 3600;
  int displayMinutes = (remainingTime % 3600) / 60;
  int displaySeconds = remainingTime % 60;

  lcd.setCursor(0, 1);
  lcd.print("Tempo: ");
  if (displayHours < 10) lcd.print("0");
  lcd.print(displayHours);
  lcd.print(":");
  if (displayMinutes < 10) lcd.print("0");
  lcd.print(displayMinutes);
  lcd.print(":");
  if (displaySeconds < 10) lcd.print("0");
  lcd.print(displaySeconds);
}

void showDominacaoTempoVencedor() {
  isExploding = true;
  toggleRele(30000, 700);

  lcd.clear();
  lcd.setCursor(0, 0);
  if (azulDominacaoTempo > amareloDominacaoTempo) {
    lcd.print("AZUL Venceu!");
  } else if (amareloDominacaoTempo > azulDominacaoTempo) {
    lcd.print("AMARELO Venceu!");
  } else {
    lcd.print("EMPATE!");
  }
  lcd.setCursor(0, 1);
  lcd.print("1/2: Det 3: Menu");

  currentDominacaoState = NONE;
}

void showDominacaoTempoDetails() {
  char buffer[9];  // Buffer para armazenar o tempo formatado

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Azul    ");
  formatTime(azulDominacaoTempo, buffer);
  lcd.print(buffer);

  lcd.setCursor(0, 1);
  lcd.print("Amarelo ");
  formatTime(amareloDominacaoTempo, buffer);
  lcd.print(buffer);
}

void handleDominacaoTempoVenceu() {
  if (buttonState1 == LOW || buttonState2 == LOW) {
    showDominacaoTempoDetails();
    delay(200);
    // Esperar pelo botão 1 ou 2 para voltar à tela do vencedor
    while (true) {
      buttonState1 = digitalRead(buttonPin1);
      buttonState2 = digitalRead(buttonPin2);
      if (buttonState1 == LOW || buttonState2 == LOW) {
        showDominacaoTempoVencedor();
        delay(200);
        break;
      }
    }
  }
  if (buttonState3 == LOW) {
    stopRele();
    currentState = GAME_MODE_MENU;
    showGameModeMenu();
    azulDominacaoTempo = 0;
    amareloDominacaoTempo = 0;
    delay(200);
  }
}

void showDominacaoTempoDominatedMessage(const char* team) {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (strcmp(team, "Azul") == 0) {
    lcd.print("Azul");
  } else {
    lcd.print("Amarelo");
  }
  lcd.setCursor(0, 1);
  lcd.print("Dominou");
  delay(1500);  // Espera por 1.5 segundos
}

void showDominacaoTempoRunning() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dominio:");
  switch (currentDominacaoState) {
    case NONE:
      lcd.print("Ninguem");
      break;
    case AZUL:
      lcd.print("Azul");
      break;
    case AMARELO:
      lcd.print("Amarelo");
      break;
  }
  showDominacaoTempoRunningTime();
}

void showDominacaoTempoRunningTime() {
  unsigned long elapsedTime = millis() - startTime;
  int remainingTime = (dominacaoDuracaoHours * 3600) + (dominacaoDuracaoMinutes * 60) + dominacaoDuracaoSeconds - (elapsedTime / 1000);

  int displayHours = remainingTime / 3600;
  int displayMinutes = (remainingTime % 3600) / 60;
  int displaySeconds = remainingTime % 60;

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

void formatTime(unsigned long timeMillis, char* buffer) {
  unsigned long totalSeconds = timeMillis / 1000;
  unsigned int hours = totalSeconds / 3600;
  unsigned int minutes = (totalSeconds % 3600) / 60;
  unsigned int seconds = totalSeconds % 60;
  sprintf(buffer, "%02u:%02u:%02u", hours, minutes, seconds);
}

void toggleRele(unsigned long timeDelay, unsigned long duration) {
  unsigned long startTime = millis();
  bool releState = false;

  while (millis() - startTime < timeDelay && isExploding == true) {
    releState = !releState;
    digitalWrite(relePin, releState);
    delay(duration);
  }

  digitalWrite(relePin, LOW); // Garante que o relé está desligado ao final
}

void stopRele() {
  isExploding = false; // Corrigido: ponto e vírgula
  digitalWrite(relePin, LOW); // Garante que o relé está desligado
}
