#include <Wire.h>

typedef void (*funcptr)(int);


// Type of messages that can be sent
//
enum MessageType {
  NO_MESSAGE,
  INT_MESSAGE,
  STRING_MESSAGE
};

// This class is a wrapper for the Wire library
//
class Communication {
private:
  int address = -1;
  void messageReceived(int bytes);
  inline static char message[32];  // 32 bytes is the limit of the Wire library implementation
  inline static int messageSize;
  inline static int numberReceived;

public:
  Communication(int myAddress);
  ~Communication();
  void Transmit(int sendToAddress, int command);
  void Transmit(int sendToAddress, char *message);
  MessageType Received(void);
  char *GetStringMessage(void);
  int GetIntMessage(void);
};

// Indicate if a message has been received
// NO_MESSAGE = No Message received!
//
MessageType Communication::Received(void) {
  if (messageSize == 0) {
    return NO_MESSAGE;
  }
  if (messageSize == 1) {
    return INT_MESSAGE;
  }
  return STRING_MESSAGE;
}


// Function to retreive a string message
//
char *Communication::GetStringMessage(void) {
  messageSize = 0;
  return message;
}

// Function to retrieve an INT
//
int Communication::GetIntMessage(void) {
  messageSize = 0;
  return numberReceived;
}

Communication::Communication(int myAddress) {
  address = myAddress;
  Wire.begin(myAddress);
  Wire.setWireTimeout();

  Wire.onReceive((funcptr)&messageReceived);
  messageSize = 0;
}

Communication::~Communication() {
}

// Transmit an INT
//
void Communication::Transmit(int sendToAddress, int command) {
  Wire.beginTransmission(sendToAddress);
  Wire.write(command);
  Wire.endTransmission();
}

// Transmit a string
//
void Communication::Transmit(int sendToAddress, char *message) {
  Wire.beginTransmission(sendToAddress);  // transmit to device #9

  for (int i = 0; i < strlen(message); i++) {
    Wire.write(message[i]);
  }

  Wire.endTransmission();  // stop transmitting
}

// Handler called when a command is received
// can lock up the device, we do the minimium
void Communication::messageReceived(int bytes) {
  if (bytes == 1) {
    numberReceived = Wire.read();
    this->messageSize = bytes;
  } else {
    int i = 0;
    while (Wire.available()) {
      message[i] = Wire.read();
      i++;
    }
    message[i] = 0;
    this->messageSize = bytes;
  }
}