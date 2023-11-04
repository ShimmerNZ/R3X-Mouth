#include <Adafruit_NeoPixel.h>

#define PIN_AUDIO_INPUT A0
#define PIN_TOUCH_SENSOR 2
#define LED_PIN 6
#define NUM_PIXELS 80
#define NUM_ROWS 12
#define CENT_X         NUM_COLS[NUM_ROWS / 2] / 2
#define CENT_Y         NUM_ROWS / 2

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

int audioHighVoltage = 3300;  // Configure the high voltage input value (in mV)
int maxBrightness = 150;      // Maximum brightness for the Neopixels

int currentMode = 0;
unsigned long lastModeChangeTime = 0;
bool touchSensorPressed = false;

int NUM_COLS[12]= {8, 8, 8, 8, 8, 8, 8, 8, 6, 4, 4, 2};

void setup() {
  strip.begin();
  strip.show();  // Initialize all pixels to 'off'
  Serial.begin(115200);
  pinMode(PIN_TOUCH_SENSOR, INPUT_PULLUP);
}

void loop() {
  // Read the audio input voltage
  //int audioLevel = analogRead(PIN_AUDIO_INPUT) * 3300 / 1023; // Convert to mV
  int audioLevel = random(1050);
  Serial.println(audioLevel);

  // Check if the touch sensor is pressed
  touchSensorPressed = digitalRead(PIN_TOUCH_SENSOR) == HIGH;

  // Change the mode if the touch sensor is pressed for 1 second
  if (touchSensorPressed && (millis() - lastModeChangeTime >= 500)) {
    currentMode = (currentMode + 1) % 10; // There are 10 modes
    lastModeChangeTime = millis();
  }

  // Update Neopixel lights based on the current mode
  switch (currentMode) {
    case 0: // Red Color
      updateRedPattern(audioLevel);
      break;
    case 1: // Blue Color
      updateBluePattern(audioLevel);
      break;
    case 2: // Blue Grow
      updateBlueGrow(audioLevel);
      break;
    case 3: // Red Grow
      updateRedGrow(audioLevel);
      break;
    case 4: // Rainbow
      updateRainbowPattern();
      break;
    case 5: // Blink Random Blue Lights
      blinkBlueLights();
      break;
    case 6: // Blink Random Red Lights
      blinkRedLights();
      break;
    case 7: // Blink Random Green Lights
      blinkGreenLights();
      break;
    case 8: // Blink Random White Lights
      blinkWhiteLights();
      break;
    case 9: // Blink Random Multi Lights
      blinkRandomLights();
      break;
  }
}

void updateRedPattern(int audioLevel) {
  for (int row = 0; row < NUM_ROWS; row++) {
    int numCols = NUM_COLS[row];
    for (int col = 0; col < numCols; col++) {
      int pixelIndex = row * 8 + col;
      int brightness = map(audioLevel, 0, audioHighVoltage, 0, maxBrightness);
      strip.setPixelColor(pixelIndex, strip.Color(brightness, 0, 0));
      strip.setBrightness(maxBrightness);
    }
  }
    strip.show();  // Update the NeoPixels
}

void updateBluePattern(int audioLevel) {
  for (int row = 0; row < NUM_ROWS; row++) {
    int numCols = NUM_COLS[row];
    for (int col = 0; col < numCols; col++) {
      int pixelIndex = row * 8 + col;
      int brightness = map(audioLevel, 0, audioHighVoltage, 0, maxBrightness);
      strip.setPixelColor(pixelIndex, strip.Color(0, 0, brightness));
      strip.setBrightness(maxBrightness);
    }
  }
  strip.show();
}

void updateBlueGrow(int audioLevel) {
  int rectWidth = map(audioLevel, 0, 1023, 0, NUM_COLS[NUM_ROWS / 2]);
  int rectHeight = map(audioLevel, 0, 1023, 0, NUM_ROWS);
// Clear the matrix
  strip.clear();

  int startX = CENT_X - (rectWidth / 2);
  int startY = CENT_Y - (rectHeight / 2)-1 ;
  int endX = startX + rectWidth;
  int endY = startY + rectHeight;

  for (int x = startX; x < endX; x++) {
    for (int y = startY; y < endY; y++) {
      int rowOffset = 0;
      for (int i = 0; i < y; i++) {
        rowOffset += NUM_COLS[i];
      }
      int pixelIndex = rowOffset + x;
      strip.setPixelColor(pixelIndex, strip.Color(0, 0, maxBrightness)); // Set the color to blue
    }
  }

  strip.show(); // Display the updated matrix
  delay(50);
}

void updateRedGrow(int audioLevel) {
  int rectWidth = map(audioLevel, 0, 1023, 0, NUM_COLS[NUM_ROWS / 2]);
  int rectHeight = map(audioLevel, 0, 1023, 0, NUM_ROWS);
// Clear the matrix
  strip.clear();

  int startX = CENT_X - (rectWidth / 2);
  int startY = CENT_Y - (rectHeight / 2)-1 ;
  int endX = startX + rectWidth;
  int endY = startY + rectHeight;

  for (int x = startX; x < endX; x++) {
    for (int y = startY; y < endY; y++) {
      int rowOffset = 0;
      for (int i = 0; i < y; i++) {
        rowOffset += NUM_COLS[i];
      }
      int pixelIndex = rowOffset + x;
      strip.setPixelColor(pixelIndex, strip.Color(maxBrightness, 0, 0)); // Set the color to red
    }
  }

  strip.show(); // Display the updated matrix
  delay(50);
}

void updateRainbowPattern() {
  for(long firstPixelHue = 0; firstPixelHue < 65535; firstPixelHue += 1028) {
    strip.rainbow(firstPixelHue);
    strip.show(); // Update strip with new contents
  }
}

void blinkBlueLights() {
  // Number of lights to blink (randomly selected)
  int numLights = random(1, NUM_PIXELS/4 + 1);

  // Create an array to store which lights are blinking
  boolean isBlinking[NUM_PIXELS];
  for (int i = 0; i < NUM_PIXELS; i++) {
    isBlinking[i] = false;
  }

  // Turn on random lights with random brightness in blue
  for (int i = 0; i < numLights; i++) {
    int randomPixel;
    do {
      randomPixel = random(NUM_PIXELS);
    } while (isBlinking[randomPixel]); // Ensure it's not already blinking

    int brightness = random(maxBrightness + 1); // Random brightness
    strip.setPixelColor(randomPixel, strip.Color(0, 0, brightness));
    strip.setBrightness(maxBrightness);
    isBlinking[randomPixel] = true;
  }

  strip.show();
  delay(50); // Adjust the delay to control the blinking rate
  strip.clear(); // Turn off all lights
  strip.show();
}

void blinkRedLights() {
  // Number of lights to blink (randomly selected)
  int numLights = random(1, NUM_PIXELS/4 + 1);

  // Create an array to store which lights are blinking
  boolean isBlinking[NUM_PIXELS];
  for (int i = 0; i < NUM_PIXELS; i++) {
    isBlinking[i] = false;
  }

  // Turn on random lights with random brightness in blue
  for (int i = 0; i < numLights; i++) {
    int randomPixel;
    do {
      randomPixel = random(NUM_PIXELS);
    } while (isBlinking[randomPixel]); // Ensure it's not already blinking

    int brightness = random(maxBrightness + 1); // Random brightness
    strip.setPixelColor(randomPixel, strip.Color(brightness, 0, 0));
    strip.setBrightness(maxBrightness);
    isBlinking[randomPixel] = true;
  }

  strip.show();
  delay(50); // Adjust the delay to control the blinking rate
  strip.clear(); // Turn off all lights
  strip.show();
}

void blinkGreenLights() {
  // Number of lights to blink (randomly selected)
  int numLights = random(1, NUM_PIXELS/4 + 1);

  // Create an array to store which lights are blinking
  boolean isBlinking[NUM_PIXELS];
  for (int i = 0; i < NUM_PIXELS; i++) {
    isBlinking[i] = false;
  }

  // Turn on random lights with random brightness in blue
  for (int i = 0; i < numLights; i++) {
    int randomPixel;
    do {
      randomPixel = random(NUM_PIXELS);
    } while (isBlinking[randomPixel]); // Ensure it's not already blinking

    int brightness = random(maxBrightness + 1); // Random brightness
    strip.setPixelColor(randomPixel, strip.Color(0, brightness, 0));
    strip.setBrightness(maxBrightness);
    isBlinking[randomPixel] = true;
  }

  strip.show();
  delay(50); // Adjust the delay to control the blinking rate
  strip.clear(); // Turn off all lights
  strip.show();
}

void blinkWhiteLights() {
  // Number of lights to blink (randomly selected)
  int numLights = random(1, NUM_PIXELS/4 + 1);

  // Create an array to store which lights are blinking
  boolean isBlinking[NUM_PIXELS];
  for (int i = 0; i < NUM_PIXELS; i++) {
    isBlinking[i] = false;
  }

  // Turn on random lights with random brightness in blue
  for (int i = 0; i < numLights; i++) {
    int randomPixel;
    do {
      randomPixel = random(NUM_PIXELS);
    } while (isBlinking[randomPixel]); // Ensure it's not already blinking

    int brightness = random(maxBrightness + 1); // Random brightness
    strip.setPixelColor(randomPixel, strip.Color(brightness, brightness, brightness));
    strip.setBrightness(maxBrightness);
    isBlinking[randomPixel] = true;
  }

  strip.show();
  delay(50); // Adjust the delay to control the blinking rate
  strip.clear(); // Turn off all lights
  strip.show();
}

void blinkRandomLights() {
  // Number of lights to blink (randomly selected)
  int numLights = random(1, NUM_PIXELS/4 + 1);

  // Create an array to store which lights are blinking
  boolean isBlinking[NUM_PIXELS];
  for (int i = 0; i < NUM_PIXELS; i++) {
    isBlinking[i] = false;
  }

  // Turn on random lights with random brightness in blue
  for (int i = 0; i < numLights; i++) {
    int randomPixel;
    do {
      randomPixel = random(NUM_PIXELS);
    } while (isBlinking[randomPixel]); // Ensure it's not already blinking

    int brightness = random(maxBrightness + 1); // Random brightness
    int r =random(0,maxBrightness);
    int g =random(0,maxBrightness);
    int b =random(0,maxBrightness);
    strip.setPixelColor(randomPixel, strip.Color(r, g, b));
    isBlinking[randomPixel] = true;
  }

  strip.show();
  delay(50); // Adjust the delay to control the blinking rate
  strip.clear(); // Turn off all lights
  strip.show();
}