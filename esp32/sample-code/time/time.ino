#include <time.h>

void setup() {
	// put your setup code here, to run once:
	Serial.begin(115200);
}

void loop() {
	// put your main code here, to run repeatedly:
	time_t seconds;
	seconds = time(NULL);
	Serial.println(seconds);
}
