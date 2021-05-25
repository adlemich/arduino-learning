//------------------------------------------------------------------------------------------------------------
// INCLUDE - Hier werden Bibliotheken und andere Code-Module eingebunden (importiert)
//------------------------------------------------------------------------------------------------------------
/* Generelle Bibliothek für Arduino Progammierung */
#include <Arduino.h>

//------------------------------------------------------------------------------------------------------------
// DEFINE - Hier werden Definitionen und Konstanten festgelegt
//------------------------------------------------------------------------------------------------------------

#define TASTER_IN_PIN         2       // Dies ist der digitale Eingang für den Taster

#define AMPEL_ROT_PIN         13      // Der digitale Ausgang für die rote Lampe
#define AMPEL_GELB_PIN        12      // Der digitale Ausgang für die gelbe Lampe
#define AMPEL_GRUEN_PIN       11      // Der digitale Ausgang für die gruene Lampe

// SPRACHMODULE
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
#define ZAHL                  int     // Dieses Wort legt fest, dass eine Variable eine Zahl sein soll
#define ZUSTAND               bool    // Dieses Wort legt fest, dass eine Variable ein Zustand (Ja/Nein) sein soll
#define FUNKTION              void    // Dieses Wort legt den Beginn einer Funktion fest
#define JA                    true    // Dieses Wort wird genutzt um den Zustand von Tasten und Schaltern zu prüfen
#define NEIN                  false   // Dieses Wort wird genutzt um den Zustand von Tasten und Schaltern zu prüfen
#define MODUS_SCHALTER_LINKS  true    // Dieses Wort drückt aus, dass der Spiel-Modus-Schalter links steht (Spiel 1)
#define MODUS_SCHALTER_RECHTS false   // Dieses Wort drückt aus, dass der Spiel-Modus-Schalter rechts steht (Spiel 2)         
#define ZUFALLSZAHL           random  // Dieses Wort wird genutzt um eine Zufallszahl zu erzeugen
 
//------------------------------------------------------------------------------------------------------------
// GLOABLE VARIABLEN - Hier legen wir Variablen, also Daten-Elemente an die wir im Programm nutzen
//------------------------------------------------------------------------------------------------------------

// Merker für den Taster
ZUSTAND tasterLetzterZustand = NEIN;

// Merker für den Ampel-Status
ZAHL ampelStatus = 1;

// ------------------------------------------------------------------------------------------------------------
// DIG_PINS_SETUP - Hier legen wir fest welche digital Pins als Eingang oder als Ausgang verwendet werden
// ------------------------------------------------------------------------------------------------------------
FUNKTION dig_pins_setup() {
  pinMode(AMPEL_ROT_PIN,    DIG_AUSGANG); 
  pinMode(AMPEL_GELB_PIN,   DIG_AUSGANG); 
  pinMode(AMPEL_GRUEN_PIN,  DIG_AUSGANG);  
  pinMode(TASTER_IN_PIN,    DIG_EINGANG);  

}

// ------------------------------------------------------------------------------------------------------------
// WURDE_TASTER_GEDRUECKT - Überprüft einen Taster und gibt JA oder NEIN zurück
// ------------------------------------------------------------------------------------------------------------
ZUSTAND wurde_taster_gedrueckt() {

  // Jetzt den Wert am Digital Eingang ablesen
  ZUSTAND taster_jetzt = digitalRead(TASTER_IN_PIN);
  ZUSTAND ergebnis = NEIN;
    
  WENN (taster_jetzt UNGLEICH tasterLetzterZustand UND taster_jetzt IST JA)
  {
    // Der Zustand des Tasters hat sich von NEIN auf JA geändert. Jetzt muss was passieren
    ergebnis = JA;
  }
  tasterLetzterZustand = taster_jetzt;
  
  ABBRUCH ergebnis; 
}

// ############################################################################################################
// ------------------------------------------------------------------------------------------------------------
// SETUP - Diese Funktion wird bei  Start des Arduino aufgerufen, hier werden alle Einrichtungen vorgenommen.
// ------------------------------------------------------------------------------------------------------------
// ############################################################################################################

FUNKTION setup() {

 // 1) Zuerst legen wir für die digitalen Pins fest, ob sie als Ausgang oder Eingang dienen sollen
 dig_pins_setup(); 

 // 2) Jetzt den Start-Zustand der Ampel festlegen
 digitalWrite(AMPEL_ROT_PIN, AN);
 digitalWrite(AMPEL_GELB_PIN, AUS);
 digitalWrite(AMPEL_GRUEN_PIN, AUS);
}

// ############################################################################################################
// ------------------------------------------------------------------------------------------------------------
// LOOP - Diese Funktion wird vom Board immer wieder durchlaufen, so schnell es der Prozessor erlaubt.
// ------------------------------------------------------------------------------------------------------------
// ############################################################################################################
FUNKTION loop() {

  // TASTER PRÜFEN UND AKTION AUSFÜHREN WENN GEDRÜCKT
  WENN (wurde_taster_gedrueckt() IST JA) {
    // Taster-Aktion nun starten 
    ampelStatus = ampelStatus + 1;

    WENN (ampelStatus > 3) {
      ampelStatus = 1;
    }
  }

  // JE NACH AMPELSTATUS DIE LEDS ANMACHEN
  WENN (ampelStatus IST 1) {
    digitalWrite(AMPEL_ROT_PIN, AN);
    digitalWrite(AMPEL_GELB_PIN, AUS);
    digitalWrite(AMPEL_GRUEN_PIN, AUS);
  }
  SONST WENN (ampelStatus IST 2) {
    digitalWrite(AMPEL_ROT_PIN, AUS);
    digitalWrite(AMPEL_GELB_PIN, AN);
    digitalWrite(AMPEL_GRUEN_PIN, AUS);
  }
  SONST WENN (ampelStatus IST 3) {
    digitalWrite(AMPEL_ROT_PIN, AUS);
    digitalWrite(AMPEL_GELB_PIN, AUS);
    digitalWrite(AMPEL_GRUEN_PIN, AN);
  }
  
}
