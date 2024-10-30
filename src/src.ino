// Test code for ESP32-S3 Inputs and Outputs

// Digital Inputs (with pull-ups)
const int PWR_SW_PIN = 14;
const int LW_BAND_SW_PIN = 12;
const int MW_BAND_SW_PIN = 6;
const int SLOW_DECODE_PIN = 5;
const int MED_DECODE_PIN = 11;
const int AUDIO_ON_OFF_PIN = 10;

// ADC Inputs
const int TUNING_PIN = 17; // ADC1 channel 0
const int VOLUME_PIN = 18; // ADC1 channel 1

// Digital Outputs
const int BACKLIGHT_PIN = 33;
const int POWER_LED_PIN = 35;
const int LW_LED_PIN = 36;
const int MW_LED_PIN = 37;
const int SW_LED_PIN = 38;
const int LOCK_LED_PIN = 43;
const int MORSE_LEDS_PIN = 44;

// PWM Outputs
const int CARRIER_PWM_PIN = 7;
const int DECODE_PWM_PIN = 3;

// PWM channels
const int CARRIER_PWM_CHANNEL = 0;
const int DECODE_PWM_CHANNEL = 1;

// PWM frequency and resolution
const int PWM_FREQUENCY = 5000; // 5 kHz
const int PWM_RESOLUTION = 8;   // 8-bit resolution

void setup()
{
  Serial.begin(115200);

  // Configure digital inputs with pull-ups
  pinMode(PWR_SW_PIN, INPUT_PULLUP);
  pinMode(LW_BAND_SW_PIN, INPUT_PULLUP);
  pinMode(MW_BAND_SW_PIN, INPUT_PULLUP);
  pinMode(SLOW_DECODE_PIN, INPUT_PULLUP);
  pinMode(MED_DECODE_PIN, INPUT_PULLUP);
  pinMode(AUDIO_ON_OFF_PIN, INPUT_PULLUP);

  // Configure digital outputs
  pinMode(BACKLIGHT_PIN, OUTPUT);
  pinMode(POWER_LED_PIN, OUTPUT);
  pinMode(LW_LED_PIN, OUTPUT);
  pinMode(MW_LED_PIN, OUTPUT);
  pinMode(SW_LED_PIN, OUTPUT);
  pinMode(LOCK_LED_PIN, OUTPUT);
  pinMode(MORSE_LEDS_PIN, OUTPUT);

  // Configure PWM outputs
  ledcSetup(CARRIER_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(CARRIER_PWM_PIN, CARRIER_PWM_CHANNEL);

  ledcSetup(DECODE_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(DECODE_PWM_PIN, DECODE_PWM_CHANNEL);
}

void loop()
{
  // Read digital inputs
  int pwr_sw_state = digitalRead(PWR_SW_PIN);
  int lw_band_sw_state = digitalRead(LW_BAND_SW_PIN);
  int mw_band_sw_state = digitalRead(MW_BAND_SW_PIN);
  int slow_decode_state = digitalRead(SLOW_DECODE_PIN);
  int med_decode_state = digitalRead(MED_DECODE_PIN);
  int audio_on_off_state = digitalRead(AUDIO_ON_OFF_PIN);

  // Print digital input states
  Serial.print("PWR_SW: ");
  Serial.print(pwr_sw_state);
  Serial.print(" | LW_BAND_SW: ");
  Serial.print(lw_band_sw_state);
  Serial.print(" | MW_BAND_SW: ");
  Serial.print(mw_band_sw_state);
  Serial.print(" | SLOW_DECODE: ");
  Serial.print(slow_decode_state);
  Serial.print(" | MED_DECODE: ");
  Serial.print(med_decode_state);
  Serial.print(" | AUDIO_ON_OFF: ");
  Serial.println(audio_on_off_state);

  // Read ADC inputs
  int tuning_value = analogRead(TUNING_PIN);
  int volume_value = analogRead(VOLUME_PIN);

  // Print ADC values
  Serial.print("TUNING: ");
  Serial.print(tuning_value);
  Serial.print(" | VOLUME: ");
  Serial.println(volume_value);

  // Toggle digital outputs
  digitalWrite(BACKLIGHT_PIN, HIGH);
  digitalWrite(POWER_LED_PIN, HIGH);
  digitalWrite(LW_LED_PIN, HIGH);
  digitalWrite(MW_LED_PIN, HIGH);
  digitalWrite(SW_LED_PIN, HIGH);
  digitalWrite(LOCK_LED_PIN, HIGH);
  digitalWrite(MORSE_LEDS_PIN, HIGH);
  delay(500);

  digitalWrite(BACKLIGHT_PIN, LOW);
  digitalWrite(POWER_LED_PIN, LOW);
  digitalWrite(LW_LED_PIN, LOW);
  digitalWrite(MW_LED_PIN, LOW);
  digitalWrite(SW_LED_PIN, LOW);
  digitalWrite(LOCK_LED_PIN, LOW);
  digitalWrite(MORSE_LEDS_PIN, LOW);
  delay(500);

  // Generate PWM signals
  for (int dutyCycle = 0; dutyCycle <= 255; dutyCycle += 5)
  {
    ledcWrite(CARRIER_PWM_CHANNEL, dutyCycle); // CARRIER_PWM_PIN
    ledcWrite(DECODE_PWM_CHANNEL, dutyCycle);  // DECODE_PWM_PIN
    delay(10);
  }
  for (int dutyCycle = 255; dutyCycle >= 0; dutyCycle -= 5)
  {
    ledcWrite(CARRIER_PWM_CHANNEL, dutyCycle);
    ledcWrite(DECODE_PWM_CHANNEL, dutyCycle);
    delay(10);
  }
}
