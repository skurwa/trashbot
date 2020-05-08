
void setup() {
	// pinMode(LED_BUILTIN, OUTPUT);
	DDRB = DDRB | 0b00100000;
}

void loop() {
	// digitalWrite(LED_BUILTIN, HIGH);
	PORTB = 0b00100000;
	delay(1000);
	// digitalWrite(LED_BUILTIN, LOW);
	PORTB = 0b00000000;
	delay(500);
}