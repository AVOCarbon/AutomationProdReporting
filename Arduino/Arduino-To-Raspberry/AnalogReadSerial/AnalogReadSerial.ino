/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.
*/
int InitTrame [8];
int StatusTrame [8];
int tampon;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  InitTrame[0] = 1;
  InitTrame[1] = 1;
  InitTrame[2] = 0;
  InitTrame[3] = 0;
  InitTrame[4] = 0;
  InitTrame[5] = 0;
  InitTrame[6] = 0;
  InitTrame[7] = 0;
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  // print out the value you read:
  int  tmp = initTrame();
  if (tmp != tampon)
  {
    Serial.println( initTrame(),DEC);
    tampon = tmp;

  }
  delay(1);        // delay in between reads for stability
}
int initTrame()
{
  int i = 0;
  int fail = -1;
  while (i < 8)
  {
    if (InitTrame[i] != ((analogRead(i) > 512)))
    {
      fail = i;
    }
    StatusTrame[i]  = ((analogRead(i) > 512));
    i++  ;
  }
  return fail;

}
