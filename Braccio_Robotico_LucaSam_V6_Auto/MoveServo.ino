/* La funzione utilizza i parametri nel seguente ordine:
      - Numero della posizione del vettore Posizioni per l'associazione alla variabile che gestisce il motore
      - Lettura del valore associato all'asse desiderato
      - variabile booleana per attivare o meno l'inversione assi
*/
void moveServo(int indice, int VAL, bool reverse) {
  if (abs(VAL - 512) > Zona_morta) {
    float delta = (float)(VAL - (512 + Zona_morta)) / 100;
    if (reverse == true) {
      Posizioni[indice] -= delta;
    } else {
      Posizioni[indice] += delta;
    }
    Posizioni[indice] = constrain(Posizioni[indice], SERVO_MIN[indice], SERVO_MAX[indice]);
    int OUT_Servo = Posizioni[indice];

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

void moveServo_digital(int indice, int Pin_ON, int Pin_OFF, bool reverse) {
  if ((Pin_ON == LOW) && (Pin_OFF == HIGH)) {
    if (reverse) {
      Posizioni[indice] -= Vel_digital;
    } else {
      Posizioni[indice] += Vel_digital;
    }
    Posizioni[indice] = constrain(Posizioni[indice], SERVO_MIN[indice], SERVO_MAX[indice]);
    int OUT_Servo = Posizioni[indice];

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