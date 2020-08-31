int speaker1 = 11;
int fsr = A1;
int red_led = 6;
int green_led = 7;
int power = 1;
unsigned long time1 = 0;
unsigned long time2 = 0;
unsigned long time3 = 0;
int threshold = 50;
long timeArray_aud[100];
long timeArray_tact[100];
long timeArray_sync[100];
int counter_aud = 0;
int counter_tact = 0;
int counter_sync = 0;
int raw_fsr = 0;
int Switch = 8;
int state = 0;
int counter_start = 0;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(speaker1, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(power, INPUT);
  pinMode(fsr, INPUT);
  pinMode(Switch, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // three stimulation types to be run 100x each in a random order with a random delay between...
  // when the experiment is finished a melody will play letting the user know it is over
  while(digitalRead(Switch) == HIGH){
    if(counter_start == 0){
      digitalWrite(red_led,HIGH);
      digitalWrite(green_led,HIGH);
      tone(speaker1, 250, 500);
      delay(500);
      tone(speaker1, 500, 500);
      delay(500);
      tone(speaker1, 750, 500);
      delay(500);       
      digitalWrite(red_led,LOW);
      digitalWrite(green_led,LOW);
      delay(500);
      counter_start = 1;
    }
    audiostim();
    delay(random(400,900));
    tactilestim();
    delay(random(400,900));
    syncstim();
    delay(random(400,900));    
  }
  if(digitalRead(Switch) == LOW && counter_start == 1){
    tone(speaker1, 250, 500);
    delay(500);
    tone(speaker1, 500, 500);
    delay(500);
    tone(speaker1, 250, 500);
    delay(500);
    digitalWrite(red_led,LOW);
    digitalWrite(green_led,LOW);
    noTone(speaker1); 
    counter_start = 0; 
  }
}

void audiostim() // create function audiostim() to be called during loop
// audiostim() turns on a red light, plays a 1000 Hz tone for 240 ms, then turns off the red light
{
  digitalWrite(red_led, HIGH); // turn on red led to let user know audio stim is happening
  delay(1000); // gives a user 300 ms to prepare
  noTone(speaker1); // set speaker to be quiet
  tone(speaker1, 1000, 240); // tone plays at 1000 Hz for 240 ms
  time3 = millis();
  delay(240); // delay takes place during tone
  Serial.print("The time stamp of the auditory stim is: ");
  Serial.println(time3);
  noTone(speaker1); // set speaker to quiet
  digitalWrite(red_led, LOW); // red led is turned off to let user know audio stim is done
}

void tactilestim() // create function tactilestim() to be called during loop
// tactilestim() turns on a green light, begins FSR reading, and ends when a touch has been recorded...
// and turns off the green light
{
  
  digitalWrite(green_led, HIGH); // turn on green led to let user know tactile stime is required
  raw_fsr = analogRead(fsr); // read values from force sensor
  while(raw_fsr < threshold){ //code waits for fsr to be nonzero
    raw_fsr = analogRead(fsr); // read values from force sensor
  }
  if(raw_fsr >= threshold) { // if statement to detect tactile stimulation
    digitalWrite(green_led, LOW);
    time1 = millis(); // time1 marks initial beginning of tactile stim
    while(raw_fsr >= threshold) { // while threshold is exceeded, time2 is constantly updated
      time2 = millis(); // time2 represents end of tactile stimulation
      raw_fsr = analogRead(fsr); // in order to constantly update time2, the pin reading has to be called again
    }
  }
  if(time2-time1 >= 5) { // if statement to confirm a touch happened before moving on to next stim
    counter_tact = counter_tact + 1; // counter used to track duration of stim
    timeArray_tact[counter_tact] = time2 - time1; // adds duration of stim to an array
    raw_fsr = 0; // sets fsr to below threshold until the analog read is called again
    Serial.print("The duration of the touch is (ms): ");
    Serial.println(timeArray_tact[counter_tact]);
    Serial.print("The time stamp of the tactile stim is: ");
    Serial.println(time1);
    time1 = millis();
    time2 = millis();
  }
  {
   digitalWrite(green_led, LOW); // turns off green led to let user know tactile stim is done  
  }
}

void syncstim() // create function syncstim() to be called during loop
// syncstim() turns on a green light, begins FSR reading, plays a 1000 Hz tone when above the ...
// threshold, and ends when a touch has been recorded, and turns off the green light
{
  digitalWrite(green_led, HIGH); // turn on green led to let user know tactile stime is required
  noTone(speaker1); // readies speaker but is silent
  raw_fsr = analogRead(fsr); // read values from force sensor
  while(raw_fsr < threshold){ //code waits for fsr to be nonzero
    raw_fsr = analogRead(fsr); //read values from force sensor
  }
  if(raw_fsr >= threshold) { // if statement to detect tactile stimulation
    tone(speaker1, 1000); // once touch detected a tone is played
    time1 = millis(); // // time1 marks initial beginning of tactile stim
    while(raw_fsr >= threshold) { // while threshold is exceeded, time2 is constantly updated
      time2 = millis(); // time2 represents end of tactile stimulation
      raw_fsr = analogRead(fsr); // in order to constantly update time2, the pin reading has to be called again
      if(analogRead(fsr) < threshold){
        noTone(speaker1);
      }
    }
  }
  if(time2-time1 >= 5) { // if statement to confirm a touch happened before moving on to next stim
    counter_sync = counter_sync + 1; // counter used to track duration of stim
    timeArray_sync[counter_sync] = time2 - time1; // adds duration of stim to an array
    Serial.print("The duration of the touch is (ms): ");
    Serial.println(timeArray_sync[counter_sync]);
    Serial.print("The time stamp of the sync stim: ");
    Serial.println(time1);
    raw_fsr = 0; // sets fsr to below threshold until the analog read is called again
    noTone(speaker1); // tone ends once force is below threshold
    digitalWrite(green_led, LOW); // turns off green led to let user know tactile stim is done
  }
}
