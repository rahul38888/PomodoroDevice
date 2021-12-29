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

void reset(){
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  
  draw(blanckNum, false);
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
  if(num < 0){
    reset();
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
    reset();
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

//  0 -> Timer running, 1 -> end of last timer
int TIMER_STATE = 0;
int TIMER_END = 1;

int deviceState = TIMER_STATE;

void resetTimer(){
  curSecs = 0;
  curPomodoro = (curPomodoro+1)%8;
}

unsigned long nextMillis = 0;

int setNextMillis(int gap){
    nextMillis = millis()+gap;
    nextMillis = nextMillis - nextMillis%100;
}

int timerRunner(){
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

bool enderOn = true;
int timerEnderProgramm(){

  int buttonState = digitalRead(buttonPin);
  
  if(buttonState == HIGH){
    noTone(buzzerPin);
    return TIMER_STATE;
  }
  
  digitalWrite(stateLedPin, isWorkState[curPomodoro]);

  if(millis() > nextMillis){
    setNextMillis(500);
    enderOn = !enderOn;
  }

  if(enderOn){
    drawNumber(secToTime(pomodoroSecs[curPomodoro]));
    tone(buzzerPin, buzzerFrequency);
  }
  else{
    drawNumber(-1);
    noTone(buzzerPin);
  }
  return TIMER_END;
}


void setup() {
  Serial.begin(9600);
  
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  pinMode(buzzerPin, OUTPUT);

  pinMode(buttonPin, INPUT);

  pinMode(stateLedPin, OUTPUT);
}

void loop() {
  if(deviceState == TIMER_STATE){
    deviceState = timerRunner();
  }
  else if(deviceState == TIMER_END){
    deviceState = timerEnderProgramm();
  }
}
