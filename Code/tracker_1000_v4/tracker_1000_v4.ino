#include <MsTimer2.h>//定时器库的头文件

#define a   3     //a+为接口D2
#define aa  4     //a-为接口D3
#define b   5     //b+为接口D4
#define bb  6     //b-为接口D5

#define NS  2     //南北半球(正反转)  

#define STAR  7   //恒星速度接口
#define Five  8   //5X速度接口
#define Hundred   9   //100X速度接口
#define MAX   10  //最大速度接口
#define Gd   11  //低电平

#define StarMs  443   //定义恒星速
#define FiveMs  88   //定义5x速
#define HundredMs   4      //定义100x速
#define StarUs  210   //
#define FiveUs  642   //
#define HundredUs   432    //
#define MaxUs   900   //定义MAX速

#define scanMs   20   //定义扫描时间

const int tab[8][4] = {{1,0,0,0}, {1,0,1,0}, {0,0,1,0}, {0,1,1,0},\
{0,1,0,0},{0,1,0,1}, {0,0,0,1}, {1,0,0,1}}; //电机电平表
long double previousMillis = 0; //上一次转换电平绝对时间
static int tab_i = 0;
int Comtime = 88; //补偿恒星速中断、读写、判断耗时 us

int ms = 0;
float us = 0.0;

int b1 = b;
int bb1 = bb;

//int time1 = 0;
//int time2;
  
void setup()
{
  //Serial.begin(9600);
  pinMode(a,  OUTPUT);  
  pinMode(aa, OUTPUT); 
  pinMode(b,  OUTPUT); 
  pinMode(bb, OUTPUT); 
  pinMode(Gd, OUTPUT); //扩展一个低电平
  
  pinMode(NS, INPUT_PULLUP); //输入上拉 南北半球(正反转)
  
  pinMode(STAR, INPUT_PULLUP); //speed
  pinMode(Five, INPUT_PULLUP); //
  pinMode(Hundred,  INPUT_PULLUP); //
  pinMode(MAX,  INPUT_PULLUP); //

  MsTimer2::set(scanMs, scanSp);//中断扫描引脚
  MsTimer2::start();
}

void scanSp(){
  int ctl[5]; //0N1S, 2-5Speed
  ctl[0] = digitalRead(NS);
  ctl[1] = digitalRead(STAR);
  ctl[2] = digitalRead(Five);
  ctl[3] = digitalRead(Hundred);
  ctl[4] = digitalRead(MAX);
  Speed(ctl);
}

void Speed(int ct[5]) //速度与方向
{
  int spp = ct[4]+2*ct[3]+4*ct[2]+8*ct[1]; // 7 11 13 14
  //Serial.println(spp);
  if(ct[0]==0){
    b1 = b;
    bb1 = bb;
  }
  else{
    b1 = bb;
    bb1 = b;
  }
  switch(spp){
    case 7:{      //star
      ms = StarMs;
      us = StarUs - Comtime;
      }break;
    case 11:{      //
      ms = FiveMs;
      us = FiveUs;
      }break;
    case 13:{      //
      ms = HundredMs;
      us = HundredUs;
      }break;
    case 14:{      //
      ms = 0;
      us = MaxUs;
      }break;
    default :{
      }break;
  }
  
}

void tt(int a1,int aa1,int b1,int bb1,int tab_i,int us)    //一个电机循环
{
  //time1 = micros();
  digitalWrite(a1,  tab[tab_i][0]);  
  digitalWrite(aa1, tab[tab_i][1]);
  digitalWrite(b1,  tab[tab_i][2]);
  digitalWrite(bb1, tab[tab_i][3]);
  delayMicroseconds (us);           //补偿微秒
  previousMillis += us/1000;        
}

void loop() {
  digitalWrite(Gd,LOW);
  if(ms>=20){           //大于20ms时才使用millis()计时
    long double currentMillis = millis();
    if (currentMillis - previousMillis >= ms) { 
      previousMillis = currentMillis;
      tab_i++;
      if(tab_i>=8) tab_i=0;
      //time2 = micros();
      //Serial.println(time2-time1);
    }
    tt(a,aa,b1,bb1,tab_i,us);
  }
  else{
    for(int i=0;i<=7;i++){
      tt(a,aa,b1,bb1,i,us);
      delay(ms);
    }
  }
  
  
}
