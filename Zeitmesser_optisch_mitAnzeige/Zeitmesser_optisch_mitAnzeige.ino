
/* INCLUDE ---------------------------------------------------------------------------------
   Hier binden wir externe Bibliotheken ein. 
   Achtung, im Arduino Studio muss die Bibliothek LiquidCrystal_I2C installiert sein.
---------------------------------------------------------------------------------------------*/   
#include <Wire.h>  // binde Bibliothek Wire fuer I2C ein
#include <LiquidCrystal_I2C.h> // binde Bibliothek LiquidCrystal_I2C ein

/* DEFINE ---------------------------------------------------------------------------------
   Hier legen wir grundlege Parameter für das Programm fest.
   Im zweiten Teil "übersetzten" wir englische Kontroll-Commandos in deutsche
---------------------------------------------------------------------------------------------*/   

#define EINGANG_LICHTSENSOR A0		// Analoger Eingang auf dem Arduino Bord der am Lichtsensor angebracht ist

#define LCD_DISPLAY_ZEICHEN 16		// Anzahl der Zeichen auf dem LCD Display
#define LCD_DISPLAY_ZEILEN  2		// Anzahl der Reihen auf dem LCD Display

#define LCD_POS_SIGNAL      0 		// Position auf dem LCD Display für den Signal Anzeiger (0 = ganz links)
#define LCD_POS_ZEIT_AKTU   2			// Position auf dem LCD Display für die Anzeige des aktuellen Zeitwerts	
#define LCD_POS_ZEIT_BEST   10		// Position auf dem LCD Display für den Signal des besten Zeitwerts

#define GRENZWERT_FAKTOR    0.95  // Die Lichtschranke wird als unterbrochen angenommen, wenn der Messwert unter diesen Faktor fällt. 

//-------------------------------------

#define AN                    HIGH    // Wert um einen digitalen Ausgang AN zu schalten
#define AUS                   LOW     // Wert um einen digitalen Ausgang AUS zu schalten
#define DIG_EINGANG           INPUT   // Wert um einen digitalen Pin als Eingang zu benutzen
#define DIG_AUSGANG           OUTPUT  // Wert um einen digitalen Pin als Ausgang zu benutzen
#define ABBRUCH               return  // Mit diesem Befehl wird eine Funktion abgebrochen
#define WENN                  if      // Mit diesem Wort kann man eine Überprüfung machen
#define IST                   ==      // Mit diesem Wort kann man eine Überprüfung machen
#define UNGLEICH              !=      // Mit diesem Wort kann man eine Überprüfung machen
#define SONST                 else    // Mit diesem Wort kann man eine Überprüfung machen (alternativen)
#define ODER                  ||      // Mit diesem Wort kann man eine Überprüfung machen
#define UND                   &&      // Mit diesem Wort kann man eine Überprüfung machen
#define SCHLEIFE              for     // Mit diesem Wort kann man eine Schleife (sich wiederholdende Befehle) starten
#define WARTE                 delay   // Mit diesem Befehl kann man für x Millisekunden warten bis es weiter geht
#define KOMMAZAHL             double  // Dieses Wort legt fest, dass eine Variable eine Zahl mit Komma (Gleitzahl) sein soll
#define ZAHL                  int     // Dieses Wort legt fest, dass eine Variable eine Zahl sein soll
#define ZUSTAND               bool    // Dieses Wort legt fest, dass eine Variable ein Zustand (Ja/Nein) sein soll
#define FUNKTION              void    // Dieses Wort legt den Beginn einer Funktion fest
#define JA                    true    // Dieses Wort wird genutzt um den Zustand von Tasten und Schaltern zu prüfen
#define NEIN                  false   // Dieses Wort wird genutzt um den Zustand von Tasten und Schaltern zu prüfen
#define MODUS_SCHALTER_LINKS  true    // Dieses Wort drückt aus, dass der Spiel-Modus-Schalter links steht (Spiel 1)
#define MODUS_SCHALTER_RECHTS false   // Dieses Wort drückt aus, dass der Spiel-Modus-Schalter rechts steht (Spiel 2)         
#define ZUFALLSZAHL           random  // Dieses Wort wird genutzt um eine Zufallszahl zu erzeugen
#define LESE_ANALOGEN_EINGANG analogRead // Mit diesem Befehl kann man einen Analogen Eingang auslesen

/* VARIABLEN ---------------------------------------------------------------------------------
   Hier legen wir alle Variable an die wir für den Programmablauf brauchen.
   Wir machen das der Übersicht halber zentral an dieser Stelle.
---------------------------------------------------------------------------------------------*/ 
// Eine Kontroll- Klasse für das LCD Display)
LiquidCrystal_I2C lcdDisplay(0x27, LCD_DISPLAY_ZEICHEN, LCD_DISPLAY_ZEILEN);  // setze LCD auf 16 Zeichen und 2 Zeilen

// Werte für den Lichtsensor 
int    wertLichtsensor 			= 0; 		// Messwert für den Lichtsensor
int    wertLaserLicht 			= 0; 		// Hier speichern wir den Normalwert für das Laserlicht (hohe Helligkeit)
int    wertLichtGrenze      = 0;
bool   schrankeAktiv     		= false; 	// Lichtschranke blockiert, aus
bool   letzterAktivStand 		= false; 	// Hiermit merken wir uns den Zustand der Lichtschranke der zuvor anlag

// Werte für die Zeitmessung und Berechnung der besten Messung
unsigned long aktuelleZeit 		= 0;
unsigned long letzteZeit 		= 0;
unsigned long zeitDauerAkt_ms 	= 0;
unsigned long zeitDauerBest_ms 	= 99999999;

// Werte um das Display zu versorgen
String datenDisplay = "";

/* SETUP() ---------------------------------------------------------------------------------
   Dies ist die Funktion die aufgerufen wird, wenn das Programm startet.
   Es started das LCD Display und misst den "normalen" Helligkeitswert des Lichtsensors.
   Achtung, der Laser muss auf den Sensor gerichtet sein, sonst klappt die Kalibrierung nicht.
---------------------------------------------------------------------------------------------*/ 
void setup() {

  // warten bis sich Dinge normalisiert haben. Das muss sein um den analogen Eingang auf normale Spannungswerte zu bringen
  WARTE(1000); 
  
  // Analogen Eingang auslesen, speichern wir als "normalen Laserwert". 
  // Der Grenzwert liegt bei 90% des gemessenen Werts
  wertLaserLicht = LESE_ANALOGEN_EINGANG(EINGANG_LICHTSENSOR);
  wertLichtGrenze = wertLaserLicht * GRENZWERT_FAKTOR;

  // Hier richten wir das LCD Display ein. 
  // Es zeigt kurz den "normalen" Lichtwert an, den wir gemessen haben.
  lcdDisplay.init();                	// initalisiere LCD 
  lcdDisplay.backlight();           	// Hintergrundbeleuchtung an
  lcdDisplay.clear();					// Speicher leeren
  lcdDisplay.setCursor(0, 0);      		// Schreib-Cursor auf Position 1, Zeile 1 (0, 0)
  lcdDisplay.print("Normalwert:"); 		// Text schreiben
  lcdDisplay.setCursor(0, 1);      		// Cursor auf Position 1, Zeile 1 (0, 0)
  lcdDisplay.print(wertLaserLicht); 	// Text schreiben

  // Den Wert für 1 Sekunde anzeigen
  WARTE(1000);
  
  // Jetzt bereiten wir das Display mit den Anzeigen für Signal, Aktuellen und besten Wert vor
  lcdDisplay.clear();							
  lcdDisplay.setCursor(LCD_POS_SIGNAL, 0);      
  lcdDisplay.print("S"); 						
  lcdDisplay.setCursor(LCD_POS_ZEIT_AKTU, 0);   
  lcdDisplay.print("LETZTE"); 
  lcdDisplay.setCursor(LCD_POS_ZEIT_BEST, 0);    
  lcdDisplay.print("BESTE"); 
}

/* LOOP() ---------------------------------------------------------------------------------
   Dies ist die Funktion die aufgerufen wird, wenn das Programm arbeitet.
   Der Arduino wird diese Funktion ohne Pause immer wieder aufrufen, so schnell er kann.
   Hier wird gemessen ob der Laser weiterhin den Sensor trifft, wenn nicht, gehen wir von
   einem Unterbruch des Laserstrahls und damit Deaktivierung der Lichtschranke aus. Das
   löst dann die Berechnung der Zeit seit der letzten Deaktivierung aus.
---------------------------------------------------------------------------------------------*/ 
void loop() {
	
  // Analogen Eingang auslesen
  wertLichtsensor = analogRead(EINGANG_LICHTSENSOR);

  // Aktuelle Zeit auslesen
  aktuelleZeit = millis();

  // Wenn der aktuelle Wert mehr als 10 % kleiner ist als der eingemessene Laserwert, ist die Lichtschranke unterbrochen
  // Das heisst, der Laser scheint nicht mehr auf den Sensor.
  WENN (wertLichtsensor > wertLichtGrenze) {
     
	 // OK, Laser auf dem Sensor, Lichtschranke ist aktiv
	 schrankeAktiv = JA;
	 // Auf dem Display zeigen wir unter S(ignal) eine 1 an.
   lcdDisplay.setCursor(LCD_POS_SIGNAL, 1);      	// Cursor auf Position 1, Zeile 1 (0, 0)
   lcdDisplay.print(1); 							// senden Nachricht an LCD
	 
  } 
  SONST {
	  
	  // OK, Laser scheint nicht mehr auf den Sensor, Schranke ist deaktiviert!
    schrankeAktiv = NEIN;
	  // Auf dem Display zeigen wir unter S(ignal) eine 0 an.
    lcdDisplay.setCursor(LCD_POS_SIGNAL, 1);      	// Cursor auf Position 1, Zeile 1 (0, 0)
    lcdDisplay.print(0); 							// senden Nachricht an LCD
  }

  // Prüfen ob wir eine positive Flanke haben (Bei der letzten Messung war die Schrank aktiv, und jetzt nicht mehr)
  WENN (letzterAktivStand IST JA && schrankeAktiv IST NEIN) {

	// JETZT MÜSSEN WIR WAS TUN

    // Aktuellen Wert berechnen: Dauer in millisekunden = Uhrzeit jetzt minus Uhrzeit der letzten Deaktivierung
    zeitDauerAkt_ms = aktuelleZeit - letzteZeit;
	
    // Zeitstempel speichern
    letzteZeit = aktuelleZeit;
    
	// Besten Wert updaten?
    WENN (zeitDauerAkt_ms < zeitDauerBest_ms) {
		
		// Die aktuelle Zeit ist besser (kleiner), also merken wir uns diese.
		zeitDauerBest_ms = zeitDauerAkt_ms; 
    }

    // Display updaten
	
	// Zuerst berechnen wir die Zeiten in Sekunden
    KOMMAZAHL fZeitDauerAkt_ms = (double)zeitDauerAkt_ms / 1000;
    KOMMAZAHL fZeitDauerBest_ms = (double)zeitDauerBest_ms / 1000;
	
	// Jetzt bauen wir den Text zusammen, den wir anzeigen wollen. Die Zeiten in Sekunden, mit Komma und 3 Nachkommastellen
    datenDisplay = String( String(fZeitDauerAkt_ms, 3) + "  " + String(fZeitDauerBest_ms, 3) + "  ");
    
	// Jetzt schicken wir den Text aufs Display.
    lcdDisplay.setCursor(LCD_POS_ZEIT_AKTU, 1);      // Cursor auf Position 1, Zeile 1 (0, 0)
    lcdDisplay.print(datenDisplay);
    
  }

  // Zu guter letzt merken wir uns den letzten Stand der Lichtschranke
  letzterAktivStand = schrankeAktiv;   
}
