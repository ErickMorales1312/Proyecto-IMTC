#define FLAME 32 // connect DO pin of sensor to this pin
#define ALARM 33 // pin 8 for Alarm 

void setup() {
  Serial.begin(9600);
  Serial.println("Fire Module Test");
  pinMode(FLAME, INPUT);//define FLAME input pin
  pinMode(ALARM, OUTPUT);//define ALARM output pin
 

void loop() {
  int fire = digitalRead(FLAME);// read FLAME sensor
  if(fire == HIGH)
  {
    digitalWrite(ALARM,HIGH);// set the buzzer ON
    Serial.println("Fire! Fire!");
  }else{
    digitalWrite(ALARM,LOW); // Set the buzzer OFF
    }
  delay(200);
}