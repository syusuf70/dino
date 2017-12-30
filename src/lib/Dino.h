/*
  Library for dino ruby gem.
*/

#ifndef Dino_h
#define Dino_h
#define TXRX_SPI false

#include "Arduino.h"
#include <Servo.h>
#include <SPI.h>
#include "DinoLCD.h"
#include "DHT.h"
#include "OneWire.h"
#include "IRremote.h"

// SoftwareSerial doesn't work on the Due yet.
#if !defined(__SAM3X8E__)
  #include "DinoSerial.h"
#endif


// Allocate listener storage based on what board we're running.
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#  define PIN_COUNT 70
#  define SERVO_OFFSET 22
#elif defined(__SAM3X8E__)
#  define PIN_COUNT 66
#  define SERVO_OFFSET 22
#else
#  define PIN_COUNT 22
#  define SERVO_OFFSET 2
#endif

// Allocate listener storage for serial registers.
#define SPI_LISTENER_COUNT 4
#define SHIFT_LISTENER_COUNT 4

// Uncomment this line to enable debugging mode.
// #define debug true

class Dino {
  public:
    Dino();
    void setupWrite(void (*writeCallback)(char *str));
    void parse(char c);
    void updateListeners();

  private:
    // API-accessible functions.
    void setMode               ();         //cmd = 0
    void dWrite                ();         //cmd = 1
    void dRead                 (int pin);  //cmd = 2
    void aWrite                ();         //cmd = 3
    void aRead                 (int pin);  //cmd = 4
    void addDigitalListener    ();         //cmd = 5
    void addAnalogListener     ();         //cmd = 6
    void removeListener        ();         //cmd = 7
    void servoToggle           ();         //cmd = 8
    void servoWrite            ();         //cmd = 9
    void handleLCD             ();         //cmd = 10
    void shiftWrite            ();         //cmd = 11
    void handleSerial          ();         //cmd = 12
    void handleDHT             ();         //cmd = 13
    void ds18Read              ();         //cmd = 15
    void irSend                ();         //cmd = 16
    void tone                  ();         //cmd = 20
    void noTone                ();         //cmd = 21
    void shiftWrite            (int latchPin,  int len, byte dataPin, byte clockPin, byte *data);          //cmd = 22
    void shiftRead             (int latchPin,  int len, byte dataPin, byte clockPin, byte clockHighFirst); //cmd = 23
    void writeSPI              (int selectPin, int len, byte spiMode, uint32_t clockRate, byte *data);     //cmd = 24
    void readSPI               (int selectPin, int len, byte spiMode, uint32_t clockRate);                 //cmd = 25
    void addShiftListener      (int latchPin,  int len, byte dataPin, byte clockPin, byte clockHighFirst); //cmd = 26
    void addSPIListener        (int selectPin, int len, byte spiMode, uint32_t clockRate);                 //cmd = 27
    void removeRegisterListener();                                                                         //cmd = 28
    void reset                 ();  //cmd = 90
    void setRegisterDivider    ();  //cmd = 97
    void setAnalogResolution   ();  //cmd = 96
    void setAnalogDivider      ();  //cmd = 97
    void setHeartRate          ();  //cmd = 98

    // Serial flow control variables.
    uint8_t rcvBytes = 0;
    uint8_t rcvBuffer = 60;
    long    lastRcv = micros();
    long    rcvWindow  = 1000000;

    // Parser state storage and utility functions.
    char *messageFragments[4];
    byte fragmentIndex;
    int charIndex;
    boolean backslash;
    void append(char c);
    void process();

    // Parsed message storage.
    char cmdStr[5]; int cmd;
    char pinStr[5]; int pin;
    char valStr[5]; int val;
    byte auxMsg[512];

    // Value and response storage.
    int rval;
    char response[16];
    char newline[2];

    // Use a write callback from the main sketch to respond.
    void (*_writeCallback)(char *str);
    void writeResponse();

    // Arduino native library variables.
    Servo servos[12];

    // Internal timing variables and utility functions.
    long heartRate;
    long lastUpdate;
    unsigned int loopCount;
    unsigned int analogDivider;
    unsigned int registerDivider;
    long timeSince (long event);

    // Listeners correspond to raw pin number by array index, and store boolean. false == disabled.
    boolean analogListeners[PIN_COUNT];
    boolean digitalListeners[PIN_COUNT];

    // Create listeners for SPI registers.
    struct spiListener{
      byte     selectPin;
      byte     len;
      byte     spiMode;
      uint32_t clockRate;
      boolean  enabled;
    };
    spiListener spiListeners[SPI_LISTENER_COUNT];

    // Create listeners for ShiftIn registers.
    struct shiftListener{
      byte     latchPin;
      byte     len;
      byte     dataPin;
      byte     clockPin;
      byte     clockHighFirst;
      boolean  enabled;
    };
    shiftListener shiftListeners[SHIFT_LISTENER_COUNT];

    // Keep track of the last read values for digital listeners. Only write responses when changed.
    byte digitalListenerValues[PIN_COUNT];

    // Listener update functions.
    void updateDigitalListeners  ();
    void updateRegisterListeners ();
    void updateAnalogListeners   ();
};
#endif
