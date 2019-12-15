#define kp 20                                            
#define ki 0.00
#define kd 20.00
//#define k_dist 5

#define dist_to_wall 40.0
#define error 2.5
#define pwm 100
#define weight 0.5
#define angle_thresh -1.5

float inter_distance = dist_to_wall ; 
int region=1;
int dist1,dist2;
float k_dist;

volatile float sensor_val[2];
float error_angle = 0;
float error_dist = 0;
float total_error = 0;
int angle_prev,angle_current;


float DISTANCE_PER_SENSORVALUE=.017;
int MAX_SENSOR_READING=4000;
int LARGE_SENSOR_READING=15000;
int CONSTANT_DIST=20;
int THRESH_DIST=5;
int THRESH_VALUE=10;
int MAX_CORREC=15;
int THRESH_ANGLE=1;
int speed_avg=120;
int calibration=50;

#define pwm1 8
#define dir1 9                         //for Right Motor
#define dir2 10

#define dir3 12                                    //for Left Motor
#define dir4 11                                    //for motor driver with 2 dir pins
#define pwm2 13

#define trigPin2 4
#define echoPin2 5
#define trigPin1 2
#define echoPin1 3

#define kp_poten A0
#define kd_poten A1

float Read_Sensor1(){
float sensor_value, dist ;
digitalWrite(trigPin1,HIGH);
delayMicroseconds(10);
digitalWrite(trigPin1,LOW);
sensor_value=pulseIn(echoPin1,HIGH);
if (sensor_value==0.0||sensor_value>=LARGE_SENSOR_READING){
    sensor_value=MAX_SENSOR_READING;
  }
  dist=sensor_value*DISTANCE_PER_SENSORVALUE;
  Serial.print("sensor1 = ");
  Serial.print(dist);
  Serial.print("\t");
  return dist;
}

float Read_Sensor2(){
float sensor_value, dist ;
digitalWrite(trigPin2,HIGH);
delayMicroseconds(10);Serial.print("correction: ");
//         Serial.println(error_angle);
digitalWrite(trigPin2,LOW);
sensor_value=pulseIn(echoPin2,HIGH);
if (sensor_value==0.0||sensor_value>=LARGE_SENSOR_READING){
    sensor_value=MAX_SENSOR_READING;
  }
  dist=sensor_value*DISTANCE_PER_SENSORVALUE;
 Serial.print("sensor2 = ");
 Serial.print(dist);
 Serial.print("\t");
 return dist;
}

float Dist_From_Wall(float val1, float val2){
    if(min(val1, val2) < max(val1 , val2) * 0.5 )
      return min(val1 , val2);
    return (val1 + val2)/2;
  }


int Check_Region(float val1, float val2){
  int dist = Dist_From_Wall(val1 , val2);
  if(abs(dist-CONSTANT_DIST)>THRESH_DIST){
    if(dist>CONSTANT_DIST){
       return 2;
    }
    else{
      return 0;
    }
  }
  else
     return 1;
  
}



int Pid_Angle(float val1,float val2)
{
  angle_prev = angle_current;
  angle_current = val1-val2;
  total_error += (angle_current - angle_prev);
  if(angle_current == 0)
    return (0);
  else
         error_angle = kp * angle_current + ki * total_error + kd * (angle_current - angle_prev);        
//         Serial.print("correction: ");
//         Serial.println(error_angle);
         return(error_angle);
  }

float pid_dist(){
  float error_dist = k_dist * ((weight-1) *( Dist_From_Wall(sensor_val[0],sensor_val[1]) - CONSTANT_DIST ));

  return(error_dist);
} 

// int Check_Region(float val1, float val2){
//  int dist =Dist_From_Wall(val1 , val2);
//  if(dist < inter_distance -THRESH_DIST)
//    return 0;
//  if(dist > inter_distance +THRESH_DIST)
//    return 2;
//  else return 1;
//}
//
void Inter_Distance(){
    sensor_val[0] = Read_Sensor1();
    sensor_val[1] = Read_Sensor2();
//    inter_distance = weight * Dist_From_Wall(sensor_val[0],sensor_val[1]) + (1-weight) * dist_to_wall; 
////    Serial.print("inter_distance :");
Serial.println("");
}

void setup(){

  Serial.begin(9600);

  pinMode(pwm1,OUTPUT);
  pinMode(dir1,OUTPUT);
  pinMode(dir2,OUTPUT);
  pinMode(pwm2,OUTPUT);
  pinMode(dir3,OUTPUT);
  pinMode(dir4,OUTPUT);                         
  
  pinMode(trigPin2,OUTPUT);
  pinMode(echoPin2,INPUT);
  pinMode(trigPin1,OUTPUT);
  pinMode(echoPin1,INPUT);
  pinMode(kp_poten,INPUT);
  pinMode(kd_poten,INPUT);

  digitalWrite(dir1,HIGH);
  digitalWrite(dir2,LOW);
  digitalWrite(dir3,LOW);
  digitalWrite(dir4,HIGH);
}

void loop()
{
 k_dist= analogRead(kp_poten)/50.0;
//
//  Serial.println(kp);
// kd= analogRead(kd_poten)/20.0;
//  Serial.println(kd); 
Inter_Distance();
angle_prev = angle_current;
angle_current= sensor_val[1] - sensor_val[0];
region=Check_Region(sensor_val[0], sensor_val[1]);
Serial.print(region);
if(region==0 or region==2){
 
  analogWrite(pwm2, (speed_avg +calibration) + Pid_Angle(sensor_val[0],sensor_val[1]) + pid_dist());
  analogWrite(pwm1, speed_avg- Pid_Angle(sensor_val[0],sensor_val[1]) - pid_dist());
//  Serial.print("error_dist :");
//  Serial.print(pid_dist());
//  Serial.print("\t");
//    Serial.print("error_angle :");
//  Serial.print(Pid_Angle(sensor_val[0],sensor_val[1]));
//  Serial.print("\t");
//  Serial.print("pwm2 :");
//  Serial.print((speed_avg +calibration) + Pid_Angle(sensor_val[0],sensor_val[1]) + pid_dist());
//  Serial.print("\t");
//   Serial.print("pwm1 :");
//  Serial.print(speed_avg- Pid_Angle(sensor_val[0],sensor_val[1]) - pid_dist());
//  Serial.print("\t");
}
else{
  if(abs(angle_current)>THRESH_ANGLE){
    analogWrite(pwm2, (speed_avg +calibration) + Pid_Angle(sensor_val[0],sensor_val[1]));
    analogWrite(pwm1, speed_avg - Pid_Angle(sensor_val[0],sensor_val[1]));
//      Serial.print("pwm2 :");
//  Serial.print((speed_avg +calibration) + Pid_Angle(sensor_val[0],sensor_val[1]));
//  Serial.print("\t");
//   Serial.print("pwm1 :");
//  Serial.print(speed_avg- Pid_Angle(sensor_val[0],sensor_val[1]));
//  Serial.print("\t");
    }
  else{
    analogWrite(pwm2, (speed_avg +calibration));
    analogWrite(pwm1, speed_avg);
//      Serial.print("pwm2 :");
//  Serial.print((speed_avg +calibration) );
//  Serial.print("\t");
//   Serial.print("pwm1 :");
//  Serial.print(speed_avg);
//  Serial.print("\t");
 }
}


  
}
