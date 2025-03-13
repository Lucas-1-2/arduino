void moveBracciov(int a0, int a1, int a2, int a3, int a4, int a5, int vel) {

  while (1) {  // aggiungere una condizione di sicurezza per fermare il braccio durante i movimenti automatici
    if (Posizioni[0] < a0) Posizioni[0]++;
    else if (Posizioni[0] > a0) Posizioni[0]--;

    if (Posizioni[1] < a1) Posizioni[1]++;
    else if (Posizioni[1] > a1) Posizioni[1]--;

    if (Posizioni[2] < a2) Posizioni[2]++;
    else if (Posizioni[2] > a2) Posizioni[2]--;

    if (Posizioni[3] < a3) Posizioni[3]++;
    else if (Posizioni[3] > a3) Posizioni[3]--;

    if (Posizioni[4] < a4) Posizioni[4]++;
    else if (Posizioni[4] > a4) Posizioni[4]--;

    if (Posizioni[5] < a5) Posizioni[5]++;
    else if (Posizioni[5] > a5) Posizioni[5]--;


    for (int i = 0; i < 6; i++) {
      Posizioni[i] = constrain(Posizioni[i], 0, 179);
      int OUT_Servo = map(Posizioni[i], 0, 179, SERVO_MIN[i], SERVO_MAX[i]);
      // scrivo nei motori ////////
      switch (i) {
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
          Serial.println("Errore nell'assegnazione del valore OUT_Servo all'indice (moveBracciov)");
          break;
      }
    }
    delay(vel);

    if((Posizioni[0] == a0) and (Posizioni[1] == a1) and (Posizioni[2] == a2) and (Posizioni[3] == a3) and (Posizioni[4] == a4) and (Posizioni[5] == a5)) break;

    /*-- Condizione di sicurezza per uscire dal ciclo while infinito, così torniamo nel loop per controllare i pulsanti modalità --*/
    if (digitalRead(Pin_Man) == LOW) break;
  }
}
