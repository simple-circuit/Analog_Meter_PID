//Analog Meter PID control and linearization 2025 Simple-Circuit

void setup(void) 
{
 Serial.begin(115200); 
 pinMode(6,OUTPUT);
 pinMode(7,OUTPUT);
 pinMode(8,OUTPUT);
 pinMode(9,INPUT_PULLUP);
 analogWriteResolution(10);
 analogWriteFreq(50000);
}


void loop() 
{
  uint32_t dt = 0;  
  float voltage;
  int i;
  float x=0.0;
  float y=0.0;
  float v2,v3,v4,sum;
  volatile float err = 0.0;    //PID Command error
  volatile float perr = 0.0;   //err times P gain
  volatile float derr = 0.0;   //differential error times D gain
  volatile float dgerr = 0.0;  //derr with low pass filter
  volatile float eold = 0.0;   //previous value of err (needed to calculate derr)
   
 while(true){ 
 
  digitalWrite(8,0);
  if (millis() >= dt) {
    dt = millis() + 5;       //step time = 5ms
    digitalWrite(8,1);
    voltage = 0.0;           //Sum differential ADC inputs 100x at 43.5KHz
    for (i=0;i<100;i++){
      voltage = float(analogRead(A0)-analogRead(A1)) + voltage; 
      delayMicroseconds(12);
    }
    voltage = voltage*0.00025+0.0;  //1.0385*24.65/1024/100 gain=0.00025 offset = 0.0

    if (digitalRead(9)==1) {              //Turn on PID control if Pin-9 is ungrounded
      err = voltage-x;
      perr = err*60.0;                    //Proportional Control
      derr = 2.0*(err-eold)/(0.005);      //Differential Control
      eold = err;                         //Save current err
      dgerr = (0.5*dgerr) + (derr*0.5);   //LP filter derr
      voltage = perr+dgerr;               //P+D for control signal
      if (voltage > 10.0) voltage = 10.0;   //Limit control to +-10V
      if (voltage < -10.0) voltage = -10.0;    
    }   
    
    //State Variable Filter Meter Simulation: w = sqrt(6.05*5)=5.5, df = 6.05/(2*w)=0.55
    y = y + ((voltage - x - y)  * 0.005 * 6.05); //dt*g1=5ms*6.05
    x = x + (y * 0.005 * 5.0); //dt*g2=5ms*5.0
    
    Serial.println(x); //print command voltage before linearity correction
    
    v2 = x * x;      //Polynomial to correct for meter linearity error
    v3 = x * v2;
    v4 = x * v3;
    sum = 0.074319+(voltage*0.97251)+(v2*0.00052471)-(v3*0.000069517)+(v4*0.000018065);  
    
    analogWrite(6,int(sum*51.2)+512); //PWM positive output 512 +- 512
    analogWrite(7,512-int(sum*51.2)); //PWM negative output 515 -+ 512
     
  } 
 }
}
