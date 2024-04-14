#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

unsigned char notepin[7]={2,3,4,5,6,7,8};
unsigned char octavepin[8]={9,10,11,12,A1,A2,A3,A4};

//multiplexer

int mpSIG = A0;
int mpS0 = A4;
int mpS1 = A3;
int mpS2 = A2;
int mpS3 = A1;

char controllerCommand[16]{ 78, 77, 76, 75, 74, 73, 72, 71, 70, 82, 81, 80, 51, 55, 91, 94  }; //MIDI CC number

unsigned char SetMuxChannel(byte channel)
{
   digitalWrite(mpS0, bitRead(channel, 0));
   digitalWrite(mpS1, bitRead(channel, 1));
   digitalWrite(mpS2, bitRead(channel, 2));
   digitalWrite(mpS3, bitRead(channel, 3));
}

//MIDI NOTE number
unsigned char note[56]={ 
  36,37,38,39,40,41,42,
  0 ,43,44,45,46,47,48,
  0 ,49,50,51,52,53,54,
  0 ,55,56,57,58,59,60,
  0 ,61,62,63,64,65,66,
  0 ,67,68,69,70,71,72,
  0 ,73,74,75,76,77,78,
  0 ,79,80,81,82,83,84};
  

  
int key[56]={};
int keyb[56]={};
int controller[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} ; 
int controllerb[16]= {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} ; //Set values to 1 to force send actual controller value on boot
int c=0;
int d=0;
void setup() {  

   pinMode(mpS0, OUTPUT);
   pinMode(mpS1, OUTPUT);
   pinMode(mpS2, OUTPUT);
   pinMode(mpS3, OUTPUT);
   pinMode(mpSIG, INPUT);
    
  for (int i=0;i<(sizeof(notepin)/sizeof(notepin[0])); i++){
    pinMode(notepin[i], INPUT);
  }
  for (int i=0;i<(sizeof(octavepin)/sizeof(octavepin[0]));i++){
    pinMode(octavepin[i],OUTPUT);
    digitalWrite(octavepin[i],LOW);
  }

  MIDI.begin();
  delay(200); //Delay 0.2 seconds for send current controllers value to esp8266  
}

void loop() {
  readmatrix(); 
  d++;
  if(d>300) //Prevents overloading MIDI messages when potentiometer reads
  {
  readcontrollers();
  d=0;
  }
  MIDI.read();
}

void comparator(){
  if (keyb[c] != key[c]){
    if (key[c]==1){
      MIDI.sendNoteOn(note[c]+12,127,1);
    }
    else{
      MIDI.sendNoteOff(note[c]+12,0,1);
    }
    keyb[c]=key[c];
  }
}

void sendmidi(byte cmd,int note,int vel){
  Serial.write(cmd);
  Serial.write(note+12);
  Serial.write(vel);
}

void readmatrix(){
  c=0;
  for (int a=0;a<(sizeof(octavepin)/sizeof(octavepin[0]));a++){
    digitalWrite(octavepin[a],HIGH);
    for (int b=0;b<(sizeof(notepin)/sizeof(notepin[0]));b++){
      if (digitalRead(notepin[b]) == HIGH){
        key[c]=1;
      }
      else{
        key[c]=0;
      }
      comparator();
    c++;
    }
    digitalWrite(octavepin[a],LOW);
  }
}

void readcontrollers(){
   for (byte i = 0; i < 16; i++){
      SetMuxChannel(i);
      controller[i] = (analogRead(mpSIG)>>3);
      if (controller[i]!=controllerb[i]){
        controllerb[i]=controller[i];
        MIDI.sendControlChange(controllerCommand[i],controller[i],1);
      }
   }
   digitalWrite(mpS0,LOW);
   digitalWrite(mpS1,LOW);
   digitalWrite(mpS2,LOW);
   digitalWrite(mpS3,LOW);
}
