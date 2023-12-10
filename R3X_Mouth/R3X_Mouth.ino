#include <Adafruit_NeoPixel.h>
#include "SoftwareSerial.h"
#include <DFMiniMp3.h>

#define PIN_AUDIO_INPUT A0
#define PIN_TOUCH_SENSOR 2
#define LED_PIN 6
#define NUM_PIXELS 80
#define NUM_ROWS 12
#define CENT_X         NUM_COLS[NUM_ROWS / 2] / 2
#define CENT_Y         NUM_ROWS / 2


class Mp3Notify; 

SoftwareSerial secondarySerial(10, 11); // RX, TX
typedef DFMiniMp3<SoftwareSerial, Mp3Notify> DfMp3;
DfMp3 dfmp3(secondarySerial);
SoftwareSerial mySoftwareSerial(9, 8); // RX, TX

class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError([[maybe_unused]] DfMp3& mp3, uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);  

    // start next track
    track += 1;
    // this example will just start back over with 1 after track 3
    if (track > 3) 
    {
      track = 1;
    }
    dfmp3.playMp3FolderTrack(track);  // sd:/mp3/0001.mp3, sd:/mp3/0002.mp3, sd:/mp3/0003.mp3
  }
  static void OnPlaySourceOnline([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};

Adafruit_NeoPixel mouth = Adafruit_NeoPixel(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

int audioHighVoltage = 100;  // Configure the high voltage input value (in mV)
int maxBrightness = 150;      // Maximum brightness for the Neopixels
float dfvolume = 25;  // variable for volume to adjust audio sensitivity

int currentMode = 4;
unsigned long lastModeChangeTime = 0;
bool touchSensorPressed = false;

int NUM_COLS[12]= {8, 8, 8, 8, 8, 8, 8, 8, 6, 4, 4, 2};

void setup() {
  mouth.begin();
  mouth.show();  // Initialize all pixels to 'off'
  pinMode(PIN_TOUCH_SENSOR, INPUT_PULLUP);
  Serial.begin(115200);
  dfmp3.begin();
  dfmp3.reset();

  uint16_t version = dfmp3.getSoftwareVersion();
  Serial.print("version ");
  Serial.println(version);
  Serial.println(dfvolume/30);
  audioHighVoltage = audioHighVoltage * (dfvolume/30);
  Serial.println(audioHighVoltage);
  uint16_t volume = dfmp3.getVolume();
  Serial.print("volume ");
  Serial.println(volume);
  dfmp3.setVolume(dfvolume);
  
  uint16_t count = dfmp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
  Serial.print("files ");
  Serial.println(count);
  
  Serial.println("starting...");

  // start the first track playing
  dfmp3.playMp3FolderTrack(23);  // sd:/mp3/0001.mp3
}

void loop() {
  // Read the audio input voltage
  int audioLevel = analogRead(PIN_AUDIO_INPUT); // Convert to mV
  if (audioLevel <= 1){audioLevel=0;}
  //int audioLevel = random(0,1023); // only enable this for testing and disable the row above
  Serial.println(audioLevel);
  // Check if the touch sensor is pressed
  touchSensorPressed = digitalRead(PIN_TOUCH_SENSOR) == HIGH;

  // Change the mode if the touch sensor is pressed for 1 second
 // if (touchSensorPressed && (millis() - lastModeChangeTime >= 500)) {
 //   currentMode = (currentMode + 1) % 11; // There are 11 modes
 //   lastModeChangeTime = millis();
 // }

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
    case 4: // Blue to Red Grow
      updateBlueToRedGrow(audioLevel);
      break;
    case 5: // Rainbow
      updateRainbowPattern();
      break;
    case 6: // Blink Random Blue Lights
      blinkBlueLights();
      break;
    case 7: // Blink Random Red Lights
      blinkRedLights();
      break;
    case 8: // Blink Random Green Lights
      blinkGreenLights();
      break;
    case 9: // Blink Random White Lights
      blinkWhiteLights();
      break;
    case 10: // Blink Random Multi Lights
      blinkRandomLights();
      break;
  }
}

void updateRedPattern(int audioLevel) {
  for (int led = 0; led < NUM_PIXELS; led++) {
      int brightness = map(audioLevel, 0, audioHighVoltage, 0, maxBrightness);
      mouth.setPixelColor(led, brightness,0,0);
    }
    mouth.show();  // Update the NeoPixels
    delay(20);
}

void updateBluePattern(int audioLevel) {
  for (int led = 0; led < NUM_PIXELS; led++) {
      int brightness = map(audioLevel, 0, audioHighVoltage, 0, maxBrightness);
      mouth.setPixelColor(led, 0,0,brightness);
    }
    mouth.show();  // Update the NeoPixels
    delay(20);
}

void updateBlueGrow(int audioLevel) {
  int rectWidth = map(audioLevel, 0, audioHighVoltage, 0, NUM_COLS[NUM_ROWS / 2]);
  int rectHeight = map(audioLevel, 0, audioHighVoltage, 0, NUM_ROWS);
// Clear the matrix
  mouth.clear();

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
      mouth.setPixelColor(pixelIndex, mouth.Color(0, 0, maxBrightness)); // Set the color to blue
    }
  }
  mouth.show(); // Display the updated matrix
  delay(20);
}

void updateRedGrow(int audioLevel) {
  int rectWidth = map(audioLevel, 0, audioHighVoltage, 0, NUM_COLS[NUM_ROWS / 2]);
  int rectHeight = map(audioLevel, 0, audioHighVoltage, 0, NUM_ROWS);
// Clear the matrix
  mouth.clear();

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
      mouth.setPixelColor(pixelIndex, mouth.Color(maxBrightness, 0, 0)); // Set the color to red
    }
  }

  mouth.show(); // Display the updated matrix
  delay(20);
}

void updateBlueToRedGrow(int audioLevel) {
  int rectWidth = map(audioLevel, 0, audioHighVoltage, 0, NUM_COLS[NUM_ROWS / 2]);
  int rectHeight = map(audioLevel, 0, audioHighVoltage, 0, NUM_ROWS);
// Clear the matrix
  mouth.clear();

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
      // Calculate the color gradient from blue to red
      int blueValue = map(audioLevel, 0, audioHighVoltage, 255, 50);
      int redValue = 0;
      if (audioLevel >=(audioHighVoltage*0.9)){
        redValue = map(audioLevel, 0, audioHighVoltage, 0, 255);
      }
      Serial.println(redValue);
      mouth.setPixelColor(pixelIndex, mouth.Color(redValue, 0, blueValue)); // Set the color gradient
    }
  }

  mouth.show(); // Display the updated matrix
  delay(20);
}

void updateRainbowPattern() {
  for(long firstPixelHue = 0; firstPixelHue < 65535; firstPixelHue += 512) {
    mouth.rainbow(firstPixelHue);
    mouth.show(); // Update mouth with new contents
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
    mouth.setPixelColor(randomPixel, mouth.Color(0, 0, brightness));
    mouth.setBrightness(maxBrightness);
    isBlinking[randomPixel] = true;
  }

  mouth.show();
  delay(50); // Adjust the delay to control the blinking rate
  mouth.clear(); // Turn off all lights
  mouth.show();
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
    mouth.setPixelColor(randomPixel, mouth.Color(brightness, 0, 0));
    mouth.setBrightness(maxBrightness);
    isBlinking[randomPixel] = true;
  }

  mouth.show();
  delay(50); // Adjust the delay to control the blinking rate
  mouth.clear(); // Turn off all lights
  mouth.show();
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
    mouth.setPixelColor(randomPixel, mouth.Color(0, brightness, 0));
    mouth.setBrightness(maxBrightness);
    isBlinking[randomPixel] = true;
  }

  mouth.show();
  delay(50); // Adjust the delay to control the blinking rate
  mouth.clear(); // Turn off all lights
  mouth.show();
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
    mouth.setPixelColor(randomPixel, mouth.Color(brightness, brightness, brightness));
    mouth.setBrightness(maxBrightness);
    isBlinking[randomPixel] = true;
  }

  mouth.show();
  delay(50); // Adjust the delay to control the blinking rate
  mouth.clear(); // Turn off all lights
  mouth.show();
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
    mouth.setPixelColor(randomPixel, mouth.Color(r, g, b));
    isBlinking[randomPixel] = true;
  }

  mouth.show();
  delay(50); // Adjust the delay to control the blinking rate
  mouth.clear(); // Turn off all lights
  mouth.show();
}

void waitMilliseconds(uint16_t msWait)
{
  uint32_t start = millis();
  
  while ((millis() - start) < msWait)
  {
    // calling mp3.loop() periodically allows for notifications 
    // to be handled without interrupts
    dfmp3.loop(); 
    delay(1);
  }
}
