//Analog Meter Calibration 2025 Simple-Circuit
//Use serial out to adjust gain and offset for ADC
//Use PWM 6,7 out for +-50uA meter adjust 

void setup(void) {
 Serial.begin(115200); 
 pinMode(6,OUTPUT);          //+PWM Out
 pinMode(7,OUTPUT);          //-PWM Out
 pinMode(8,OUTPUT);          //Timing test pin
 analogWriteResolution(10);
 analogWriteFreq(50000);    //133MHz/1024=129.8KHz so counter is good for 10-bits
}

void loop() {
  uint32_t dt = 0;  
  float voltage;
  int i;
  
 while(true){ 
   if (millis() >= dt) {
     dt = millis() + 5; //loop step time = 5ms  
     voltage = 0.0;
     for (i=0;i<100;i++){   //Average 100 differential readings at 43.5KHz
      digitalWrite(8,1);
      voltage = float(analogRead(A0)-analogRead(A1)) + voltage; 
      delayMicroseconds(12);
      digitalWrite(8,0);
     }
     //Convert 10-bit values to +-10V range
     voltage = voltage*0.0002479+0.0;  //gain=1.03*24.65/1024/100 and offset=0.0
    
     Serial.println(voltage,2);            //print the voltage for calibration
     analogWrite(6,int(voltage*51.2)+512); //PWM +out 512 +- 512
     analogWrite(7,512-int(voltage*51.2)); //PWM -out 512 -+ 512  
  } 
 }
}
