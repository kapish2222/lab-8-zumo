int motorright = 9;                                                              
int motorrightdir  = 7;
int motorleft = 10;
int motorleftdir  = 8;

#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/UInt16.h>
#include <SoftwareSerial.h>

class BlueROS : public ArduinoHardware
{
private:
  SoftwareSerial *mySerial;
public:
  
  BlueROS() {
  }
  void init() {
    mySerial = new SoftwareSerial(A0, 11);
    mySerial->begin(57600);
  }
  int read() {
    return mySerial->read();
  };
  void write(uint8_t* data, int l) {
    for (int i = 0; i < l; i++) {
      mySerial->write(data[i]);
    }
  }
};
ros::NodeHandle_<BlueROS, 3, 3, 100, 100> nh;


std_msgs::UInt16 leftsensor;
std_msgs::UInt16 frontsensor;
std_msgs::UInt16 rightsensor;
ros::Publisher publeft("/zumo/left", &leftsensor);
ros::Publisher pubfront("/zumo/front", &frontsensor);
ros::Publisher pubright("/zumo/right", &rightsensor);


// defines pins numbers
const int echoPinLeft = 2;
const int trigPinLeft =A2;

const int echoPinFront = 5;
const int trigPinFront = A4;

const int echoPinRight = 4;
const int trigPinRight = A3;

// defines variables
long duration;
int distance;

void ros_handler( const geometry_msgs::Twist& cmd_msg) {
  float x = cmd_msg.linear.x;
  float z = cmd_msg.angular.z;
  if(x < 0) forward(60);
  if(z == 0) forward(60);
  if(z > 0) right(60);
  if(z < 0) left(60);
  if(z == 1) stop();
}

ros::Subscriber<geometry_msgs::Twist> sub("cmd_vel", ros_handler);

void setup() {
  pinMode(trigPinLeft, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPinLeft, INPUT); // Sets the echoPin as an Input
  pinMode(trigPinFront, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPinFront, INPUT); // Sets the echoPin as an Input
  pinMode(trigPinRight, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPinRight, INPUT); // Sets the echoPin as an Input
  
  pinMode(motorright, OUTPUT);                                                      
  pinMode(motorleft, OUTPUT);     
  pinMode(motorrightdir, OUTPUT);  
  pinMode(motorleftdir, OUTPUT);  

  nh.initNode();
  nh.subscribe(sub);
  nh.advertise(publeft);
  nh.advertise(pubfront);
  nh.advertise(pubright);
}

void loop() 
{
  leftsensor.data = ultrasonic(echoPinLeft,trigPinLeft);
  frontsensor.data = ultrasonic(echoPinFront,trigPinFront);
  rightsensor.data = ultrasonic(echoPinRight,trigPinRight);

  publeft.publish( &leftsensor);
  pubfront.publish( &frontsensor);
  pubright.publish( &rightsensor);

  delay(100);

  nh.spinOnce();
  delay(100);
}

//direction is controlled by the digital pin 7 and 8.
// HIGH is forward, LOW is backward
// Pins 9 and 10 control speed.
// Length of time controls the distance

void forward(int time)
{
digitalWrite(motorrightdir, LOW);
analogWrite(motorright,120); 
digitalWrite(motorleftdir, LOW);
analogWrite(motorleft, 120); 
delay(time);
}

void backward(int time)
{
digitalWrite(motorrightdir, HIGH);
analogWrite(motorright,120); 
digitalWrite(motorleftdir, HIGH);
analogWrite(motorleft, 120); 
delay(time);
}

void left(int time)
{
digitalWrite(motorrightdir, LOW);
analogWrite(motorright,120); 
digitalWrite(motorleftdir, HIGH);
analogWrite(motorleft, 120); 
delay(time);
}

void right(int time)
{
digitalWrite(motorrightdir, HIGH);
analogWrite(motorright,120); 
digitalWrite(motorleftdir, LOW);
analogWrite(motorleft, 120); 
delay(time);
}


void stop()
{
analogWrite(motorright, 0); 
analogWrite(motorleft, 0); 
}

int ultrasonic(int echoPin, int trigPin) {
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);
// Calculating the distance
distance= duration*0.034/2;

return distance;
}
