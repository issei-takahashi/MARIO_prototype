//const int LED=13;
#define delaycons 400
int count=0;

void setup()
{
  Serial.begin(9600);
  pinMode(8,OUTPUT);  //5A(1の位)
  pinMode(9,OUTPUT);  //6A(2の位)
  pinMode(10,OUTPUT);  //7A(4の位)
  pinMode(11,OUTPUT);  //8A(8の位)
  pinMode(12,OUTPUT);  //9A(16の位) 

  pinMode(3,OUTPUT);  //ctrl
}

void loop()
{
  if(Serial.available()>0)
  {
    int data = Serial.read();
    Serial.flush();
    switch(data){
      case 'a': digitalWrite(8+count, LOW); count++; break;
      case 'b': digitalWrite(8+count, HIGH); count++; break;
//    case '2': two(); break;
//    case '3': three(); break;
//    case '4': four(); break;
//    case '5': five(); break;
     }
    
    if(count==5)
    {
      chgSig();
      count=0;
    }
//    digitalWrite(LED, HIGH);
  }   
  else{
//    digitalWrite(LED, LOW);
  }
  delay(100);
  
}


void  delaytime()
{
  delay(delaycons);
}

void zero()
{
  digitalWrite(9,LOW);
  digitalWrite(10,LOW);
  digitalWrite(11,LOW);
  digitalWrite(12,LOW);
 
//  Serial.print("0mm");

  chgSig();
  delaytime();
}

void one()
{
  digitalWrite(9,HIGH);
  digitalWrite(10,LOW);
  digitalWrite(11,LOW);
  digitalWrite(12,LOW);

//  Serial.print("10mm");

  chgSig();
  delaytime();
}

void two()
{
  digitalWrite(9,LOW);
  digitalWrite(10,HIGH);
  digitalWrite(11,LOW);
  digitalWrite(12,LOW);
  
//  Serial.print("20mm");


  chgSig();
  delaytime();
}

void three()
{
  digitalWrite(9,HIGH);
  digitalWrite(10,HIGH);
  digitalWrite(11,LOW);
  digitalWrite(12,LOW);

//  Serial.print("30mm");


  chgSig();
  delaytime();
}

void four()
{
  digitalWrite(9,LOW);
  digitalWrite(10,LOW);
  digitalWrite(11,HIGH);
  digitalWrite(12,LOW);

//  Serial.print("40mm");

  chgSig();
  delaytime();
}

void five()
{
  digitalWrite(9,HIGH);
  digitalWrite(10,LOW);
  digitalWrite(11,HIGH);
  digitalWrite(12,LOW);

//  Serial.print("50mm");

  chgSig();
  delaytime();
}

void chgSig()
{
  delay(10);
  digitalWrite(3,HIGH);
  delay(40);
  digitalWrite(3,LOW);
  delay(50);
}
  

