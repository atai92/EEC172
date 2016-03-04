//Willie Huey 996679150
//Alan Tai 996844794
#define NELEMS(x)  (sizeof(x)/sizeof(x[0]))
#define PIN_SCE   7 //Pin 3 on LCD
#define PIN_RESET 6 //Pin 4 on LCD
#define PIN_DC    5 //Pin 5 on LCD
#define PIN_SDIN  4 //Pin 6 on LCD
#define PIN_SCLK  3 //Pin 7 on LCD
#define LCD_COMMAND 0 
#define LCD_DATA  1
#define LCD_X     84
#define LCD_Y     48

// OAuth Key
#define TOKEN "2533601430-pFm5k9RI1loXAyUhc64skkwfYPZYZ5bN3ykoIai"

// Twitter Proxy
#define LIB_DOMAIN "arduino-tweet.appspot.com"
#define ircmsg "PRIVMSG #Sparky :"

TCPClient client;//IRC client
TCPClient twitter;//Twitter client
char buffer[150];//IRC message buffer

char tweet[100];//tweet message to send
bool wasavailable = false;
int ivar = 0;//index for buffer
int qvar = 0;//to clear out the buffer

void setup()
{
    delay(1000);
  //establish IRC connection
  if (client.connect("irc.freenode.net", 6667))
  {
    client.write("NICK SparkyBot\r\n");
    client.write("USER SparkyBot 8 * : SparkyBot\r\n");
    client.write("JOIN #Sparky\r\n");
    client.write("PRIVMSG #Sparky :Hello world!\r\n");
  }
}

void loop()
{
    while (client.available()) {
        char c = client.read();//read characters from IRC
        buffer[ivar] = c;//store message from IRC in buffer
        if (c == '\n') {//if end of message reset index
            ivar=0;
            break;
        }
        ivar++;
        if (ivar > 150) {//if message too long
            ivar = 0;
        }
        wasavailable = true;//flag to tell if client was available to process buffer
    }
    if (wasavailable) {
        if (posfind("PING",buffer) == 0) {//send pong back so you don't timeout.
            buffer[1] = 'O';
            client.write(buffer);
        }
        if(posfind("#tweet",buffer) != -1)//send tweet if "#tweet" is in message
        {
            int p = posfind("#tweet",buffer) + 7;//get the position after #tweet.
            for (int iterator = 0; iterator < 300; iterator++) {
                tweet[iterator] = buffer[p+iterator];//get message to be tweeted
            }
            delay(500);
            if (twitter.connect(LIB_DOMAIN, 80)) {//establish TCP connection to twitter
                twitter.println("POST /update HTTP/1.0");
                twitter.println("Host: " LIB_DOMAIN);
                twitter.print("Content-Length: ");
                twitter.println(strlen(tweet)+strlen(TOKEN)+14);
                twitter.println();
                twitter.print("token=");
                twitter.print(TOKEN); //send the OAuth token here so we can tweet
                twitter.print("&status=");
                twitter.println(tweet); //send the tweet message here
            }
        }
        else if (posfind("Hello SparkyBot", buffer) != -1) {//if message including "Hello SparkyBot" found display these messages
            int rval = rand() % 6;
            switch (rval) {
                case 0:
                    client.write("PRIVMSG #Sparky :Hello!\r\n");
                    break;
                case 1:
                    client.write("PRIVMSG #Sparky :What's up!?\r\n");
                    break;
                case 2:
                    client.write("PRIVMSG #Sparky :Oh, Hi there...\r\n");
                    break;
                case 3:
                    client.write("PRIVMSG #Sparky :Hey there buddy!\r\n");
                    break;
                case 4:
                    client.write("PRIVMSG #Sparky :How's it goin!?\r\n");
                    break;
                default:
                    client.write("PRIVMSG #Spark :Yo\r\n");
                    break;
            }
        }
        else if (posfind("Welcome SparkyBot", buffer) != -1) {//if message including "Welcome SparkyBot" found display these messages
            int rval = rand() % 5; //gives a random number from 0 to 4
            switch (rval) {
                case 0:
                    client.write("PRIVMSG #Sparky :Thanks!\r\n");
                    break;
                case 1:
                    client.write("PRIVMSG #Sparky :Thank you!\r\n");
                    break;
                case 2:
                    client.write("PRIVMSG #Sparky :Aww, thanks!\r\n");
                    break;
                case 3:
                    client.write("PRIVMSG #Sparky :Well aren't you just a sweetie. Thank you.\r\n");
                    break;
                default:
                    client.write("PRIVMSG #Sparky :Aww thanks cutie pie.\r\n");
                    break;
            }
        }
        else if (posfind("Go away SparkyBot",buffer) != -1) {//if message including "Go away SparkyBot" found disconnection from irc and rejoin after a few seconds
            client.write("PRIVMSG #Sparky :Okay...\r\n");
            delay(500);
            client.write("QUIT :");
            char name[40];
            getname(buffer, name);
            client.write(name);
            client.write(" does not like me... :(\r\n");
            delay(500);
            client.stop();
            delay(2000);
            if (client.connect("irc.freenode.net", 6667))
            {
                client.write("NICK SparkyBot\r\n");
                client.write("USER SparkyBot 8 * : SparkyBot\r\n");
                client.write("JOIN #Sparky\r\n");
                client.write("PRIVMSG #Sparky :Hello. I'm back!\r\n");
            }
        }
        else if (posfind("#init self destruct",buffer) != -1) {//if message including "#init self destruct SparkyBot" found disconnection from irc
            char name[40];
            getname(buffer, name);
            delay(100);
            client.write(ircmsg);
            delay(100);
            client.write(name);
            delay(100);
            client.write(" has initiated the self-destruct sequence. Self-destructing in 3...\r\n");
            delay(1000);
            client.write("PRIVMSG #Sparky :2...\r\n");
            delay(1000);
            client.write("PRIVMSG #Sparky :1...\r\n");
            delay(1000);
            client.write("QUIT :SELF-DESTRUCT SEQUENCE COMPLETE! \r\n");
            client.stop();
        }
        else if (posfind("SparkyBot",buffer) != -1 && posfind("PRIVMSG #Sparky :", buffer) != -1)//default message if "SparkyBot" is referred to
        {
            int rval = rand() % 6;
            switch (rval) {
                case 0:
                    client.write("PRIVMSG #Sparky :U mad\r\n");
                    break;
                case 1:
                    client.write("PRIVMSG #Sparky :Cool story bro\r\n");
                    break;
                case 2:
                    client.write("PRIVMSG #Sparky :The cake is a lie\r\n");
                    break;
                case 3:
                    client.write("PRIVMSG #Sparky :Did I give you permission to talk to me?\r\n");
                    break;
                case 4:
                    client.write("PRIVMSG #Sparky :Tell me more\r\n");
                    break;
                default:
                    client.write("PRIVMSG #Spark :Destroy all humans\r\n");
                    break;
            }
        }
        for (qvar = 0; qvar < 150; qvar++) buffer[qvar] = NULL;
        wasavailable = false;
    }
}

int posfind(char *findthis, char *source) { //Gives the position of the start of the "findthis" string.
    char *dest = strstr(source, findthis);
    int pos;
    pos = dest - source;
    if (!dest) pos = -1;
    return pos;
}

void getname(char * source, char output[40]) {//Get the name of user that posted.
    for (int i = 0; i < 150; i++) {
        if (source[i+1] == '!') {
            output[i] = '\0';
            break;
        }
        output[i] = source[i+1];
    }
}
