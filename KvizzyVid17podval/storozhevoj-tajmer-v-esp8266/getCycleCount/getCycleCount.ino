// getCycleCount.ino

void setup() 
{
  Serial.begin(115200);
}

void loop() 
{
  Serial.println(ESP.getCycleCount());
  delay(5000);
}
