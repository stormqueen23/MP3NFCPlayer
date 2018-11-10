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

//mp3 control
//boolean isPlaying = false;
boolean isPause = false;
int currentTrack = 1;
int currentFolder = 1;


boolean yellowIsPressed;
boolean redIsPressed;
boolean greenIsPressed;
boolean blueIsPressed;
boolean whiteIsPressed;
boolean blackIsPressed;

int buttonIgnoreTime = 6;
boolean ignoreButton = false;


struct FolderInfo {
	String id;
	String titel;
	int folderNumber;
	boolean music;
	boolean revert;
	int previous;
	int next;
	int tracks;
};

FolderInfo cards [100];
//for card init
int counter = 0;
int prev = 1;

void printDFPlayerDetail(uint8_t type, int value);

void addRFID(int folder,String titel, String rfid, boolean music, boolean revert) {
	struct FolderInfo info;
	info.folderNumber = folder;
	info.titel = titel;
	info.id = rfid;
	info.music = music;
	info.revert = revert;
	info.previous = prev;
	info.next = 1;
	info.tracks = myDFPlayer.readFileCountsInFolder(folder);
	cards[counter] = info;

	prev = folder;
	if (counter > 0) {
		cards[counter-1].next = folder;
	}
	counter++;
}

void initRFIDs() {
	/*
	 * 470113342778128 = Wedding Peach
	 * 411917417016064129 = Dorfbewohner
	 * 413318523419864129 = Bowser
	 * 4625320222473129 = Wario
	 * 41913813022176129 = Diddy Kong
	 * 42401481012572128 = Rosalina
	 * 4141192178173128 = Schaufelritter
	 * 493107219964128 = Link
	 * 41735598273128 = Hund
	 * 46611212212472128 = Cpt. Olima
	 * 4735422619864128 = Tintenfisch Mädchen
	 *
	 * 641764325 = Karte
	 */


	addRFID(1, "Conni", "411917417016064129", false, false); //Dorfbewohner
	addRFID(2, "Bibi und Tina", "", false, false);
	addRFID(3, "Fuchsbande", "4735422619864128", false, false); //Tintenfisch Mädchen
	addRFID(4, "Siggi", "4141192178173128", false, false); //Schaufelritter
	addRFID(5, "Wickie", "493107219964128", false, false); //Link
	addRFID(6, "Petterson und Findus", "429611413292133", false, false); //gebastelt
	addRFID(7, "Paw Patrol", "46611212212472128", false, false); //Pikmin
	addRFID(8, "Mascha und der Bär", "41735598273128", false, false); //Hund
	addRFID(9, "Leo Lausemaus", "41913813022176129", false, false); //Diddy Kong
	addRFID(10, "Lauras Stern", "42401481012572128", false, false); //Rosalina
	addRFID(11, "Janosch", "413318523419864129", false, false);//Bowser
	addRFID(12, "Der Mondbär", "470113342778128", false, false); // Wedding Peach
	addRFID(13, "Bob der Baumeister", "", false, false);

	addRFID(40, "Figarino", "4625320222473129", false, true); //Wario

	addRFID(50, "Unter meinem Bett", "", true, false);
	addRFID(51, "Sing mit Conni", "", true, false);
	addRFID(52, "Augsburger Puppenkiste", "", true, false);
	addRFID(55, "Biene Maja Lieder", "", true, false);
	addRFID(70, "Meine Lieder für das 2. Lebensjahr", "", true, false);
	addRFID(71, "30 Spiel und Bewegungslieder", "", true, false);
	addRFID(80, "Kinder singen Weihnachtslieder", "", true, false);

	addRFID(90, "Die Ärzte", "641764325", true, false); //Karte
	//addRFID(99, "Sounds", "", false, false);

	cards[0].previous = cards[counter-1].folderNumber;
}

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
	if (!myDFPlayer.begin(playerSerial)) { //Use softwareSerial to communicate with mp3.
		Serial.println(F("Unable to begin:"));
		Serial.println(F("1.Please recheck the connection!"));
		Serial.println(F("2.Please insert the SD card!"));
		while (true) {
			delay(0); // Code to compatible with ESP8266 watch dog.
		}
	}
	Serial.println(F("DFPlayer Mini online."));
	myDFPlayer.volume(28); //Set volume value. From 0 to 30

	initRFIDs();
	currentRFID = "";
}

void loop() {
	readButtons();
	checkCard();

	if (RFIDChanged) {
		myDFPlayer.stop();
		LEDOff();
		playNFCTrack();
	} else if (!ignoreButton) {
		checkButtonActions();
	}

	if (myDFPlayer.available()) {
		printDFPlayerDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
	}

	delay(50);
}

void checkButtonActions() {

	//vorheriger Ordner
	FolderInfo currentFolderInfo;
	for (int i = 0; i < 100; i++) {
		FolderInfo tmp = cards[i];
		if (tmp.folderNumber == currentFolder) {
			currentFolderInfo = tmp;
		}
	}

	FolderInfo nextFolderInfo;
	FolderInfo prevFolderInfo;
	for (int i = 0; i < 100; i++) {
		FolderInfo tmp = cards[i];
		if (tmp.folderNumber == currentFolderInfo.next) {
			nextFolderInfo = tmp;
		}
		if (tmp.folderNumber == currentFolderInfo.previous) {
			prevFolderInfo = tmp;
		}
	}

	if (blackIsPressed) {
		currentFolder = currentFolderInfo.next;
		currentTrack = 1;
		if (nextFolderInfo.revert) {
			currentTrack = nextFolderInfo.tracks;
		}
		Serial.print("Nächster Ordner: ");
		Serial.println(currentFolder);
		Serial.print("Track: ");
		Serial.println(currentTrack);
		myDFPlayer.playFolder(currentFolder, currentTrack);
		LEDOn();
		if (currentRFID.length() > 0) {

		}
	}
	if (whiteIsPressed) {
		currentFolder = currentFolderInfo.previous;
		currentTrack = 1;
		if (prevFolderInfo.revert) {
			currentTrack = prevFolderInfo.tracks;
		}
		Serial.print("Vorheriger Ordner: ");
		Serial.println(currentFolder);
		Serial.print("Track: ");
		Serial.println(currentTrack);
		myDFPlayer.playFolder(currentFolder, currentTrack);
		LEDOn();
	}
	if (blueIsPressed) {
		Serial.println("Nächster Track");
		nextFileInFolder();
		LEDOn();
	}
	if (greenIsPressed) {
		Serial.println("Vorheriger Track");
		prevFileInFolder();
		LEDOn();
	}
	if (redIsPressed) {
		myDFPlayer.stop();
		LEDOff();
		currentRFID = "";
	}
	if (yellowIsPressed) {
		if (isPause) {
			myDFPlayer.start();
			isPause = false;
			LEDOn();
		} else {
			if (currentRFID.length() == 0) {
//				currentFolder = 1;
//				currentTrack = 1;
				myDFPlayer.playFolder(currentFolder, currentTrack);
				//myDFPlayer.loopFolder(currentFolder);
			}
			myDFPlayer.pause();
			isPause = true;
			LEDOff();
		}
	}
}

void nextFileInFolder() {
	for (int i = 0; i < 100; i++) {
		FolderInfo tmp = cards[i];
		if (tmp.folderNumber == currentFolder) {
			Serial.print("Aktueller Ordner: ");
			Serial.println(currentFolder);
			if (tmp.revert) {
				if (currentTrack == 1) {
					currentTrack = tmp.tracks;
					Serial.println("A-R");
				} else {
					currentTrack = currentTrack - 1;
					Serial.println("B-R");
				}
			} else {
				if (currentTrack < tmp.tracks) {
					currentTrack = currentTrack + 1;
					Serial.println("A");
				} else {
					currentTrack = 1;
					Serial.println("B");
				}
			}
			break;
		}
	}
	Serial.print("Starte Ordner ");
	Serial.print(currentFolder);
	Serial.print(" Lied ");
	Serial.println(currentTrack);
	myDFPlayer.playFolder(currentFolder, currentTrack);
}

void prevFileInFolder() {
	for (int i = 0; i < 100; i++) {
		FolderInfo tmp = cards[i];
		if (tmp.folderNumber == currentFolder) {
			Serial.print("Aktueller Ordner: ");
			Serial.println(currentFolder);
			if (tmp.revert) {
				if (currentTrack < tmp.tracks) {
					currentTrack = currentTrack + 1;
					Serial.println("A-R");
				} else {
					currentTrack = 1;
					Serial.println("B-R");
				}
			} else {
				if (currentTrack == 1) {
					currentTrack = tmp.tracks;
					Serial.println("A");
				} else {
					currentTrack = currentTrack - 1;
					Serial.println("B");
				}
			}
			break;
		}
	}
	Serial.print("Starte Ordner ");
	Serial.print(currentFolder);
	Serial.print(" Lied ");
	Serial.println(currentTrack);
	myDFPlayer.playFolder(currentFolder, currentTrack);
}

void playNFCTrack() {
	Serial.print("checkTrack: Check currentRFID: ");
	Serial.println(currentRFID);
	Serial.print("Zugeordneter Ordner: ");
	FolderInfo currentF;
	for (int i = 0; i < 100; i++) {
		FolderInfo tmp = cards[i];
		if (tmp.id == currentRFID) {
			currentF = tmp;
			break;
		}
	}
	int tmpFolder = currentF.folderNumber;
	int track = 1;
	if (currentF.revert) {
		track = currentF.tracks;
	}
	if (tmpFolder > 0) {
		Serial.println(tmpFolder);
		currentFolder = tmpFolder;
		currentTrack = track;
		Serial.print("Play track ");
		Serial.print(currentTrack);
		Serial.print(" of folder: ");
		Serial.println(currentFolder);
		myDFPlayer.stop();
		delay(100);
		myDFPlayer.playFolder(currentFolder, currentTrack);
		//myDFPlayer.loopFolder(currentFolder);
		LEDOn();
	} else {
		Serial.println("-keiner-");
		myDFPlayer.playFolder(99, 1);
	}
}


void checkCard() {
	RFIDChanged = false;
	String tmpCard;

	// Look for new cards
	if (!rfid.PICC_IsNewCardPresent()) {
		return;
	}
	if (!rfid.PICC_ReadCardSerial()) {
		return;
	}

	Serial.print("Karte gewechselt zu ");

	for (byte i = 0; i < rfid.uid.size; i++) {
		tmpCard += rfid.uid.uidByte[i];
	}
	Serial.println(tmpCard);

	if (tmpCard != currentRFID) {
		//RFID hat gewechselt
		if (tmpCard != "") {
			RFIDChanged = true;
			currentRFID = tmpCard;
		}
	}

	rfid.PICC_HaltA();
	rfid.PCD_StopCrypto1();

}

int getFolder(String rfid) {
	int result = 0;
	for (int i = 0; i < 100; i++) {
		FolderInfo tmp = cards[i];
			if (tmp.id == rfid) {
				result = tmp.folderNumber;
			}
	}
	return result;
}

boolean startButtonTimer = false;

void readButtons() {
	yellowIsPressed = isYellowPressed();
	redIsPressed = isRedPressed();
	greenIsPressed = isGreenPressed();
	blueIsPressed = isBluePressed();
	blackIsPressed = isBlackPressed();
	whiteIsPressed = isWhitePressed();

	if (yellowIsPressed) {
		Serial.println("GELB");
		startButtonTimer = true;
	}
	if (redIsPressed) {
		Serial.println("ROT");
		startButtonTimer = true;
	}
	if (greenIsPressed) {
		Serial.println("GRUEN");
		startButtonTimer = true;
	}
	if (blueIsPressed) {
		Serial.println("BLAU");
		startButtonTimer = true;
	}
	if (blackIsPressed) {
		Serial.println("SCHWARZ");
		startButtonTimer = true;
	}
	if (whiteIsPressed) {
		Serial.println("WEISS");
		startButtonTimer = true;
	}

	if (buttonIgnoreTime < 6) {
		ignoreButton = true;
		Serial.println("Ignoriere Buttoneingabe");
	}
	if (startButtonTimer) {
		buttonIgnoreTime = buttonIgnoreTime-1;
	}

	if (buttonIgnoreTime == 0) {
		buttonIgnoreTime = 6;
		ignoreButton = false;
		startButtonTimer = false;
	}
}

/**
 * MP3 Player output stuff
 */
void printDFPlayerDetail(uint8_t type, int value) {
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
		nextFileInFolder();
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

void LEDOn() {
	digitalWrite(GREEN_LED_L, HIGH);
	digitalWrite(GREEN_LED_R, HIGH);
}
void LEDOff() {
	digitalWrite(GREEN_LED_L, LOW);
	digitalWrite(GREEN_LED_R, LOW);
}
