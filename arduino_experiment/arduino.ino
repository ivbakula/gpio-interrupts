#define OUT_PIN		3
void setup() {

}

void loop() {
	digitalWrite(OUT_PIN, HIGH);
	delay(0.1);
	digitalWrite(OUT_PIN, LOW);
	delay(1000);
}
