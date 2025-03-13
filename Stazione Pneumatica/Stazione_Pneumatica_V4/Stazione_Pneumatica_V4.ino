/*
  Lucas Prati && Nicola Laghi 4^A EN 07-03-2022
  Stazione Pneumatica Versione 4
  Versione 4: Prima Versione Multipezzo (CON controllo temperatura, e display LCD)
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

const int T001 = 500;             //tempo T1 espresso in millisecondi
const int T002 = 1000;            //tempo T2 espresso in millisecondi
const int T003 = 1000;            //tempo T3 espresso in millisecondi
const int T004 = 500;             //tempo T4 espresso in millisecondi

unsigned long int tempo1;        //variabile per tenere traccia del tempo trascorso nella prima parte di pallogramma (inserimento pezzo)
unsigned long int tempo2;        //variabile per tenere traccia del tempo trascorso nella parte centrale di pallogramma (etichettatrice)
unsigned long int tempo3;        //variabile per tenere traccia del tempo trascorso nella parte finale di pallogramma (uscita del pezzo sul nastro)
unsigned long int tempo_acquisizione;       //variabile per tenere traccia del tempo trascorso (usata nell'acquisizione della temperatura e aggiornamento del display)

int hh = 0;                 //tempo in ore
int mm = 0;                 //tempo in minuti
int ss = 0;                 //tempo in secondi
int pezzi_lavorati = 0;     //numero di pezzi lavorati stampati a schermo
float Temperatura = 0.0;    //variabile per la temperatura
const float T_MAX = 40.00;

bool stato[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                //stato attuale del programma in base al pallogramma
bool Inserito1 = false;



void setup() {
  Serial.begin(9600);
  lcd.init();         //Inizializzo il display
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Temp:     ");
  lcd.setCursor(0, 1);
  lcd.print("Pezzi:00 0:00:00");

  /*---------- SET PIN INPUT ----------*/
  pinMode(FUsc, INPUT);
  pinMode(Allineamento, INPUT);
  pinMode(FIng, INPUT);
  pinMode(PStop, INPUT);

  /*---------- SET PIN OUTPUT ----------*/
  pinMode(Nastro, OUTPUT);
  pinMode(Rotazione, OUTPUT);
  pinMode(Spin2_Av, OUTPUT);
  pinMode(Spin2_Ind, OUTPUT);
  pinMode(Etichettatrice, OUTPUT);
  pinMode(Spin1, OUTPUT);
  pinMode(Compressore, OUTPUT);


  digitalWrite(Compressore, LOW);  // Compressore acceso fin dall'inizio
  /*--- Reset di tutti i motori ---*/
  digitalWrite(Nastro, HIGH);
  digitalWrite(Rotazione, HIGH);
  digitalWrite(Spin2_Av, HIGH);
  digitalWrite(Spin2_Ind, HIGH);
  digitalWrite(Etichettatrice, HIGH);
  digitalWrite(Spin1, HIGH);

  /*--- Inizializzazione della piattaforma e del secondo spintore ---*/
  digitalWrite(Rotazione, LOW);
  while (digitalRead(Allineamento) == HIGH);
  digitalWrite(Rotazione, HIGH);
  digitalWrite(Spin2_Ind, LOW);
  delay(500);
  digitalWrite(Spin2_Ind, HIGH);

}

void loop() {
  if ((Temperatura < T_MAX) && (digitalRead(PStop) == HIGH)) {
    digitalWrite(Compressore, LOW);    //Accendo il compressore
    if ((digitalRead(FIng) == HIGH) && (Inserito1 == false)) {
      Inserito1 = true;
      stato[0] = 1;
      tempo1 = millis();
    }

    if (stato[0] == 1) {
      if (((millis() - tempo1) >= T001) && (digitalRead(Allineamento) == LOW)) {
        stato[0] = 0;
        stato[1] = 1;
        tempo1 = millis();
      }
    }

    if (stato[1] == 1) {
      digitalWrite(Spin1, LOW);
      if ((millis() - tempo1) >= T002) {
        stato[1] = 0;
        stato[2] = 1;
      }
    }

    if (stato[2] == 1) {
      digitalWrite(Spin1, HIGH);
      if ((stato[5] == 0) && (stato[6] == 0) && (stato[9] == 0) && (stato[10] == 0)) {
        stato[2] = 0;
        stato[3] = 1;
      }
    }

    if (stato[3] == 1) {
      digitalWrite(Rotazione, LOW);
      if (digitalRead(Allineamento) == HIGH) {
        Inserito1 = false;
        stato[3] = 0;
        stato[4] = 1;
      }
    }

    if (stato[4] == 1) {
      digitalWrite(Rotazione, LOW);
      if (digitalRead(Allineamento) == LOW) {
        stato[4] = 0;
        stato[5] = 1;
        tempo2 = millis();
      }
    }

    if (stato[5] == 1) {
      digitalWrite(Rotazione, HIGH);
      digitalWrite(Etichettatrice, LOW);
      if ((millis() - tempo2) >= T003) {
        stato[5] = 0;
        stato[6] = 1;
        tempo2 = millis();
      }
    }

    if (stato[6] == 1) {
      digitalWrite(Etichettatrice, HIGH);
      if (((millis() - tempo2) >= T004) && (stato[1] == 0) && (stato[9] == 0) && (stato[10] == 0)) {
        stato[6] = 0;
        stato[7] = 1;
      }
    }

    if (stato[7] == 1) {
      digitalWrite(Rotazione, LOW);
      if (digitalRead(Allineamento) == HIGH) {
        stato[7] = 0;
        stato[8] = 1;
      }
    }

    if (stato[8] == 1) {
      digitalWrite(Rotazione, LOW);
      if (digitalRead(Allineamento) == LOW) {
        stato[8] = 0;
        stato[9] = 1;
      }
    }

    if (stato[9] == 1) {
      digitalWrite(Rotazione, HIGH);
      if (digitalRead(FUsc) == LOW) {
        stato[9] = 0;
        stato[10] = 1;
        tempo3 = millis();
      }
    }

    if (stato[10] == 1) {
      digitalWrite(Spin2_Av, LOW);
      if ((millis() - tempo3) >= T002) {
        stato[10] = 0;
        stato[11] = 1;
        tempo3 = millis();
      }
    }

    if (stato[11] == 1) {
      digitalWrite(Spin2_Av, HIGH);
      digitalWrite(Spin2_Ind, LOW);
      digitalWrite(Nastro, LOW);
      if ((millis() - tempo3) >= T002) {
        stato[11] = 0;
        stato[12] = 1;
      }
    }

    if (stato[12] == 1) {
      digitalWrite(Spin2_Ind, HIGH);
      digitalWrite(Nastro, LOW);
      if (digitalRead(FUsc) == HIGH) {
        pezzi_lavorati++;
        /*--- set del cursore in base al numero di pezzi lavorati ---*/
        if (pezzi_lavorati < 9) {
          lcd.setCursor(7, 1);
        } else {
          lcd.setCursor(6, 1);
        }
        lcd.print(pezzi_lavorati);
        stato[12] = 0;
        digitalWrite(Nastro, HIGH);
      }
    }



    /*-------------Interruzione per Emergenza o Temperatura Elevata-------------*/
  } else {
    /*--- Reset di tutti i motori ---*/
    digitalWrite(Nastro, HIGH);
    digitalWrite(Rotazione, HIGH);
    digitalWrite(Spin2_Av, HIGH);
    digitalWrite(Spin2_Ind, HIGH);
    digitalWrite(Etichettatrice, HIGH);
    digitalWrite(Spin1, HIGH);
    digitalWrite(Compressore, HIGH);   //Spengo il compressore
    if (digitalRead(PStop) == LOW) {
      Serial.println("Pulsante di Emergenza attivato, sistema in standby");

      /*--- Memorizzo la differenza di tempo trascorso così da ripartire dallo stesso istante una volta riavviato il sistema ---*/
      tempo1 = millis() - tempo1;
      tempo2 = millis() - tempo2;
      tempo3 = millis() - tempo3;
      tempo_acquisizione = millis() - tempo_acquisizione;

      while (digitalRead(PStop) == LOW);

      /*--- Imposto il timer sulla stessa differenza di tempo trascorso così da ripartire dallo stesso istante in cui ho interrotto il programma ---*/
      tempo1 = millis() - tempo1;
      tempo2 = millis() - tempo2;
      tempo3 = millis() - tempo3;
      tempo_acquisizione = millis() - tempo_acquisizione;

      Serial.println("Pulsante di Emergenza disattivato, sistema in esecuzione");

    } else {
      for (int i = 0; i < 13; i++) stato[i] = 0;  //reset globale di tutti gli stati
      lcd.setCursor(6, 1);
      lcd.print("00");
      pezzi_lavorati = 0;
      Inserito1 = 0;
    }
  }
  if ((millis() - tempo_acquisizione) >= 1000) {
    tempo_acquisizione = millis();
    Temperatura = analogRead(Sens_Temp) * 0.488;
    lcd.setCursor(5, 0);
    lcd.print("   ");
    lcd.setCursor(5, 0);
    lcd.print(Temperatura);
    
    if ((Temperatura < T_MAX) && (digitalRead(PStop) == HIGH)) {
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
}
