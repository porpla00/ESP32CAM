int buttonPin = 12;
int buzzer =  14;
int buttonState = 0;

void setup() {
  Serial.begin(115200);
  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin, INPUT);
}
void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    Serial.println("turn buzzer off");
    digitalWrite(buzzer, HIGH);
  }
  else {
    Serial.println("turn buzzer on");
    digitalWrite(buzzer, LOW);
  }
}