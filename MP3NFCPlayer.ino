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
#include <stdio.h>

#include <SoftwareSerial.h>
#include <Wire.h>
#include <DFRobotDFPlayerMini.h>

#include <LiquidCrystal_I2C.h>

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
LiquidCrystal_I2C lcd(0x27, 16, 2);

//RFID reader
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

//mp3 player
SoftwareSerial playerSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

String currentRFID = "";
boolean RFIDChanged = false;

//mp3 control
boolean isPause = false;
int currentTrack = 0;
int currentFolder = 1;
String idleScreen = "HALLO MIRIAM";

boolean yellowIsPressed;
boolean redIsPressed;
boolean greenIsPressed;
boolean blueIsPressed;
boolean whiteIsPressed;
boolean blackIsPressed;

int buttonIgnoreTime = 6;
boolean ignoreButton = false;

struct RFIDInfo {
	String rfid;
	int folderNumber;
};

struct FolderInfo {
	//String id;
	String titel;
	int folderNumber;
	boolean music;
	boolean revert;
	int previous;
	int next;
	int tracks;
};

FolderInfo cards [100];
RFIDInfo rfids[200];

//for card init
int counter = 0;
int rfidcounter = 0;
int prev = 1;

void printDFPlayerDetail(uint8_t type, int value);

void addFolder(int folder, String titel, String rfid, boolean music, boolean revert) {
	struct FolderInfo info;
	info.folderNumber = folder;
	info.titel = titel;
	info.music = music;
	info.revert = revert;
	info.previous = prev;
	info.next = 1;
	info.tracks = myDFPlayer.readFileCountsInFolder(folder);
	if (info.tracks > 0) {
		cards[counter] = info;

		prev = folder;
		if (counter > 0) {
			cards[counter-1].next = folder;
		}
		counter++;

		addRFID(folder, rfid);
	}
}

void addRFID(int folder, String rfid) {
	struct RFIDInfo rinfo;
	rinfo.rfid = rfid;
	rinfo.folderNumber = folder;
	rfids[rfidcounter] = rinfo;
	rfidcounter++;
}

void initFolders() {
	addFolder(1, "CONNI", "", false, false);
	addFolder(2, "Bibi und Tina", "424311413292133", false, false); //gebastelt
	addFolder(3, "Fuchsbande", "", false, false);
	addFolder(4, "Siggi", "430311413292133", false, false); //Gebastelt (Kette)
	addFolder(5, "Wickie", "436311413292133", false, false); //Gebastelt Wicki
	addFolder(6, "Petterson und Findus", "429611413292133", false, false); //gebastelt
	addFolder(7, "Paw Patrol", "442611413292133", false, false); //Zuma (Kette)
	addFolder(8, "Mascha und der Bär", "447411413292133", false, false); //Pandabär Anhänger
	addFolder(9, "Leo Lausemaus", "", false, false); //
	addFolder(10, "Lauras Stern", "442311413292133", false, false); //Gebastelt Stern
	addFolder(11, "Janosch", "413318523419864129", false, false);//Bowser
	addFolder(12, "Der Mondbär", "", false, false);
	addFolder(13, "Bob der Baumeister", "", false, false);
	addFolder(14, "Lieselotte ", "435611413292133", false, false); //Klorollen Kuh
	addFolder(15, "Bibi Blocksberg ", "444511413292133", false, false);
	addFolder(16, "Hotzenplotz ", "", false, false);
	addFolder(17, "Neuer Hotzenplotz ", "", false, false);
	addFolder(18, "Die Schlümpfe ", "", false, false);

	addFolder(19, "Die Olchis", "", false, false);
	addFolder(20, "------", "", false, false);
	addFolder(21, "------", "", false, false);
	addFolder(22, "------", "", false, false);
	addFolder(23, "------", "", false, false);

	addFolder(40, "Figarino", "4625320222473129", false, true); //Wario

	addFolder(50, "Unter meinem Bett", "", true, false);
	addFolder(51, "Sing mit Conni", "", true, false);
	addFolder(52, "Augsburger Puppenkiste", "", true, false);
	addFolder(55, "Biene Maja Lieder", "", true, false);
	addFolder(70, "Meine Lieder für das 2. Lebensjahr", "", true, false);
	addFolder(71, "30 Spiel und Bewegungslieder", "", true, false);
	addFolder(80, "Kinder singen Weihnachtslieder", "", true, false);

	addFolder(90, "Die Ärzte", "641764325", true, false); //Karte
	//addRFID(99, "Sounds", "", false, false);

	cards[0].previous = cards[counter-1].folderNumber;

	//addRFID(6,  )
}

void setup() {
	//debug
	Serial.begin(115200);

	//LCD
	lcd.init();
	lcd.backlight();
	lcd.print(idleScreen);

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
	myDFPlayer.volume(29); //Set volume value. From 0 to 30

	initFolders();
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

struct FolderInfo getCurrectFolder() {
	struct FolderInfo folder;
	for (int i = 0; i < 100; i++) {
		FolderInfo tmp = cards[i];
		if (tmp.folderNumber == currentFolder) {
			folder = tmp;
		}
	}
	return folder;
}

void checkButtonActions() {

	//vorheriger Ordner
	struct FolderInfo currentFolderInfo = getCurrectFolder();
//	for (int i = 0; i < 100; i++) {
//		FolderInfo tmp = cards[i];
//		if (tmp.folderNumber == currentFolder) {
//			currentFolderInfo = tmp;
//		}
//	}

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
		playCurrentFolder(true);
	}
	if (whiteIsPressed) {
		currentFolder = currentFolderInfo.previous;
		currentTrack = 1;
		if (prevFolderInfo.revert) {
			currentTrack = prevFolderInfo.tracks;
		}
		playCurrentFolder(true);
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
		currentTrack = 0;
		lcd.clear();
		lcd.setCursor(1, 0);
		lcd.print(idleScreen);
	}
	if (yellowIsPressed) {
		if (isPause) {
			Serial.println("Player in pause -> start");
			myDFPlayer.start();
			isPause = false;
			LEDOn();
		} else {
			if (currentFolder == 99 || currentTrack == 0) {
				if (currentTrack == 0) {
					currentTrack = 1;
				}
				if (currentFolder == 99) {
					currentFolder = 1;
				}
				playCurrentFolder(true);
			} else {
				Serial.println("Player to pause");
				myDFPlayer.pause();
				isPause = true;
				LEDOff();
			}
		}
	}
}

void nextFileInFolder() {
	for (int i = 0; i < 100; i++) {
		FolderInfo tmp = cards[i];
		if (tmp.folderNumber == currentFolder) {
			if (tmp.revert) {
				if (currentTrack == 1) {
					currentTrack = tmp.tracks;
				} else {
					currentTrack = currentTrack - 1;
				}
			} else {
				if (currentTrack < tmp.tracks) {
					currentTrack = currentTrack + 1;
				} else {
					currentTrack = 1;
				}
			}
			break;
		}
	}
	playCurrentFolder(true);
}

void prevFileInFolder() {
	for (int i = 0; i < 100; i++) {
		FolderInfo tmp = cards[i];
		if (tmp.folderNumber == currentFolder) {
			if (tmp.revert) {
				if (currentTrack < tmp.tracks) {
					currentTrack = currentTrack + 1;
				} else {
					currentTrack = 1;
				}
			} else {
				if (currentTrack == 1) {
					currentTrack = tmp.tracks;
				} else {
					currentTrack = currentTrack - 1;
				}
			}
			break;
		}
	}
	playCurrentFolder(true);
}

void playNFCTrack() {
	Serial.print("playNFCTrack: Check currentRFID: ");
	Serial.println(currentRFID);

	FolderInfo currentF;
	boolean found = false;
	int tmpFolder = -1;
	for (int i = 0; i < 200; i++) {
		RFIDInfo tmp = rfids[i];
		if (tmp.rfid == currentRFID) {
			tmpFolder = tmp.folderNumber;
			found = true;
			break;
		}
	}
	for (int i = 0; i < 100; i++) {
		FolderInfo tmp = cards[i];
		if (tmp.folderNumber == tmpFolder) {
			currentF = tmp;
			break;
		}
	}

	int track = 1;
	if (currentF.revert) {
		track = currentF.tracks;
	}
	Serial.println(tmpFolder);
	if (found) {
		currentFolder = tmpFolder;
		currentTrack = track;
		myDFPlayer.stop();
		delay(100);
		playCurrentFolder(true);
	} else {
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(currentRFID);
		Serial.println("-keiner-");
		currentFolder = 99;
		currentTrack = 1;
		myDFPlayer.stop();
		delay(100);
		//playCurrentFolder(false);
	}
}

void playCurrentFolder(boolean setLCD) {
	Serial.print("Play track ");
	Serial.print(currentTrack);
	Serial.print(" of folder: ");
	Serial.println(currentFolder);
	myDFPlayer.playFolder(currentFolder, currentTrack);

	String folderNumber;
	if (setLCD) {
		boolean revert = false;
		int maxTracks = 1;
		String titel = idleScreen;
		for (int i = 0; i < 100; i++) {
			FolderInfo tmp = cards[i];
			if (tmp.folderNumber == currentFolder) {
				folderNumber = tmp.folderNumber;
				titel = tmp.titel;
				revert = tmp.revert;
				maxTracks = tmp.tracks;
				break;
			}
		}

		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print(titel);
		lcd.setCursor(0, 1);
		lcd.print(folderNumber);
		lcd.setCursor(13, 1);
		if (!revert) {
			lcd.print(currentTrack);
		} else {
			int revTrack = maxTracks - currentTrack + 1;
			lcd.print(revTrack);
		}
	}

	LEDOn();
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
		if (currentFolder < 99) {
			nextFileInFolder();
		} else {
			LEDOff();
		}
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

void printFolderInfo(int folder, int track) {
	Serial.print("Ordner: ");
	Serial.println(folder);
	Serial.print("Track: ");
	Serial.println(track);
}
