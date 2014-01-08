
//const int LED=13;
#define delaycons 50 
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
  
  changeHeight(0);
  chgSig();
  delaytime();
}

int pow(int x,int n)
{
  if(n==0){
    return 1;
  }
  else{
    return x*pow(x,n-1);
  }
}

void loop()
{
  int data=-1;
  static int height = 0;
  if( Serial.available() ){
    while( Serial.available() ){
      data = Serial.read();
    }
    Serial.flush();
    if( abs(height-data) < 30 ){  // 今回の移動が3cm以下だったら
      changeHeight(data);
      height = data;
      chgSig();
      delaytime();
    }
  }
}

void  delaytime()
{
  delay(delaycons);
}

void changeHeight(int mm)
{
  if(mm<0){ 
    mm=0; 
  }
  else if(mm>280){ 
    mm=280; 
  }
  int cm=mm/10;
  int keta[5] = {LOW,LOW,LOW,LOW,LOW}; // 1,2,4,8,16に対応
  // 1,2,4,8,16 -> pin 8,9,10,11,12
  for(int i=4;i>=0;i--){
    int I = pow(2,i);
    if(cm>=I){
      keta[i] = HIGH;
      cm-=I;
    }
  }
  for(int i=0;i<5;i++){
    digitalWrite(8+i,keta[i]);
  }
}

void four()
{
 digitalWrite(8,LOW); 
  digitalWrite(9,LOW);
  digitalWrite(10,HIGH);
  digitalWrite(11,LOW);
  digitalWrite(12,LOW);

  //  Serial.print("40mm");

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
