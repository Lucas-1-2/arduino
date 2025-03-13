// PROGRAMMA IN LAVORAZIONE       LCD da controllare 'void Cursore()'    Serial OK   Autom OK      Man OK      pick_Place OK      Bilancia da fare


/* Funzionamento:
   tramite i Joystick è possibile comandare gli assi denominati come Rotazione, Spalla, Polso_Rotazione e Morsa;
   alla pressione dei pulsanti situato nella shield posso comandare gli assi denominati come Gomito e Polso_Verticale;
   è inoltre possibile monitorare tramite porta seriale le modifiche di posizione di tutti gli assi.
*/

#include <LiquidCrystal_I2C.h>
#include <Servo.h>


// Nomi servomotori
Servo Rotazione;
Servo Spalla;
Servo Gomito;
Servo Polso_Verticale;
Servo Polso_Rotazione;
Servo Morsa;


#define Joy_Rotazione A8         // Pin Rotazione joystick-1
#define Joy_Spalla A9            // Pin Spalla joystick-1
#define Joy_Polso_Rotazione 10  // Pin Polso_Rotazione joystick-2
#define Joy_Morsa 11            // Pin Morsa joystick-2

#define Joy_Gomito_Up 52    // Pin Gomito_UP joystick-1
#define Joy_Gomito_Down 50  // Pin Gomito_Down joystick-1
#define Joy_Polso_Up 53     // Pin Polso_UP joystick-2
#define Joy_Polso_Down 51   // Pin Polso_Down joystick-2

#define Zona_morta 100  // Set "Zona Morta Joystick"


// Pin Servomotori
const int Pin_Rotazione = 7;        // Pin Servomotore Asse Rotazione
const int Pin_Spalla = 6;           // Pin Servomotore Asse Spalla
const int Pin_Gomito = 5;           // Pin Servomotore Asse Gomito
const int Pin_Polso_Verticale = 4;  // Pin Servomotore Asse Polso_Verticale
const int Pin_Polso_Rotazione = 3;  // Pin Servomotore Asse Polso_Rotazione
const int Pin_Morsa = 2;            // Pin Servomotore Asse Morsa

// Pin Posizione
const int Pin_Man = 48;
const int Pin_Auto = 46;
const int Pin_Pick_Place = 44;



// Set posizioni  minime assi, in ordine: Rotazione, Spalla, Gomito, Polso_Verticale, Polso_Rotazione, Morsa
const int SERVO_MIN[6] = { 0, 15, 0, 0, 0, 35 };

// Set posizioni  massime assi, in ordine: Rotazione, Spalla, Gomito, Polso_Verticale, Polso_Rotazione, Morsa
const int SERVO_MAX[6] = { 180, 140, 180, 180, 180, 90 };

// Creazione del vettore che conterrà le posizioni dei servomotori
// !!!Attenzione!!! I valori contengono le posizioni iniziali dei 6 servomotori identiche alla variabile 'Posizioni_Home'
float Posizioni[6] = { 90, 90, 90, 90, 90, 35 };  // Rotazione, Spalla, Gomito, Polso_Verticale, Polso_Rotazione, Morsa

// Creazione del vettore che conterrà le posizioni home dei servomotori
// !!!Attenzione!!! I valori contengono le posizioni iniziali dei 6 servomotori identiche alla variabile 'Posizioni'
float Posizioni_Home[6] = { 90, 90, 90, 90, 90, 35 };  // Rotazione, Spalla, Gomito, Polso_Verticale, Polso_Rotazione, Morsa

float Vel_digital = 1;


bool ManAutoN = 1;    // se uguale a 1, è attivo il movimento manuale
bool Automatico = 0;  // se uguale a 1 la modalità home è attiva, con possibilità di attivare modalità pick and place
bool Pick_Place = 0;  // se uguale a 1 la modalità pick and place è attiva

/*--- LCD ---*/
LiquidCrystal_I2C lcd(0x27, 20, 4);
int cont = 0;                                 // variabile intera per il conteggio degli impulsi che mi interessano nell'interrupt prima di aggiornare l'LCD
#define ms_refresh_LCD 200                    // dichiaro i millisecondi che voglio attendere prima che l'interrupt mi aggiorni i dati sul monitor LCD
#define impulsi_interrupt ms_refresh_LCD / 2  // converto i millisecondi che voglio attendere prima che l'interrupt mi aggiorni i dati sul monitor LCD, in impulsi

/*--- Porta Seriale ---*/
unsigned long int tstamp;
const int refresh_display = 1000;  //tempo di refresh della porta seriale (in millisecondi)



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  Serial.println("Avvio Programma...");
  Serial.println("Source-Code: Braccio_Robotico_LucaSam_V6_Auto.ino");

  lcd.init();
  lcd.backlight();
  attachInterrupt(0, User_Interface, RISING);


  // Set dell'asse Rotazione e posizionamento nel punto di start
  Rotazione.attach(Pin_Rotazione);
  Rotazione.write(Posizioni[0]);
  Serial.print("\nAsse Rotazione nella posizione di partenza: ");
  Serial.println(Posizioni[0]);
  lcd.setCursor(0, 0);
  lcd.print("BASE: ");
  Cursore(0, 6, 0);  //Scrivo la posizione della base nell'LCD (rotazione, motore 0)

  // Set dell'asse Spalla e posizionamento nel punto di start
  Spalla.attach(Pin_Spalla);
  Spalla.write(Posizioni[1]);
  Serial.print("\nAsse Spalla nella posizione di partenza: ");
  Serial.println(Posizioni[1]);
  lcd.setCursor(0, 1);
  lcd.print("SPAL: ");
  Cursore(1, 6, 1);  //Scrivo la posizione della spalla nell'LCD (motore 1)


  // Set della Gomito e posizionamento nel punto di start
  Gomito.attach(Pin_Gomito);
  Gomito.write(Posizioni[2]);
  Serial.print("\nAsse Gomito nella posizione di partenza: ");
  Serial.println(Posizioni[2]);
  lcd.setCursor(0, 2);
  lcd.print("GOMI: ");
  Cursore(2, 6, 2);  //Scrivo la posizione del gomito nell'LCD (motore 2)


  // Set dell'asse Polso_Verticale e posizionamento nel punto di start
  Polso_Verticale.attach(Pin_Polso_Verticale);
  Polso_Verticale.write(Posizioni[3]);
  Serial.print("\nAsse Polso_Verticale nella posizione di partenza: ");
  Serial.println(Posizioni[3]);
  lcd.setCursor(11, 0);
  lcd.print("POLV: ");
  Cursore(3, 17, 0);  //Scrivo la posizione del polso verticale nell'LCD (motore 3)


  // Set dell'asse Polso_Rotazione e posizionamento nel punto di start
  Polso_Rotazione.attach(Pin_Polso_Rotazione);
  Polso_Rotazione.write(Posizioni[4]);
  Serial.print("\nAsse Polso_Rotazione nella posizione di partenza: ");
  Serial.println(Posizioni[4]);
  lcd.setCursor(11, 1);
  lcd.print("POLR: ");
  Cursore(4, 17, 1);  //Scrivo la posizione del polso rotazione nell'LCD (motore 4)


  // Set della Morsa e posizionamento nel punto di start
  Morsa.attach(Pin_Morsa);
  Morsa.write(Posizioni[5]);
  Serial.print("\nAsse Morsa nella posizione di partenza: ");
  Serial.println(Posizioni[5]);
  lcd.setCursor(11, 2);
  lcd.print("MORS: ");
  Cursore(5, 17, 2);  //Scrivo la posizione della morsa nell'LCD (motore 5 )


  // Set dei pulsanti della shield
  pinMode(Joy_Gomito_Down, INPUT_PULLUP);
  pinMode(Joy_Polso_Down, INPUT_PULLUP);
  pinMode(Joy_Gomito_Up, INPUT_PULLUP);
  pinMode(Joy_Polso_Up, INPUT_PULLUP);

  pinMode(Pin_Man, INPUT_PULLUP);
  pinMode(Pin_Auto, INPUT_PULLUP);
  pinMode(Pin_Pick_Place, INPUT_PULLUP);
  tstamp = millis();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  if ((ManAutoN) && (!Automatico) && (!Pick_Place)) {
    // Movimento Assi controllati con joystick
    moveServo(0, analogRead(Joy_Rotazione), true);        // Muove l'asse Rotazione
    moveServo(1, analogRead(Joy_Spalla), false);          // Muove l'asse Spalla
    moveServo(4, analogRead(Joy_Polso_Rotazione), true);  // Muove l'asse Polso_Rotazione
    moveServo(5, analogRead(Joy_Morsa), false);           // Muove l'asse Morsa

    // Movimento Assi controllati con pulsanti della shield (NB gli assi opposti DEVONO AVERE LA VARIABILE REVERSE OPPOSTA TRA LORO)
    moveServo_digital(2, digitalRead(Joy_Gomito_Up), digitalRead(Joy_Gomito_Down), false);  // Muove l'asse Gomito_Up
    moveServo_digital(2, digitalRead(Joy_Gomito_Down), digitalRead(Joy_Gomito_Up), true);   // Muove l'asse Gomito_Down
    moveServo_digital(3, digitalRead(Joy_Polso_Up), digitalRead(Joy_Polso_Down), false);    // Muove l'asse Polso_Up
    moveServo_digital(3, digitalRead(Joy_Polso_Down), digitalRead(Joy_Polso_Up), true);     // Muove l'asse Polso_Down

    /*--- Gestisco la porta seriale USB ---*/
    if ((millis() - tstamp) >= refresh_display) {
      tstamp = millis();
      for (int i = 0; i < 6; i++) {
        Serial.print("\t");
        Serial.print(Posizioni[i]);
      }
      Serial.print("\n\n");
    }


  } else if ((Automatico) && (!ManAutoN) && (!Pick_Place)) {
    moveBracciov(Posizioni_Home[0], Posizioni_Home[1], Posizioni_Home[2], Posizioni_Home[3], Posizioni_Home[4], Posizioni_Home[5], 20);
    /*
    Rotazione.write(Posizioni_Home[0]);
    Spalla.write(Posizioni_Home[1]);
    Gomito.write(Posizioni_Home[2]);
    Polso_Verticale.write(Posizioni_Home[3]);
    Polso_Rotazione.write(Posizioni_Home[4]);
    Morsa.write(Posizioni_Home[5]);
    */
    for (int i = 0; i < 6; i++) Posizioni[i] = Posizioni_Home[i];  //aggiorno i valori delle posizioni


  } else if ((Pick_Place) && (!ManAutoN) && (!Automatico)) {
    moveBracciov(172, 97, 22, 14, 91, 35, 20);
    moveBracciov(172, 97, 22, 14, 91, 100, 20);
    moveBracciov(160, 90, 60, 50, 91, 100, 20);
    moveBracciov(20, 90, 60, 50, 91, 100, 10);
    moveBracciov(7, 93, 19, 14, 91, 100, 20);
    moveBracciov(7, 93, 19, 14, 91, 35, 20);
    moveBracciov(Posizioni_Home[0], Posizioni_Home[1], Posizioni_Home[2], Posizioni_Home[3], Posizioni_Home[4], Posizioni_Home[5], 20);
  }

  // Controllo pulsanti per posizionamento automatico
  if (digitalRead(Pin_Man) == LOW) {
    ManAutoN = true;
    Automatico = false;
    Pick_Place = false;
    while (digitalRead(Pin_Man) == LOW);
  }
  if ((ManAutoN) && (digitalRead(Pin_Auto)) == LOW) {
    Automatico = true;
    ManAutoN = false;
    Pick_Place = false;
    while (digitalRead(Pin_Auto) == LOW);
  }
  if ((Automatico) && (digitalRead(Pin_Pick_Place) == LOW)) {
    Pick_Place = true;
    ManAutoN = false;
    Automatico = false;
    while (digitalRead(Pin_Pick_Place) == LOW);
  }

  delay(10);
}