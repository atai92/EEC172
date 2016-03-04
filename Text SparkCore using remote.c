#define PIN_SCE   7 //Pin 3 on LCD
#define PIN_RESET 6 //Pin 4 on LCD
#define PIN_DC    5 //Pin 5 on LCD
#define PIN_SDIN  4 //Pin 6 on LCD
#define PIN_SCLK  3 //Pin 7 on LCD

#define LCD_COMMAND 0 
#define LCD_DATA  1

#define LCD_X     84
#define LCD_Y     48

#define StartDelimiter 0
#define LengthMSB 1
#define LengthLSB 2
#define APIIdentifier 3
#define FrameID 4
#define DestAddressMSB 5
#define DestAddressLSB 6
#define Options 7
#define RFData 8
#define headertrailercount 9

char incomingByte = 0;   // for incoming serial data
int p = 0;// discard trash
int length_MSB = 0;// to record incoming length for MSB
int length_LSB = 0;// to record incoming length for LSB
int TOUT = 1700000;// timeout for button presses
char data[100];// receiving data
char buffer[100];//output data
int length;//length of incoming packet
int led = D7;
char sequence[17];//sequence for button press
int slength = 0;//index for sequence
int tout = 0;//not used
int pina0 = A0; //ir signal pin
int length_of_bit, t1, t2;//length of ir signal bit, time 1 and time 2 for delta calculation 
int t3, t4;//time 3 and time 4 for delta calculation for timeout
int touttime = 0;//time out time for button presses
int x = 0;//not used
int packetrcv;//flag to tell packet received
char current_letter = '.';// used so we can output same character twice
char prev_letter;
char packet[110];//output packet
int letter = 0;//index for buttons array
int num_letters = 0;//index for the buffer
int i;//index variable
int temp = 0;
int checksum;//checksum for outgoing
int datalength = 0;//length of outgoing data

//button arrays
char *two[8] = {"A","B","C","a","b","c","2"};
char *three[8] = {"D","E","F","d","e","f","3"};
char *four[8] = {"G","H","I","g","h","i","4"};
char *five[8] = {"J","K","L","j","k","l","5"};
char *six[8] = {"M","N","O","m","n","o","6"};
char *seven[10] = {"P","Q","R","S","p","q","r","s","7"};
char *eight[8] = {"T","U","V","t","u","v","8"};
char *nine[10] = {"W","X","Y","Z","w","x","y","z","9"};
char *zero[3] = {" ", "0"};

static const byte ASCII[][5] = { 
    {0x00, 0x00, 0x00, 0x00, 0x00} // 20  
  ,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
  ,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
  ,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
  ,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
  ,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
  ,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
  ,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
  ,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
  ,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
  ,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
  ,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
  ,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
  ,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
  ,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
  ,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
  ,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
  ,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
  ,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
  ,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
  ,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
  ,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
  ,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
  ,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
  ,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
  ,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
  ,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
  ,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
  ,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
  ,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
  ,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
  ,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
  ,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
  ,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
  ,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
  ,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
  ,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
  ,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
  ,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
  ,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
  ,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
  ,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
  ,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
  ,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
  ,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
  ,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
  ,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
  ,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
  ,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
  ,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
  ,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
  ,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
  ,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
  ,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
  ,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
  ,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
  ,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
  ,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
  ,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
  ,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
  ,{0x02, 0x04, 0x08, 0x10, 0x20} /* 5c \ */
  ,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
  ,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
  ,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
  ,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
  ,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
  ,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
  ,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
  ,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
  ,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
  ,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
  ,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
  ,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
  ,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
  ,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j 
  ,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
  ,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
  ,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
  ,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
  ,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
  ,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
  ,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
  ,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
  ,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
  ,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
  ,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
  ,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
  ,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
  ,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
  ,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
  ,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
  ,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
  ,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
  ,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
  ,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ~
  ,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f DEL
  };

void setup() {
	//initialize everything
    LCDInit(); //Init the LCD
    Serial1.begin(9600);     // opens serial port, sets data rate to 9600 bps
    pinMode(led, OUTPUT);
    pinMode(pina0, INPUT);
    touttime = 0;
    slength = 0;
    t1 = 0;
    t2 = 0;
    t3 = 0;
    t4 = 0;
    for(x = 0; x < 16; x++)
    {
        sequence[x] = NULL;
    }
    //initialize packet
    packet[StartDelimiter] = 0x7E;
	packet[LengthMSB] = 0x00;
	packet[APIIdentifier] = 0x01;
	packet[FrameID] = 0x00;
	packet[DestAddressMSB] = 0x00;
	packet[DestAddressLSB] = 0x00;
	packet[Options] = 0x01;
    
    packetrcv = 0;
	//setup interrupts
    attachInterrupt(A0, *RISING_EDGE_HANDLER, RISING);
    interrupts();
}

void loop() {
	//map sequences to buttons and create data for packet
    if (slength == 16){
        if(strcmp(sequence,"lllssslslsslssss") == 0)
        {
            t3 = t4;
            t4 = micros();
            touttime = t4-t3;
            prev_letter = current_letter;
			current_letter = '0';
			if (touttime > TOUT && current_letter == prev_letter) { prev_letter = '.'; }
			if (current_letter == prev_letter) { num_letters--; datalength--; if (letter == 1) letter = 0; else letter++; }
			if (current_letter != prev_letter) letter = 0;
			buffer[num_letters] = zero[letter][0];
            LCDString(buffer);
            packet[RFData+datalength] = zero[letter][0];
			num_letters++; datalength++;
			touttime = 0;
        }
        else if(strcmp(sequence,"lllssslsssssssss") == 0)
        {
            t3 = t4;
            t4 = micros();
            touttime = t4-t3;
            prev_letter = current_letter;
			current_letter = '1';
			if (touttime > TOUT && current_letter == prev_letter) { prev_letter = '.'; }
			if (current_letter == prev_letter) { num_letters--; datalength--; if (letter == 6) letter = 0; else letter++; }
			if (current_letter != prev_letter) letter = 0;
			buffer[num_letters] = '1';
			LCDString(buffer);
			packet[RFData+datalength] = '1';
			num_letters++; datalength++;
			touttime = 0;
        }
        else if(strcmp(sequence,"lllssslsssslssss") == 0)
        {
            t3 = t4;
            t4 = micros();
            touttime = t4-t3;
            prev_letter = current_letter;
			current_letter = '2';
			if (touttime > TOUT && current_letter == prev_letter) { prev_letter = '.'; }
			if (current_letter == prev_letter) { num_letters--; datalength--; if (letter == 6) letter = 0; else letter++; }
			if (current_letter != prev_letter) letter = 0;
			buffer[num_letters] = two[letter][0];
			LCDString(buffer);
			packet[RFData+datalength] = two[letter][0];
			num_letters++; datalength++;
			touttime = 0;
        }
        else if(strcmp(sequence,"lllssslssssslsss") == 0)
        {
            t3 = t4;
            t4 = micros();
            touttime = t4-t3;
            prev_letter = current_letter;
			current_letter = '3';
			if (touttime > TOUT && current_letter == prev_letter) { prev_letter = '.'; }
			if (current_letter == prev_letter) { num_letters--; datalength--; if (letter == 6) letter = 0; else letter++; }
			if (current_letter != prev_letter) letter = 0;
			buffer[num_letters] = three[letter][0];
            LCDString(buffer);
			packet[RFData+datalength] = three[letter][0];
			num_letters++; datalength++;
			touttime = 0;
        }
        else if(strcmp(sequence,"lllssslssssllsss") == 0)
        {
            t3 = t4;
            t4 = micros();
            touttime = t4-t3;
            prev_letter = current_letter;
			current_letter = '4';
			if (touttime > TOUT && current_letter == prev_letter) { prev_letter = '.'; }
			if (current_letter == prev_letter) { num_letters--; datalength--; if (letter == 6) letter = 0; else letter++; }
			if (current_letter != prev_letter) letter = 0;
			buffer[num_letters] = four[letter][0];
			LCDString(buffer);
			packet[RFData+datalength] = four[letter][0];
			num_letters++; datalength++;
			touttime = 0;
        }
        else if(strcmp(sequence,"lllssslsssssslss") == 0)
        {
            t3 = t4;
            t4 = micros();
            touttime = t4-t3;
            prev_letter = current_letter;
			current_letter = '5';
			if (touttime > TOUT && current_letter == prev_letter) { prev_letter = '.'; }
			if (current_letter == prev_letter) { num_letters--; datalength--; if (letter == 6) letter = 0; else letter++; }
			if (current_letter != prev_letter) letter = 0;
			buffer[num_letters] = five[letter][0];
			LCDString(buffer);
			packet[RFData+datalength] = five[letter][0];
			num_letters++; datalength++;
			touttime = 0;
        }
        else if(strcmp(sequence,"lllssslsssslslss") == 0)
        {
            t3 = t4;
            t4 = micros();
            touttime = t4-t3;
            prev_letter = current_letter;
			current_letter = '6';
			if (touttime > TOUT && current_letter == prev_letter) { prev_letter = '.'; }
			if (current_letter == prev_letter) { num_letters--; datalength--; if (letter == 6) letter = 0; else letter++; }
			if (current_letter != prev_letter) letter = 0;
			buffer[num_letters] = six[letter][0];
			LCDString(buffer);
			packet[RFData+datalength] = six[letter][0];
			num_letters++; datalength++;
			touttime = 0;
        }
        else if(strcmp(sequence,"lllssslsssssllss") == 0)
        {
            t3 = t4;
            t4 = micros();
            touttime = t4-t3;
            prev_letter = current_letter;
			current_letter = '7';
			if (touttime > TOUT && current_letter == prev_letter) { prev_letter = '.'; }
			if (current_letter == prev_letter) { num_letters--; datalength--; if (letter == 8) letter = 0; else letter++; }
			if (current_letter != prev_letter) letter = 0;
			buffer[num_letters] = seven[letter][0];
			LCDString(buffer);
			packet[RFData+datalength] = seven[letter][0];
			num_letters++; datalength++;
			touttime = 0;
        }
        else if(strcmp(sequence,"lllssslsssslllss") == 0)
        {
            t3 = t4;
            t4 = micros();
            touttime = t4-t3;
            prev_letter = current_letter;
			current_letter = '8';
			if (touttime > TOUT && current_letter == prev_letter) { prev_letter = '.'; }
			if (current_letter == prev_letter) { num_letters--; datalength--; if (letter == 6) letter = 0; else letter++; }
			if (current_letter != prev_letter) letter = 0;			
			buffer[num_letters] = eight[letter][0];
			LCDString(buffer);
			packet[RFData+datalength] = eight[letter][0];
			num_letters++; datalength++;
			touttime = 0;
        }
        else if(strcmp(sequence,"lllssslslsssssss") == 0)
        {
            t3 = t4;
            t4 = micros();
            touttime = t4-t3;
            prev_letter = current_letter;
			current_letter = '9';
			if (touttime > TOUT && current_letter == prev_letter) { prev_letter = '.'; }
			if (current_letter == prev_letter) { num_letters--; datalength--; if (letter == 8) letter = 0; else letter++; }
			if (current_letter != prev_letter) letter = 0;
			buffer[num_letters] = nine[letter][0];
			LCDString(buffer);
			packet[RFData+datalength] = nine[letter][0];
			num_letters++; datalength++;
			touttime = 0;
        }
        else if(strcmp(sequence,"lllssslssslsssss") == 0)
        {
            prev_letter = current_letter;
			current_letter = '-';
			if (num_letters != 0)
				num_letters--;
			datalength--;
			buffer[num_letters] = NULL;
			LCDString(buffer);
			touttime = 0;
            //LCDString("DELETE");
        }
        else if(strcmp(sequence,"lllssslsslssllss") == 0)
        {
            LCDClear();
            num_letters = 0;
            //When "enter" button is pressed, calculate checksum.
			// ++Calculate checksum
			packet[LengthLSB] = 5+datalength;
			for (i = 0; i < datalength+5; i++) {
				temp = temp + packet[i+3];
			}
			checksum = RFData+datalength;
			packet[checksum] = temp;
			temp = packet[checksum];
			temp = 0xff - temp;
			packet[checksum] = temp;
			for (i = 0; i < headertrailercount+datalength; i++) {
			    Serial1.write(packet[i]);
			    if (i < 100) buffer[i] = NULL;
			}
			datalength = 0;
			temp = 0;
			touttime = 1700000;
			letter = 0;
        }
        delay(250);
        slength = 0;
    }
	//if packet received output the packets data
    if(packetrcv == 1)
    {
        //LCDClear();
        LCDString(data);
        packetrcv = 0;
         //remember to clear data!
        //LCDString(sequence);
        //interrupts();
    }
	//read the incoming data from packet
    if (Serial1.available() > 0) {
                // read the incoming byte:
                incomingByte = Serial1.read();
                if( p == 2) //want to ignore the first three bytes. We only need the length to determine how long the data is.
                {
                        length = incomingByte;
                }
                else if( p > 7)
                {
                        data[p - 8] = incomingByte;
                }
                p++;
        }
    if (Serial1.available() < 1 && p > 0) { //want to clear the checksum byte.
        packetrcv = 1;
        for (int i = length-5; i < 100; i++) {
            data[i] = NULL; 
        }
    }
    if (Serial1.available() < 1) {            
        p = 0;
    }
}

void RISING_EDGE_HANDLER() {//on rising edge calculate delta of previous time from previous rising edge and current time from current rising edge
    t1 = t2;
    t2 = micros();
    if (t1 != 0) {
        if (t2 - t1 < 0) {
            return;
        }
        if (t2 - t1 > 1200) {
            sequence[slength] = 'l';
        }
        else
            sequence[slength] = 's';
        slength++;
    }
    if (slength == 16) {
        t2 = 0;
    }
}

void gotoXY(int x, int y) {
  LCDWrite(0, 0x80 | x);  // Column.
  LCDWrite(0, 0x40 | y);  // Row.  ?
}

void LCDClear(void) {
  for (int index = 0 ; index < (LCD_X * LCD_Y / 8) ; index++)
    LCDWrite(LCD_DATA, 0x00);
    
  gotoXY(0, 0); //After we clear the display, return to the home position
}

void LCDCharacter(char character) {
  LCDWrite(LCD_DATA, 0x00); //Blank vertical line padding

  for (int index = 0 ; index < 5 ; index++)
    LCDWrite(LCD_DATA, ASCII[character - 0x20][index]);
    //0x20 is the ASCII character for Space (' '). The font table starts with this character

  LCDWrite(LCD_DATA, 0x00); //Blank vertical line padding
}

//Given a string of characters, one by one is passed to the LCD
void LCDString(char *characters) {
  LCDClear();    
  while (*characters)
    LCDCharacter(*characters++);
}


void LCDInit(void) {

  //Configure control pins
  pinMode(PIN_SCE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);

  //Reset the LCD to a known state
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_RESET, HIGH);

  LCDWrite(LCD_COMMAND, 0x21); //Tell LCD that extended commands follow
  LCDWrite(LCD_COMMAND, 0xB8); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
  LCDWrite(LCD_COMMAND, 0x04); //Set Temp coefficent
  LCDWrite(LCD_COMMAND, 0x14); //LCD bias mode 1:48: Try 0x13 or 0x14

  LCDWrite(LCD_COMMAND, 0x20); //We must send 0x20 before modifying the display control mode
  LCDWrite(LCD_COMMAND, 0x0C); //Set display control, normal mode. 0x0D for inverse
}

void LCDWrite(byte data_or_command, byte data) {
  digitalWrite(PIN_DC, data_or_command); //Tell the LCD that we are writing either to data or a command

  //Send the data
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}

