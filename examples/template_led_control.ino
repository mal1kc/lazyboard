// ------ led control ------

#define led 1 // defining pin number

void setup() {
  pinMode(led, OUTPUT); // setting pinmode as output
}

void loop() {
  digitalWrite(led, HIGH); // led on
  delay(500);
  digitalWrite(led, LOW); // led off
  delay(500);
}
