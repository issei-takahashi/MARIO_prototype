#define delaycons 500

void setup() {
  pinMode(3,OUTPUT);  //CTRL

  pinMode(12,OUTPUT); // 5A (1の位) 
  pinMode(11,OUTPUT); // 6A (2の位) 
  pinMode(10,OUTPUT); // 7A (4の位) 
  pinMode(9,OUTPUT);  // 8A (8の位)
  pinMode(8,OUTPUT);  // 9A (16の位)
  pinMode(7,OUTPUT);  //10A (32の位)
  pinMode(6,OUTPUT);  //11A (64の位)
  pinMode(5,OUTPUT);  //12A (128の位)
  pinMode(4,OUTPUT);  //13A (256の位)

  Serial.begin(9600);  //serial channel starts
  
  digitalWrite(3, HIGH);
}


/*
0～9まで順番に変化していく
 */
void loop() {
  zero();
  one(); 
  two();
  three();
  four();
  five();
  six();
}

void  delaytime()
{
  delay(delaycons);
}

void zero()
{
  digitalWrite(12,LOW);
  digitalWrite(11,LOW);
  digitalWrite(10,LOW);
  digitalWrite(9,LOW);
  digitalWrite(8,LOW);
  digitalWrite(7,LOW);
  digitalWrite(6,LOW);
  digitalWrite(5,LOW);
  digitalWrite(4,LOW);

  Serial.print("0mm");

  chgSig();
  delaytime();
}

void one()
{
  digitalWrite(12,LOW);
  digitalWrite(11,LOW);
  digitalWrite(10,LOW);
  digitalWrite(9,LOW);
  digitalWrite(8,LOW);
  digitalWrite(7,LOW);
  digitalWrite(6,LOW);
  digitalWrite(5,HIGH);
  digitalWrite(4,LOW);

  Serial.print("2mm");

  chgSig();
  delaytime();
}

void two()
{
  digitalWrite(12,LOW);
  digitalWrite(11,LOW);
  digitalWrite(10,LOW);
  digitalWrite(9,LOW);
  digitalWrite(8,LOW);
  digitalWrite(7,HIGH);
  digitalWrite(6,LOW);
  digitalWrite(5,LOW);
  digitalWrite(4,LOW);

  Serial.print("8mm");


  chgSig();
  delaytime();
}

void three()
{
  digitalWrite(12,LOW);
  digitalWrite(11,LOW);
  digitalWrite(10,LOW);
  digitalWrite(9,LOW);
  digitalWrite(8,LOW);
  digitalWrite(7,LOW);
  digitalWrite(6,LOW);
  digitalWrite(5,HIGH);
  digitalWrite(4,LOW);

  Serial.print("16mm");


  chgSig();
  delaytime();
}

void four()
{

  digitalWrite(12,LOW);
  digitalWrite(11,LOW);
  digitalWrite(10,LOW);
  digitalWrite(9,HIGH);
  digitalWrite(8,LOW);
  digitalWrite(7,LOW);
  digitalWrite(6,LOW);
  digitalWrite(5,LOW);
  digitalWrite(4,LOW);

  
  Serial.print("32mm");

  chgSig();
  delaytime();
}

void five()
{
  digitalWrite(12,LOW);
  digitalWrite(11,LOW);
  digitalWrite(10,HIGH);
  digitalWrite(9,LOW);
  digitalWrite(8,LOW);
  digitalWrite(7,LOW);
  digitalWrite(6,LOW);
  digitalWrite(5,LOW);
  digitalWrite(4,LOW);

  
  Serial.print("64mm");

  chgSig();
  delaytime();
}

void six()
{
  digitalWrite(12,LOW);
  digitalWrite(11,LOW);
  digitalWrite(10,LOW);
  digitalWrite(9,LOW);
  digitalWrite(8,LOW);
  digitalWrite(7,LOW);
  digitalWrite(6,LOW);
  digitalWrite(5,LOW);
  digitalWrite(4,LOW);

  
  Serial.print("256mm");

  chgSig();
  delaytime();
}

void chgSig()
{
  delay(10);
  digitalWrite(3,LOW);
  delay(100);
  digitalWrite(3,HIGH);
  delay(100);
}


