void setup() {
  pinMode(D1,OUTPUT);
  pinMode(D4,OUTPUT);
}
 
void loop() {
  digitalWrite(D1,HIGH);
  digitalWrite(D4,HIGH);
  delay(2000);
  digitalWrite(D1,LOW);
  digitalWrite(D4,LOW);
  delay(2000);
  digitalWrite(D1,HIGH);
  digitalWrite(D4,HIGH);
  delay(2000);
  digitalWrite(D1,LOW);
  digitalWrite(D4,LOW);
  delay(2000);
}