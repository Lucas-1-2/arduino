
#define angM1min  0
#define angM1max  180
#define angM1Home 140
#define angM2min  15
#define angM2max  140
#define angM2Home 90
#define angM3min  0
#define angM3max  180
#define angM3Home 6
#define angM4min  0
#define angM4max  180
#define angM4Home 10
#define angM5min  0
#define angM5max  180
#define angM5Home 90
#define angM6min  35
#define angM6max  100
#define angM6Home 90

#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27,20,4); 

Servo base;  
Servo shoulder;  
Servo elbow;
Servo wrist;
Servo wristrot;
Servo gripper;


int angM2=angM2Home;   
int angM1=angM1Home;     
int angM3=angM3Home;     
int angM4=angM4Home;
int angM5=angM5Home;
int angM6=angM6Home;

unsigned long int tstamp;

void setup() {
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  base.attach(11);   
  base.write(angM1);
  shoulder.attach(10);  
  shoulder.write(angM2);
  pinMode(52,INPUT_PULLUP);
  pinMode(50,INPUT_PULLUP);
  elbow.attach(9);  
  elbow.write(angM3);
  wrist.attach(6);   
  wrist.write(angM4);
  pinMode(53,INPUT_PULLUP);
  pinMode(51,INPUT_PULLUP);
  wristrot.attach(5);   
  wristrot.write(angM5);
  gripper.attach(3);   
  gripper.write(angM6);
  pinMode(46, INPUT_PULLUP);
  pinMode(48, INPUT_PULLUP);
  pinMode(44, INPUT_PULLUP); 
  delay(20000);
  moveBracciov(angM1Home,angM2Home,angM3Home,angM4Home,angM5Home,angM6Home,20);
  moveBracciov(angM1Home,90,90,90,angM5Home,angM6Home,20);
  moveBracciov(90,90,90,90,angM5Home,angM6Home,20);   
  delay(2000);
  Serial.begin(9600);
  
}

int baseval, Servo2val, gripperval, wristval;
bool elbow_up, elbow_down;
bool wrist_up, wrist_down;
bool shoulder_up, shoulder_down;
bool man, aut;
bool ManAutoN=true;


void loop() {
   
  man=digitalRead(48);
  aut=digitalRead(46);
  if(!man and aut) ManAutoN=true;
  else if(man and !aut) ManAutoN=false;

  if(ManAutoN){
    baseval=analogRead(A8);  
    if(baseval<200) angM1--;
    else if(baseval>800) angM1++;
    
    if(angM1<angM1min) angM1=angM1min;
    else if(angM1>angM1max) angM1=angM1max;
    
    Servo2val=analogRead(A9);  
    if(Servo2val<200) angM2--;
    else if(Servo2val>800) angM2++;
    
    if(angM2<angM2min) angM2=angM2min;
    else if(angM2>angM2max) angM2=angM2max;
    
    elbow_up=digitalRead(52);
    elbow_down=digitalRead(50);
    if(!elbow_up and elbow_down) angM3--;     
    else if(elbow_up and !elbow_down) angM3++;   
   
    if(angM3<angM3min) angM3=angM3min;
    else if(angM3>angM3max) angM3=angM3max;
    
    wrist_up=digitalRead(53);
    wrist_down=digitalRead(51);
    if(!wrist_up and wrist_down) angM4--;     
    else if(wrist_up and !wrist_down) angM4++;   
    
    if(angM4<angM4min) angM4=angM4min;
    else if(angM4>angM4max) angM4=angM4max;
   
    wristval=analogRead(A10);  
    if(wristval<200) angM5--;
    else if(wristval>800) angM5++;
    
    if(angM5<angM5min) angM5=angM5min;
    else if(angM5>angM5max) angM5=angM5max;
    
    gripperval=analogRead(A11);  
    if(gripperval<200) angM6--;
    else if(gripperval>800) angM6++;
    
    if(angM6<angM6min) angM6=angM6min;
    else if(angM6>angM6max) angM6=angM6max;

    
    base.write(angM1);
    shoulder.write(angM2);
    elbow.write(angM3); 
    wrist.write(angM4);
    wristrot.write(angM5); 
    gripper.write(angM6);
   
    
    delay(50);              
  }
  else {

    moveBracciov(90,90,90,90,angM5Home,angM6Home,20);     
    
    if(digitalRead(44)==0){          
      moveBracciov(172,97,22,14,91,35,20);     
      moveBracciov(172,97,22,14,91,100,20);    
      moveBracciov(160,90,60,50,91,100,20);    
      moveBracciov(20,90,60,50,91,100,10);      
      
      moveBracciov(7,93,19,14,91,100,20);     
      
      moveBracciov(7,93,19,14,91,35,20);     
  
    }

  }
  if(millis()-tstamp > 1000){
    Serial.print("Base, angolo: ");
    Serial.print(angM1);
    Serial.print("   M1val: ");
    Serial.println(baseval);
    Serial.print("Shoulder, angolo: ");
    Serial.print(angM2);
    Serial.print("   M2val: ");
    Serial.println(Servo2val);
    Serial.print("Elbow, angolo: ");
    Serial.print(angM3);
    Serial.print("   M3Up: ");
    Serial.print(elbow_up);
    Serial.print("   M3Down: ");
    Serial.println(elbow_down);
    Serial.print("Wrist, angolo: ");
    Serial.print(angM4);
    Serial.print("  M4Up: ");  
    Serial.print(wrist_up);
    Serial.print("  M4Down:  ");
    Serial.println(wrist_down);
    Serial.print("Wrist, angolo");
    Serial.print(angM5);
    Serial.print("  M5val: ");
    Serial.println(wristval);
    Serial.print("Gripper, angolo: ");
    Serial.print(angM6);
    Serial.print("  M6val: ");
    Serial.println(gripperval);
    lcd.setCursor(1,0);
    lcd.print("M1: ");
    lcd.print(angM1);
    lcd.print("  ");  
    lcd.setCursor(12,0);
    lcd.print("M2: ");
    lcd.print(angM2);
    lcd.print("  ");
    lcd.setCursor(1,1);
    lcd.print("M3: ");
    lcd.print(angM3);
    lcd.print("  ");
    lcd.setCursor(12,1);
    lcd.print("M4: ");
    lcd.print(angM4);
    lcd.print("  ");
    lcd.setCursor(1,2);
    lcd.print("M5: ");
    lcd.print(angM5);
    lcd.print("  ");
    lcd.setCursor(12,2);
    lcd.print("M6: ");
    lcd.print(angM6);
    lcd.print("  ");
    lcd.setCursor(8,3);
    if(ManAutoN) lcd.print("MAN ");
    else lcd.print("AUTO");
    tstamp=millis();
  }
  
}

void moveBracciov(int a1, int a2, int a3, int a4, int a5, int a6, int vel){
  
  while(1){    
    if(angM1 < a1) angM1++;
    else if (angM1 > a1) angM1--;
    if(angM1<angM1min) angM1=angM1min;
    else if(angM1>angM1max) angM1=angM1max;
    if(angM2 < a2) angM2++;
    else if (angM2 > a2) angM2--;
     if(angM2<angM2min) angM2=angM2min;
    else if(angM2>angM2max) angM2=angM2max;
    if(angM3 < a3) angM3++;
    else if (angM3 > a3) angM3--;
    if(angM3<angM3min) angM3=angM3min;
    else if(angM3>angM3max) angM3=angM3max;
    if(angM4 < a4) angM4++;
    else if (angM4 > a4) angM4--;
    if(angM4<angM4min) angM4=angM4min;
    else if(angM4>angM4max) angM4=angM4max;
    if(angM5 < a5) angM5++;
    else if (angM5 > a5) angM5--;
    if(angM5<angM5min) angM5=angM5min;
    else if(angM5>angM5max) angM5=angM5max;
    if(angM6 < a6) angM6++;
    else if (angM6 > a6) angM6--;
    if(angM6<angM6min) angM6=angM6min;
    else if(angM6>angM6max) angM6=angM6max;
    base.write(angM1);
    shoulder.write(angM2);
    elbow.write(angM3);
    wrist.write(angM4);
    wristrot.write(angM5);
    gripper.write(angM6);
    delay(vel);
    if((angM1 == a1) and (angM2 == a2) and (angM3 == a3) and (angM4 == a4) and (angM5 == a5) and (angM6 == a6)) break;
   
  }
}
