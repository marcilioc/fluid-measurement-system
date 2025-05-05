/*
  Weight Measurement Messaging
  Description: Relays Actuation Test
  Platform: Arduino Uno
  Owner: Caio Marcilio dos Santos
*/

int OUT1 = 6;
int OUT2 = 7;

char comm;
void setup() {
  pinMode(OUT1, OUTPUT);        // Sets Arduino pin 6 as OUTPUT
  pinMode(OUT2, OUTPUT);        // Sets Arduino pin 7 as OUTPUT
  Serial.begin(9600);           // Starts serial communication
}

void loop() {
  if(Serial.available()){
    comm = Serial.read();
    switch(comm){
      case 'w': digitalWrite(OUT1, HIGH); break;
      case 'q': digitalWrite(OUT1, LOW); break;
      case 's': digitalWrite(OUT2, HIGH); break;
      case 'a': digitalWrite(OUT2, LOW); break;
    }
  }
}
