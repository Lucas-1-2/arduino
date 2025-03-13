/*
  Lucas Prati && Nicola Laghi 4^A EN 23-03-2022
  Stazione Pneumatica Versione 5
  Versione 5: differenza dalla versione 3, il controllo temperatura e il display lcd con il conteggio pezzi.è MONOPEZZO
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

/*Dichiarazione Ingressi*/
#define FUsc 29           // Fotocellula fine lavorazione [I1]
#define Allineamento 35   // Allineamento piattaforma rotante [I4]
#define FIng 37           // Presenza pezzo in ingresso [I5]
#define PStop 39          // Pulsante start/stop [I6]
#define Sens_Temp A0      // Sensore di temperatura LM35

/*Dichiarazione Uscite*/
#define Nastro 6            // Motore nastro trasportatore [Q1]
#define Rotazione 2         // Motore per ruotare [Q2]
#define Spin2_Av 36         // Spintore avanti per inserire il pezzo nel nastro [Q3]
#define Spin2_Ind 40        // Spintore indietro [Q4]
#define Etichettatrice 32   // Etichettatrice [Q5]
#define Spin1 4             // Spintore di ingresso nella stazione rotante [Q6]
#define Compressore 44      // Compressore [Q7]

const int T1 = 500;                         //tempo T1 espresso in millisecondi
const int T2 = 1000;                         //tempo T2 espresso in millisecondi
const int T3 = 1000;                        //tempo T3 espresso in millisecondi
const int T4 = 500;                         //tempo T4 espresso in millisecondi

unsigned long int tempo;                    //variabile per tenere traccia del tempo trascorso (usata nel pallogramma)
unsigned long int tempo_acquisizione;       //variabile per tenere traccia del tempo trascorso (usata nell'acquisizione della temperatura e aggiornamento del display)

int hh = 0;                 //tempo in ore
int mm = 0;                 //tempo in minuti
int ss = 0;                 //tempo in secondi
int pezzi_lavorati = 0;     //numero di pezzi lavorati stampati a schermo
float Temperatura = 0.0;    //variabile per la temperatura
char stato;                 //stato attuale del programma in base al pallogramma




void setup() {
  Serial.begin(9600);
  lcd.init();         //Inizializzo il display
  lcd.backlight();

  /*---------- SET PIN INPUT ----------*/
  pinMode(FUsc, INPUT);
  pinMode(Allineamento, INPUT);
  pinMode(FIng, INPUT);
  pinMode(PStop, INPUT);
  pinMode(Sens_Temp, INPUT);

  /*---------- SET PIN OUTPUT ----------*/
  pinMode(Nastro, OUTPUT);
  pinMode(Rotazione, OUTPUT);
  pinMode(Spin2_Av, OUTPUT);
  pinMode(Spin2_Ind, OUTPUT);
  pinMode(Etichettatrice, OUTPUT);
  pinMode(Spin1, OUTPUT);
  pinMode(Compressore, OUTPUT);

  stato = 'A';
  digitalWrite(Compressore, LOW);  // Compressore acceso fin dall'inizio
  Stato_Vuoto();
  lcd.setCursor(0, 0);
  lcd.print("Temp:     ");
  lcd.setCursor(0, 1);
  lcd.print("Pezzi:00 0:00:00");

  /*--- Inizializzazione della piattaforma e del secondo spintore ---*/
  digitalWrite(Rotazione, LOW);
  while (digitalRead(Allineamento) == HIGH);
  digitalWrite(Rotazione, HIGH);
  digitalWrite(Spin2_Ind, LOW);
  delay(500);
  digitalWrite(Spin2_Ind, HIGH);
}

void loop() {

  if ((Temperatura < 40) && (digitalRead(PStop) == HIGH)) {
    digitalWrite(Compressore, LOW);    //Accendo il compressore
    /*-------------Stati del Pallogramma-------------*/
    if (stato == 'A') {
      Stato_Vuoto();
      if ((digitalRead(FUsc) == LOW) && (digitalRead(FIng) == HIGH)) {
        stato = 'B';
        tempo = millis();
      }

    } else if (stato == 'B') {
      Stato_Vuoto();
      if ((millis() - tempo) >= T1) {
        stato = 'C';
        tempo = millis();
      }

    } else if (stato == 'C') {
      Set_Uscite(1, 1, 1, 1, 1, 0);   //attivo Q6
      if ((millis() - tempo) >= T2) {
        stato = 'D';
      }

    } else if (stato == 'D') {
      Set_Uscite(1, 0, 1, 1, 1, 1);   //attivo Q2
      if (digitalRead(Allineamento) == HIGH) {
        stato = 'E';
      }

    } else if (stato == 'E') {
      Set_Uscite(1, 0, 1, 1, 1, 1);   //attivo Q2
      if (digitalRead(Allineamento) == LOW) {
        stato = 'F';
        tempo = millis();
      }

    } else if (stato == 'F') {
      Set_Uscite(1, 1, 1, 1, 0, 1);   //attivo Q5
      if ((millis() - tempo) >= T3) {
        stato = 'G';
        tempo = millis();
      }

    } else if (stato == 'G') {
      Stato_Vuoto();
      if ((millis() - tempo) >= T4) {
        stato = 'H';
      }

    } else if (stato == 'H') {
      Set_Uscite(1, 0, 1, 1, 1, 1);   //attivo Q2
      if (digitalRead(Allineamento) == HIGH) {
        stato = 'I';
      }

    } else if (stato == 'I') {
      Set_Uscite(1, 0, 1, 1, 1, 1);   //attivo Q2
      if (digitalRead(Allineamento) == LOW) {
        stato = 'L';
        tempo = millis();
      }

    } else if (stato == 'L') {
      Set_Uscite(1, 1, 0, 1, 1, 1);   //attivo Q3
      if ((millis() - tempo) >= T2) {
        stato = 'M';
        tempo = millis();
      }

    } else if (stato == 'M') {
      Set_Uscite(0, 1, 1, 0, 1, 1);   //attivo Q1 e Q4
      if ((millis() - tempo) >= T2) {
        stato = 'N';
      }

    } else if (stato == 'N') {
      Set_Uscite(0, 1, 1, 1, 1, 1);   //attivo Q1
      if (digitalRead(FUsc) == HIGH) {
        stato = 'O';
      }

    } else if (stato == 'O') {
      Stato_Vuoto();
      if (digitalRead(FUsc) == LOW) {
        pezzi_lavorati++;
        /*--- set del cursore in base al numero di pezzi lavorati ---*/
        if (pezzi_lavorati < 9) {
          lcd.setCursor(7, 1);
        } else {
          lcd.setCursor(6, 1);
        }
        lcd.print(pezzi_lavorati);
        stato = 'A';
      }
    }


    /*-------------Interruzione per Emergenza o Temperatura Elevata-------------*/
  } else {
    Stato_Vuoto();
    digitalWrite(Compressore, HIGH);                      //Spengo il compressore
    if (digitalRead(PStop) == LOW) {
      Serial.println("Pulsante di Emergenza attivato, sistema in standby");
      tempo = millis() - tempo;                             //Memorizzo la differenza di tempo trascorso così da ripartire dallo stesso istante una volta riavviato il sistema
      tempo_acquisizione = millis() - tempo_acquisizione;   //Memorizzo la differenza di tempo trascorso così da ripartire dallo stesso istante una volta riavviato il sistema
      while (digitalRead(PStop) == LOW);
      tempo = millis() - tempo;                             //Imposto il timer sulla stessa differenza di tempo trascorso così da ripartire dallo stesso istante in cui ho interrotto il programma
      tempo_acquisizione = millis() - tempo_acquisizione;   //Imposto il timer sulla stessa differenza di tempo trascorso così da ripartire dallo stesso istante in cui ho interrotto il programma
      Serial.println("Pulsante di Emergenza disattivato, sistema in esecuzione");
    }
  }
  if ((millis() - tempo_acquisizione) >= 1000) {
    tempo_acquisizione = millis();
    Temperatura = analogRead(Sens_Temp) * 0.488;
    lcd.setCursor(5, 0);
    lcd.print("   ");
    lcd.setCursor(5, 0);
    lcd.print(Temperatura);
    ss++;
    /*--- Aggiornamento del tempo ---*/
    if (ss == 60) {
      ss = 0;
      lcd.setCursor(14, 1);
      lcd.print("00");
      mm++;
    }
    if (mm == 60) {
      mm = 0;
      lcd.setCursor(11, 1);
      lcd.print("00");
      hh++;
    }
    if (hh == 10) {
      hh = 0;
    }
    /*--- set del cursore in base al valore dei tempi ---*/
    if (ss < 10) {           //gestione del set cursore per i secondi
      lcd.setCursor(15, 1);
    } else {
      lcd.setCursor(14, 1);
    }
    lcd.print(ss);

    if (mm < 10) {           //gestione del set cursore per i minuti
      lcd.setCursor(12, 1);
    } else {
      lcd.setCursor(11, 1);
    }
    lcd.print(mm);

    lcd.setCursor(9, 1);   //gestione del set cursore per le ore
    lcd.print(hh);
  }
}
