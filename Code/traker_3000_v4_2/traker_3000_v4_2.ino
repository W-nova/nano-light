#include <MsTimer2.h>//定时器库的头文件

#define NS  2     //南北半球(正反转) 

#define a   3     //a+
#define aa  4     //a-
#define b   5     //b+
#define bb  6     //b-


#define SS0  7   //速度编码1
#define SS1  9   //~2
#define SS2  11   //~3
#define Gd1  8  //低电平
#define Gd2  10  //低电平
#define Gd3  12  //低电平

#define scanMs   20   //定义扫描时间

//电机电平表
const int tab[8][4] = {{1,0,0,0}, {1,0,1,0}, {0,0,1,0}, {0,1,1,0},\
{0,1,0,0},{0,1,0,1}, {0,0,0,1}, {1,0,0,1}}; 
static int tab_i = 0;//查表循环

//速度表： {{Ms},{Us}}    恒星速/5X/MAX /1h /2h /3h /5h /12h延时速度
//计算：恒星日：23:56:04 = 86164s /3037.5/96/2 ~ 147.743ms/step
//如需月亮速：89428s /3037.5/96/2~153.340ms/step
const int Sptab[2][8] = {{147 ,29 ,0  ,6  ,12 ,18 ,30 ,73 },\
                         {743 ,547,800,156,311,467,778,868}}; 

long double previousMillis = 0; //上一次转换电平绝对时间

int Comtime = 98; //补偿恒星速中断、读写、判断耗时 us

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
  pinMode(Gd1, OUTPUT); //扩展一个低电平
  pinMode(Gd2, OUTPUT); //扩展一个低电平
  pinMode(Gd3, OUTPUT); //扩展一个低电平
  
  pinMode(NS, OUTPUT); //输入上拉 南北半球(正反转)
  
  pinMode(SS0, INPUT_PULLUP); //speed
  pinMode(SS1, INPUT_PULLUP); //
  pinMode(SS2,  INPUT_PULLUP); //


  MsTimer2::set(scanMs, scanSp);//中断扫描引脚
  MsTimer2::start();
}

void scanSp(){
  int ctl[4]; //0N1S, 2-5Speed
  ctl[0] = digitalRead(NS);
  ctl[1] = digitalRead(SS0);
  ctl[2] = digitalRead(SS1);
  ctl[3] = digitalRead(SS2);
  Speed(ctl);
}

void Speed(int ct[4]) //速度与方向
{
  int spp = ct[3]+2*ct[2]+4*ct[1]; // 0-7
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
    case 0:{      //star000
      ms = Sptab[0][0];
      us = Sptab[1][0] - Comtime;
      }break;
    case 1:{      //5Xstar001
      ms = Sptab[0][1];
      us = Sptab[1][1];
      }break;
    case 7:{      //MAX111
      ms = Sptab[0][2];
      us = Sptab[1][2];
      }break;
    case 2:{      //1h 010
      ms = Sptab[0][3];
      us = Sptab[1][3];
      }break;
    case 3:{      //2h 011
      ms = Sptab[0][4];
      us = Sptab[1][4];
      }break;
    case 4:{      //3h 100
      ms = Sptab[0][5];
      us = Sptab[1][5];
      }break;
    case 5:{      //5h 101
      ms = Sptab[0][6];
      us = Sptab[1][6];
      }break;
    case 6:{      //12h 110
      ms = Sptab[0][7];
      us = Sptab[1][7];
      }break;
    default :{
      }break;
  }
  //Serial.println(ms);
  //Serial.println(us);
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
  digitalWrite(Gd1,LOW);
  digitalWrite(Gd2,LOW);
  digitalWrite(Gd3,LOW);
  digitalWrite(NS,HIGH);
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
