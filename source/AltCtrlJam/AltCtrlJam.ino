/*  Ricardo Tejada 2016
 *  Arduino Sketch for Alt Ctrl Jam game entry.
 *  This code runs on the Arduino for the controller.
 *  The schematic will be uploaded eventually.
 */

int l_meter = 0;  // light sensor
int l_meter_min = 1023;
int l_meter_max = 0;

byte p_meter = 0;  // potentiaometer
byte p_meter_anim = 0;

byte button = 0; // simple button

byte color_red = 0;
byte color_green = 0;
byte color_blue = 0;

void setup()
{
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, INPUT);

  Serial.begin(9600);
}

void loop()
{
  button = digitalRead(12);

  if(!button)
    calibrate();

  l_meter = analogRead(A0);
  
  if(l_meter > l_meter_max)
    l_meter_max = l_meter;
  if(l_meter < l_meter_min)
    l_meter_min = l_meter;
  
  l_meter = map(l_meter, l_meter_min, l_meter_max, 0, 255);
  l_meter = 255 - l_meter;
  
  p_meter = map(analogRead(A1), 0, 1023, 0, 255);
  p_meter_anim = map(p_meter, 0, 255, 0, 63) % 8;
  
  if(Serial.available() && Serial.read() == 'A')  // Is the line open for communication?
  {
    Serial.write('A');  // Begin message
    Serial.write(p_meter);
    Serial.write(p_meter_anim);
    Serial.write(l_meter);
    Serial.write(button);
    Serial.flush();
  }

  //analogWrite(9, color_red);
  //analogWrite(10, color_green);
  //analogWrite(11, color_blue);
}

void calibrate()
{

}




