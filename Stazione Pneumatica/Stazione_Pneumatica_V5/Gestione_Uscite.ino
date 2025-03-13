
void Stato_Vuoto() {
  digitalWrite(Nastro, HIGH);
  digitalWrite(Rotazione, HIGH);
  digitalWrite(Spin2_Av, HIGH);
  digitalWrite(Spin2_Ind, HIGH);
  digitalWrite(Etichettatrice, HIGH);
  digitalWrite(Spin1, HIGH);
}

void Set_Uscite(bool qn1, bool qn2, bool qn3, bool qn4, bool qn5, bool qn6) {
  digitalWrite(Nastro, qn1);
  digitalWrite(Rotazione, qn2);
  digitalWrite(Spin2_Av, qn3);
  digitalWrite(Spin2_Ind, qn4);
  digitalWrite(Etichettatrice, qn5);
  digitalWrite(Spin1, qn6);
}
