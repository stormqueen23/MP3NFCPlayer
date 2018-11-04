
/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read new NUID from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 *
 * Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 *
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 *
 * @license Released into the public domain.
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

//26.10.2018
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

//#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <DFRobotDFPlayerMini.h>

//#include <LCD.h>
//#include <LiquidCrystal_I2C.h>


#define SS_PIN 53
#define RST_PIN 5

#define GREEN_LED_R 30
#define GREEN_LED_L 28

#define B_YELLOW 33
#define B_RED 32
#define B_GREEN 26
#define B_BLUE 7
#define B_BLACK 34
#define B_WHITE 36

//LCD
//LiquidCrystal_I2C lcd(0x27);

//RFID reader
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
//MFRC522::MIFARE_Key key;

//mp3 player
SoftwareSerial playerSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;


//String lastID = "";
String currentRFID = "";
boolean RFIDChanged = false;

boolean isPlaying = false;
//int lastTrack = -1;
int currentTrack = -1;

String bowser = "413318523419864129";
String link = "493107219964128";
String wario = "4625320222473129";
String diddy_kong = "41913813022176129";
String wedding_peach = "470113342778128";
String karte = "641764325";
String clip = "2206016289";

boolean yellowIsPressed;
boolean redIsPressed;
boolean greenIsPressed;
boolean blueIsPressed;
boolean whiteIsPressed;
boolean blackIsPressed;

void printDFPlayerDetail(uint8_t type, int value);

void setup() {
  //debug
  Serial.begin(115200);

  //LCD
//  lcd.begin(16,2);
//  lcd.clear();
//  lcd.setCursor(0,0);
//  lcd.print("Hallo");
//  lcd.setCursor(0,1);
//  lcd.print("Miriam");
//  lcd.setBacklight(LOW);

  //RF-ID
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522

  //LED
  pinMode(GREEN_LED_R, OUTPUT);
  pinMode(GREEN_LED_L, OUTPUT);

  //Buttons
  pinMode(B_YELLOW, INPUT_PULLUP);
  pinMode(B_RED, INPUT_PULLUP);
  pinMode(B_GREEN, INPUT_PULLUP);
  pinMode(B_BLUE, INPUT_PULLUP);
  pinMode(B_BLACK, INPUT_PULLUP);
  pinMode(B_WHITE, INPUT_PULLUP);

  //MP3-Player
  playerSerial.begin(9600);
  if (!myDFPlayer.begin(playerSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(27); //Set volume value. From 0 to 30
  myDFPlayer.enableLoop();

}

boolean pause = false;

void loop() {
  checkButtons();
  checkCard1();
  //Serial.println(lastID);
  if (RFIDChanged) {
      checkTrack();
  }

  if (blackIsPressed) {
	myDFPlayer.playFolder(1, 1);
  }
  if (whiteIsPressed) {
	  myDFPlayer.playFolder(50, 1);
  }
  if (redIsPressed) {
    myDFPlayer.stop();
    currentRFID = "";
  }
  if (blueIsPressed) {
    myDFPlayer.next();
  }
  if (greenIsPressed) {
    myDFPlayer.previous();
  }
  if (yellowIsPressed) {
	  if (pause) {
		  myDFPlayer.start();
		  pause = false;
	  } else {
		  myDFPlayer.pause();
		  pause = true;
	  }
  }

  if (myDFPlayer.available()) {
    printDFPlayerDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }

  delay(50);
}








void checkCard() {
  RFIDChanged = false;
  String tmpCard;
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  MFRC522::StatusCode status;

  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent()) {
    return;
  }
  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    Serial.print("NUID nicht gelesen");
    return;

   Serial.print(F("PICC type: "));
   MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
   Serial.println(rfid.PICC_GetTypeName(piccType));

  // Store NUID into nuidPICC array
  for (byte i = 0; i < rfid.uid.size; i++) {
     // nuidPICC[i] = rfid.uid.uidByte[i];
    tmpCard += rfid.uid.uidByte[i];
  }

  Serial.print("currentRFID: ");
  Serial.println(currentRFID);


  if (tmpCard != currentRFID) {
    //RFID hat gewechselt
    if (tmpCard != "") {
      RFIDChanged = true;
      currentRFID = tmpCard;
      Serial.print("Karte gewechselt zu currentRFID: ");
      Serial.println(currentRFID);
    }
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


void PrintHex(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
     char tmp[16];
       for (int i=0; i<length; i++) {
         sprintf(tmp, "0x%.2X",data[i]);
         Serial.print(tmp); Serial.print(" ");
       }
}
uint8_t buf[10]= {};
MFRC522::Uid id;
MFRC522::Uid id2;
bool is_card_present = false;

void cpid(MFRC522::Uid *id){
  memset(id, 0, sizeof(MFRC522::Uid));
  memcpy(id->uidByte, rfid.uid.uidByte, rfid.uid.size);
  id->size = rfid.uid.size;
  id->sak = rfid.uid.sak;
}

bool cmpid(MFRC522::Uid *id1, MFRC522::Uid *id2){
  return memcmp(id1, id2, sizeof(MFRC522::Uid));
}

void deregister_card(){
  is_card_present = false;
  memset(&id,0, sizeof(id));
}
uint8_t control = 0x00;


void checkCard1() {
  RFIDChanged = false;
  String tmpCard;
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Look for new cards
  if ( !rfid.PICC_IsNewCardPresent()) {
    return;
  }
  if ( !rfid.PICC_ReadCardSerial()) {
    return;
  }
  //PrintHex(id.uidByte, id.size);
  //Serial.println("hello");
  bool result = true;
  uint8_t buf_len=4;
  cpid(&id);
  Serial.print("NewCard ");
  PrintHex(id.uidByte, id.size);
  for (byte i = 0; i < rfid.uid.size; i++) {
    tmpCard += rfid.uid.uidByte[i];
  }
  Serial.print(tmpCard);
  Serial.println("");

  if (tmpCard != currentRFID) {
    //RFID hat gewechselt
    if (tmpCard != "") {
      RFIDChanged = true;
      currentRFID = tmpCard;
      Serial.print("Karte gewechselt zu currentRFID: ");
      Serial.println(currentRFID);
    }
  }
  /*
  while(true){
    control=0;
    for(int i=0; i<3; i++){
      if(!rfid.PICC_IsNewCardPresent()){
        if(rfid.PICC_ReadCardSerial()){
          //Serial.print('a');
          control |= 0x16;
        }
        if(rfid.PICC_ReadCardSerial()){
          //Serial.print('b');
          control |= 0x16;
        }
        //Serial.print('c');
          control += 0x1;
      }
      //Serial.print('d');
      control += 0x4;
    }

    //Serial.println(control);
    if(control == 13 || control == 14){
      //card is still there
    } else {
      break;
    }
  }
  Serial.println("CardRemoved");
  delay(500); //change value if you want to read cards faster
*/

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

}

void checkTrack() {
  Serial.print("Check currentRFID: ");
  Serial.println(currentRFID);
  if (currentRFID == bowser) {
    Serial.print("Bowser");
    myDFPlayer.playFolder(90, 1);
  }
  if (currentRFID == karte) {
    Serial.print("Karte");
    myDFPlayer.playFolder(1, 1);
  }
  if (currentRFID == clip) {
    Serial.print("Clip");
    myDFPlayer.playFolder(5, 1);
  }
  if (currentRFID == wedding_peach) {
    Serial.print("Hochzeits Peach");
    myDFPlayer.playFolder(1, 1);
  }
}



void checkButtons() {
  yellowIsPressed = isYellowPressed();
  redIsPressed = isRedPressed();
  greenIsPressed = isGreenPressed();
  blueIsPressed = isBluePressed();
  blackIsPressed = isBlackPressed();
  whiteIsPressed = isWhitePressed();

  if (yellowIsPressed) {
    Serial.println("GELB");
  }
  if (redIsPressed) {
    Serial.println("ROT");
  }
  if (greenIsPressed) {
    Serial.println("GRUEN");
  }
  if (blueIsPressed) {
    Serial.println("BLAU");
  }
  if (blackIsPressed) {
    Serial.println("SCHWARZ");
  }
  if (whiteIsPressed) {
    Serial.println("WEISS");
  }
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

/**
 * MP3 Player output stuff
 */
void printDFPlayerDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

}
boolean isYellowPressed() {
  return !digitalRead(B_YELLOW);
}

boolean isRedPressed() {
  return !digitalRead(B_RED);
}

boolean isGreenPressed() {
  return !digitalRead(B_GREEN);
}

boolean isBluePressed() {
  return !digitalRead(B_BLUE);
}

boolean isBlackPressed() {
  return !digitalRead(B_BLACK);
}

boolean isWhitePressed() {
  return !digitalRead(B_WHITE);
}
