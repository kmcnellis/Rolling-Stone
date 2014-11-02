int incomingByte = 0;   // for incoming serial data

int speakerPin = 7;

int length = 43; // the number of notes
char notes[] = "b b D b D   b a b b b   Dsb b b b   FsFsFs  b b D b Ds  b a b b b   Dsb b b b   FsFsFs"; // a space represents a rest
int beats[] = {1,1,2,2,4,6,1,1,2,2,4,8,1,1,1,1,4,4,4,2,2,10,1,1,2,2,4,6,1,1,2,2,4,8,1,1,1,1,4,4,4,2,2};
int tempo = 100;

void playTone(int freq, int duration) {
  Serial.println(freq);
  int tone = 1000000L/freq;
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, char modif, int duration) {
  char names[] = {'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C', 'D', 'E', 'F', 'G', 'A', 'B'};
  int tones[] = { 262, 277, 294, 311, 330, 330, 349, 370, 392, 415, 440, 466, 494, 494, 523, 554, 587, 622, 659, 659, 698, 740, 784, 831, 880, 932, 988, 988};

  // play the tone corresponding to the note name
  for (int i = 0; i < 16; i++) {
    if (names[i] == note) {
      i *=2;
      if (modif=='f'){
        i-=1;
      }else if(modif=='s'){
        i+=1;
      }
      playTone(tones[i], duration);
    }
  }
}

void setup() {
        Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
        pinMode(speakerPin, OUTPUT);

}

void loop() {

        // send data only when you receive data:
        if (Serial.available() > 0) {
                // read the incoming byte:
                incomingByte = Serial.read();
                for (int i = 0; i < length; i++) {
                  if (notes[i*2] == ' ') {
                    delay(beats[i] * tempo); // rest
                  } else {
                    playNote(notes[i*2], notes[(i*2)+1], beats[i] * tempo);
                  }

                  // pause between notes
                  delay(tempo / 2); 
                }
                Serial.flush();
        }
}
