
//const int LED=13;
#define delaycons 50 
int count=0;

void setup()
{
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
  
  Serial.begin(9600);
  
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
  int pinval[9] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW};
  // index :  0   1   2   3    4    5    6     7    8
  // var   :  1,  2,  4,  8,  16,  32,  64,  128, 256
  // pin   : 12, 11, 10,  9,   8,   7,   6,    5,   4
  for(int i=8;i>=0;i--){
    int var = pow(2,i);
    if(mm>=var){
      pinval[i] = HIGH;
      mm -= var;
    }
  }
  for(int i=0;i<9;i++){
    digitalWrite(12-i,pinval[i]);
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
