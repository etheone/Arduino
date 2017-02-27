int rxPin = 12;
 
 
void setup()
{       pinMode(rxPin, INPUT);
        Serial.begin(9600);
}
 
 
void loop()
{
        int i = 0;
        unsigned long t = 0;
 
        byte prevBit = 0;
        byte bit = 0;
 
        unsigned long sender = 0;
        bool group = false;
        bool on = false;
        unsigned int recipient = 0;
 
        // latch 1
        while((t < 9480 || t > 10350))
        {       t = pulseIn(rxPin, LOW, 1000000);
        }
 
        // latch 2
        while(t < 2550 || t > 2700)
        {       t = pulseIn(rxPin, LOW, 1000000);
        }
 
        // data
        while(i < 64)
        {
                t = pulseIn(rxPin, LOW, 1000000);
 
                if(t > 200 && t < 365)
                {       bit = 0;
                }
                else if(t > 1000 && t < 1360)
                {       bit = 1;
                }
                else
                {       i = 0;
                        break;
                }
 
                if(i % 2 == 1)
                {
                        if((prevBit ^ bit) == 0)
                        {       // must be either 01 or 10, cannot be 00 or 11
                                i = 0;
                                break;
                        }
 
                        if(i < 53)
                        {       // first 26 data bits
                                sender <<= 1;
                                sender |= prevBit;
                        }      
                        else if(i == 53)
                        {       // 26th data bit
                                group = prevBit;
                        }
                        else if(i == 55)
                        {       // 27th data bit
                                on = prevBit;
                        }
                        else
                        {       // last 4 data bits
                                recipient <<= 1;
                                recipient |= prevBit;
                        }
                }
 
                prevBit = bit;
                ++i;
        }
 
        // interpret message
        if(i > 0)
        {       printResult(sender, group, on, recipient);
        }
}
 
 
void printResult(unsigned long sender, bool group, bool on, unsigned int recipient)
{
        Serial.print("sender ");
        Serial.println(sender);
 
        if(group)
        {       Serial.println("group command");
        }
        else
        {       Serial.println("no group");
        }
 
        if(on)
        {       Serial.println("on");
        }
        else
        {       Serial.println("off");
        }
 
        Serial.print("recipient ");
        Serial.println(recipient);
 
        Serial.println();
}
