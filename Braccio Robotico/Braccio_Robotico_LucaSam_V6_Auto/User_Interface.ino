void User_Interface() {

  /*--- Gestisco il monitor LCD ---*/
  cont++;
  if (cont > impulsi_interrupt) {
    Cursore(0, 6, 0);   //Scrivo la posizione della base nell'LCD (rotazione, motore 0)
    Cursore(1, 6, 1);   //Scrivo la posizione della spalla nell'LCD (motore 1)
    Cursore(2, 6, 2);   //Scrivo la posizione del gomito nell'LCD (motore 2)
    Cursore(3, 17, 0);  //Scrivo la posizione del polso verticale nell'LCD (motore 3)
    Cursore(4, 17, 1);  //Scrivo la posizione del polso rotazione nell'LCD (motore 4)
    Cursore(5, 17, 2);  //Scrivo la posizione della morsa nell'LCD (motore 5 )
    cont = 0;
  }
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Cursore(int indice, int riga, int colonna) {
  lcd.setCursor(colonna, riga);
  lcd.print("   ");

  if (Posizioni[indice] < 10) {
    lcd.setCursor(colonna + 2, riga);

  } else if ((Posizioni[indice] >= 10) && (Posizioni[indice] < 100)) {
    lcd.setCursor(colonna + 1, riga);

  } else {
    lcd.setCursor(colonna, riga);
  }

  lcd.print(Posizioni[indice]);
}