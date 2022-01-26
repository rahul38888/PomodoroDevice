int D1 = 2;
int D2 = 3;
int D3 = 4;
int D4 = 5;

int dataPin = 11;
int latchPin = 8;
int clockPin = 12;

int buzzerPin = 6;

int buttonPin = 7;

int stateLedPin = 9;

// ----- ^^ PINS ^^ -----

int delayTime = 4;
int buzzerFrequency = 523;

int nums[10] = {
  1, 79, 18, 6, 76, 36, 32, 15, 0, 4
};
int blanckNum = 127;
int pLetter = 24;

void updateShiftRegisterState(byte input){
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, input);
  digitalWrite(latchPin, HIGH);
}

void draw(int state, bool enableDecimal){
  int leds = state*2;
  if(!enableDecimal) {
    leds = leds + 1;
  }
  
  updateShiftRegisterState(leds);
}

void resetTo(int letter, int dState){
  digitalWrite(D1, dState);
  digitalWrite(D2, dState);
  digitalWrite(D3, dState);
  digitalWrite(D4, dState);
  
  draw(letter, false);
}

void setDigit(int d1, int d2, int d3, int d4){
  digitalWrite(D1, d1);
  digitalWrite(D2, d2);
  digitalWrite(D3, d3);
  digitalWrite(D4, d4);
}

void setD1() {
  setDigit(HIGH, LOW, LOW, LOW);
}

void setD2() {
  setDigit(LOW, HIGH, LOW, LOW);
}

void setD3() {
  setDigit(LOW, LOW, HIGH, LOW);
}

void setD4() {
  setDigit(LOW, LOW, LOW, HIGH);
}

void drawNumber(int num){
  if(num == -1){
    resetTo(blanckNum, LOW);
    return;
  }
  else if(num == -2){
    resetTo(pLetter, HIGH);
    return;
  }

  for(int i=0; i<4; i++){
    if(num==0 and i>0){
      break;
    }
    
    int dig = num % 10;
    num = num/10;
    
    if(i==0){
      setD4();
    }
    else if(i==1){
      setD3();
    }
    else if(i==2){
      setD2();
    }
    else if(i==3){
      setD1();
    }
    bool enableDecimal = false;
    if(i==2){
      enableDecimal = true;
    }
    draw(nums[dig], enableDecimal);
    delay(delayTime);
    resetTo(blanckNum, LOW);
  }
}

int secToTime(int secs){
  int mins = secs/60;
  int remSecs = secs%60;

  return mins*100 + remSecs;
}

int pomodoroSecs[8] = {
  1500, 300, 1500, 300, 1500, 300, 1500, 900
};
//int pomodoroSecs[8] = {
//  10, 5, 10, 5, 10, 5, 10, 6
//};

bool isWorkState[8] = {
  true, false, true, false, true, false, true, false
};

int curPomodoro = 0;

int curSecs = -1;

int STOP_SIGNAL = -2;
int WELCOME_STATE = -1;
int TIMER_STATE = 0;
int TIMER_END = 1;
int TIMER_PAUSED = 2;
int SKIP_SIGNAL = 3;

int deviceState = WELCOME_STATE;

void resetTimer(){
  curSecs = 0;
  curPomodoro = (curPomodoro+1)%8;
}

unsigned long nextMillis = 0;

int setNextMillis(int gap){
    nextMillis = millis()+gap;
    nextMillis = nextMillis - nextMillis%100;
}

int lastButtonState = LOW;
int curButtonState;
unsigned long debounceDelay = 50;
unsigned long lastDebounceTime  = 0;
unsigned long buttonDownMillis = 0;

unsigned long longPressTime = 1000;
unsigned long stopPressTime = 3000;

unsigned long isButtonPressed(){
  unsigned long pressTime = 0;
  
  int buttonState = digitalRead(buttonPin);

  if(lastButtonState != buttonState)
    lastDebounceTime = millis();

  if((millis() - lastDebounceTime) > debounceDelay)
    if(buttonState != curButtonState){
      curButtonState = buttonState;
      
      if(curButtonState == HIGH)
         buttonDownMillis = millis();
      if(curButtonState == LOW)
        pressTime = millis() - buttonDownMillis;
    }

  lastButtonState = buttonState;
  return pressTime;
}

bool disOn = true;

void resetDevice(){
  curSecs = -1;
  curPomodoro = 0;
}

int timerPaused(){
  unsigned long buttonMillis = isButtonPressed();
  if(buttonMillis>stopPressTime){
    drawNumber(-1);
    noTone(buzzerPin);
    resetDevice();
    return STOP_SIGNAL;
  }
  else if(buttonMillis>longPressTime)
    return SKIP_SIGNAL;
  else if(buttonMillis)
    return TIMER_STATE;
  
  if(millis() > nextMillis){
    setNextMillis(500);
    disOn = !disOn;
  }

  if(disOn)
    drawNumber(secToTime(curSecs));
  else
    drawNumber(-1);

  return TIMER_PAUSED;

}

int timerRunner(){

  unsigned long buttonMillis = isButtonPressed();
  if(buttonMillis>stopPressTime){
    drawNumber(-1);
    noTone(buzzerPin);
    resetDevice();
    return STOP_SIGNAL;
  }
  else if(buttonMillis>longPressTime)
    return SKIP_SIGNAL;
  else if(buttonMillis) {
    tone(buzzerPin, buzzerFrequency);
    noTone(buzzerPin);
    return TIMER_PAUSED;
  }
  
  if(curSecs <= pomodoroSecs[curPomodoro]){
    if(millis() > nextMillis){
      curSecs++;
      setNextMillis(1000);
    }

    digitalWrite(stateLedPin, isWorkState[curPomodoro]);
    
    drawNumber(secToTime(curSecs));
    return TIMER_STATE;
  }
  else{
    resetTimer();
    return TIMER_END;
  }
}

int timerEnderProgramm(){
  unsigned long buttonMillis = isButtonPressed();
  if(buttonMillis>stopPressTime){
    drawNumber(-1);
    noTone(buzzerPin);
    resetDevice();
    return STOP_SIGNAL;
  }
  else if(buttonMillis > longPressTime)
    return SKIP_SIGNAL;
  else if(buttonMillis){
    noTone(buzzerPin);
    return TIMER_STATE;
  }
  
  digitalWrite(stateLedPin, isWorkState[curPomodoro]);

  if(millis() > nextMillis){
    setNextMillis(500);
    disOn = !disOn;
  }

  if(disOn){
    drawNumber(secToTime(pomodoroSecs[curPomodoro]));
    tone(buzzerPin, buzzerFrequency);
  }
  else{
    drawNumber(-1);
    noTone(buzzerPin);
  }
  return TIMER_END;
}

int skipCurrentTimer(){
    resetTimer();
    return TIMER_END;
}


unsigned long welcomeTomeLength = 2000;
unsigned long startMillis = 0;
int welcomeProgramm(){
  if(millis()-startMillis<=welcomeTomeLength){
    tone(buzzerPin, buzzerFrequency);
  }
  else
    noTone(buzzerPin);
  
  unsigned long buttonMillis = isButtonPressed();
  if(buttonMillis>stopPressTime){
    drawNumber(-1);
    noTone(buzzerPin);
    resetDevice();
    return STOP_SIGNAL;
  }
  else if(buttonMillis){
    pinMode(stateLedPin, OUTPUT);
    noTone(buzzerPin);
    return TIMER_STATE;
  }
  
  drawNumber(-2);
  return WELCOME_STATE;
}

int doNothing(){
  pinMode(stateLedPin, INPUT);
  if(isButtonPressed()){
    startMillis = millis();
    return WELCOME_STATE;
  }

  return STOP_SIGNAL;
}

void setup() {
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  pinMode(buzzerPin, OUTPUT);

  pinMode(buttonPin, INPUT);

  pinMode(stateLedPin, INPUT);
}

void loop() {
  if(deviceState == STOP_SIGNAL)
    deviceState = doNothing();
  else if(deviceState == WELCOME_STATE)
    deviceState = welcomeProgramm();
  else if(deviceState == TIMER_STATE)
    deviceState = timerRunner();
  else if(deviceState == TIMER_PAUSED)
    deviceState = timerPaused();
  else if(deviceState == TIMER_END)
    deviceState = timerEnderProgramm();
  else if(deviceState == SKIP_SIGNAL)
    deviceState = skipCurrentTimer();
  
}
