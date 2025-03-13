// PROGRAMMA FUNZIONANTE


/* Funzionamento:
   tramite i Joystick è possibile comandare gli assi denominati come Rotazione, Spalla, Polso_Rotazione e Morsa;
   alla pressione dei pulsanti situato nella shield posso comandare gli assi denominati come Gomito e Polso_Verticale;
   è inoltre possibile monitorare tramite porta seriale le modifiche di posizione di tutti gli assi.
*/

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
#define Joy_Polso_Rotazione A10  // Pin Polso_Rotazione joystick-2
#define Joy_Morsa A11            // Pin Morsa joystick-2

#define Joy_Gomito_Up 52    // Pin Gomito_UP joystick-1
#define Joy_Gomito_Down 50  // Pin Gomito_Down joystick-1
#define Joy_Polso_Up 53     // Pin Polso_UP joystick-2
#define Joy_Polso_Down 51   // Pin Polso_Down joystick-2

#define Zona_morta 100  // Set "Zona Morta Joystick"


// Pin Servomotori
const int Pin_Rotazione = 7 ;       // Pin Servomotore Asse Rotazione
const int Pin_Spalla = 6 ;          // Pin Servomotore Asse Spalla
const int Pin_Gomito = 5;           // Pin Servomotore Asse Gomito
const int Pin_Polso_Verticale = 4;  // Pin Servomotore Asse Polso_Verticale
const int Pin_Polso_Rotazione = 3;  // Pin Servomotore Asse Polso_Rotazione
const int Pin_Morsa = 2;            // Pin Servomotore Asse Morsa

// Pin Posizione
const int Pin_Home_ON = 48;
const int Pin_Home_OFF = 46;



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

float Old_Posizioni[6] = {0, 0, 0, 0, 0, 0};
float Vel_digital = 1;

bool Refresh_coordinate = false;
bool ManAutoN = true;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  Serial.println("Avvio Programma...");
  Serial.println("Source-Code: Braccio_Robotico_LucaSam_V4_ManAutoN.ino");


  // Set dell'asse Rotazione e posizionamento nel punto di start
  Rotazione.attach(Pin_Rotazione);
  Rotazione.write(Posizioni[0]);
  Serial.print("\nAsse Rotazione nella posizione di partenza: ");
  Serial.println(Posizioni[0]);

  // Set dell'asse Spalla e posizionamento nel punto di start
  Spalla.attach(Pin_Spalla);
  Spalla.write(Posizioni[1]);
  Serial.print("\nAsse Spalla nella posizione di partenza: ");
  Serial.println(Posizioni[1]);

  // Set della Gomito e posizionamento nel punto di start
  Gomito.attach(Pin_Gomito);
  Gomito.write(Posizioni[2]);
  Serial.print("\nAsse Gomito nella posizione di partenza: ");
  Serial.println(Posizioni[2]);

  // Set dell'asse Polso_Verticale e posizionamento nel punto di start
  Polso_Verticale.attach(Pin_Polso_Verticale);
  Polso_Verticale.write(Posizioni[3]);
  Serial.print("\nAsse Polso_Verticale nella posizione di partenza: ");
  Serial.println(Posizioni[3]);

  // Set dell'asse Polso_Rotazione e posizionamento nel punto di start
  Polso_Rotazione.attach(Pin_Polso_Rotazione);
  Polso_Rotazione.write(Posizioni[4]);
  Serial.print("\nAsse Polso_Rotazione nella posizione di partenza: ");
  Serial.println(Posizioni[4]);

  // Set della Morsa e posizionamento nel punto di start
  Morsa.attach(Pin_Morsa);
  Morsa.write(Posizioni[5]);
  Serial.print("\nAsse Morsa nella posizione di partenza: ");
  Serial.println(Posizioni[5]);


  // Set dei pulsanti della shield
  pinMode(Joy_Gomito_Down, INPUT_PULLUP);
  pinMode(Joy_Polso_Down, INPUT_PULLUP);
  pinMode(Joy_Gomito_Up, INPUT_PULLUP);
  pinMode(Joy_Polso_Up, INPUT_PULLUP);

  pinMode(Pin_Home_ON, INPUT_PULLUP);
  pinMode(Pin_Home_OFF, INPUT_PULLUP);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  if (ManAutoN) {
    // Movimento Assi controllati con joystick
    moveServo(0, analogRead(Joy_Rotazione), true);          // Muove l'asse Rotazione
    moveServo(1, analogRead(Joy_Spalla), false);            // Muove l'asse Spalla
    moveServo(4, analogRead(Joy_Polso_Rotazione), true);    // Muove l'asse Polso_Rotazione
    moveServo(5, analogRead(Joy_Morsa), false);             // Muove l'asse Morsa

    // Movimento Assi controllati con pulsanti della shield (NB gli assi opposti DEVONO AVERE LA VARIABILE REVERSE OPPOSTA TRA LORO)
    moveServo_digital(2, digitalRead(Joy_Gomito_Up), digitalRead(Joy_Gomito_Down), false);   // Muove l'asse Gomito_Up
    moveServo_digital(2, digitalRead(Joy_Gomito_Down), digitalRead(Joy_Gomito_Up), true);   // Muove l'asse Gomito_Down
    moveServo_digital(3, digitalRead(Joy_Polso_Up), digitalRead(Joy_Polso_Down), false);     // Muove l'asse Polso_Up
    moveServo_digital(3, digitalRead(Joy_Polso_Down), digitalRead(Joy_Polso_Up), true);     // Muove l'asse Polso_Down

  } else {
    Rotazione.write(Posizioni_Home[0]);
    Spalla.write(Posizioni_Home[1]);
    Gomito.write(Posizioni_Home[2]);
    Polso_Verticale.write(Posizioni_Home[3]);
    Polso_Rotazione.write(Posizioni_Home[4]);
    Morsa.write(Posizioni_Home[5]);
  }

  // Controllo pulsanti per posizionamento automatico in HOME
  if ((digitalRead(Pin_Home_ON) == LOW) && (digitalRead(Pin_Home_OFF) == HIGH)) ManAutoN = false;
  if ((digitalRead(Pin_Home_OFF) == LOW) && (digitalRead(Pin_Home_ON) == HIGH)) ManAutoN = true;



  // Controllo se uno dei motori ha cambiato coordinate
  for (int i = 0; i < 6; i++) {
    if (Old_Posizioni[i] != Posizioni[i]) {
      Refresh_coordinate = true;
      Old_Posizioni[i] = Posizioni[i];
    }
  }

  // In caso, eseguo un refresh sul monitor seriale delle posizioni dei motori
  if (Refresh_coordinate) {
    for (int i = 0; i < 6; i++) {
      Serial.print("\t");
      Serial.print(Posizioni[i]);
    }
    Serial.print("\n\n");
    Refresh_coordinate = false;
  }

  delay(20);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* La funzione utilizza i parametri nel seguente ordine:
      - Numero della posizione del vettore Posizioni per l'associazione alla variabile che gestisce il motore
      - Lettura del valore associato all'asse desiderato
      - variabile booleana per attivare o meno l'inversione assi
*/
void moveServo(int indice, int VAL, bool reverse) {
  if (abs(VAL - 512) > Zona_morta) {
    float delta = (float)(VAL - 512) / 100;
    if (reverse == true) {
      Posizioni[indice] -= delta;
    } else {
      Posizioni[indice] += delta;
    }
    Posizioni[indice] = constrain(Posizioni[indice], 0, 179);
    int OUT_Servo = map(Posizioni[indice], 0, 179, SERVO_MIN[indice], SERVO_MAX[indice]);

    switch (indice) {
      case 0:
        Rotazione.write(OUT_Servo);
        break;

      case 1:
        Spalla.write(OUT_Servo);
        break;

      case 4:
        Polso_Rotazione.write(OUT_Servo);
        break;

      case 5:
        Morsa.write(OUT_Servo);
        break;

      default:
        Serial.println("Errore nell'assegnazione del valore OUT_Servo all'indice (moveServo)");
        break;
    }
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* La funzione utilizza i parametri nel seguente ordine:
      - Numero della posizione del vettore Posizioni per l'associazione alla variabile che gestisce il motore
      - Lettura del valore associato all'asse desiderato
      - Lettura del valore associato all'asse indesiderato
      - variabile booleana per attivare o meno l'inversione assi
*/

void moveServo_digital (int indice, int Pin_ON, int Pin_OFF, bool reverse) {
  if ((Pin_ON == LOW) && (Pin_OFF == HIGH)) {
    if (reverse) {
      Posizioni[indice] -= Vel_digital;
    } else {
      Posizioni[indice] += Vel_digital;
    }
    Posizioni[indice] = constrain(Posizioni[indice], 0, 179);
    int OUT_Servo = map(Posizioni[indice], 0, 179, SERVO_MIN[indice], SERVO_MAX[indice]);

    switch (indice) {
      case 2:
        Gomito.write(OUT_Servo);
        break;

      case 3:
        Polso_Verticale.write(OUT_Servo);
        break;

      default:
        Serial.println("Errore nell'assegnazione del valore OUT_Servo all'indice (moveServo_digital)");
        break;
    }
  }
}
