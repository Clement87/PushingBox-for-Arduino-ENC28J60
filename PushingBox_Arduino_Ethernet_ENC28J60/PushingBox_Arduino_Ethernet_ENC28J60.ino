////
//
// General code from http://www.pushingbox.com for Arduino + Ethernet Shield (ENC28J60)
// Library EtherCard from https://github.com/jcw/ethercard is under mit license
//
////
#include <EtherCard.h>

  /////////////////
 // MODIFY HERE //
/////////////////
static byte mymac[] = {0x24,0x99,0x46,0xAD,0x30,0x31};   // Be sure this address is unique in your network

//Your secret DevID from PushingBox.com. You can use multiple DevID  on multiple Pin if you want
char DEVID1[] = "Your_DevID_Here";        //Scenario : "The mailbox is open"

//Numeric Pin where you connect your switch
uint8_t pinDevid1 = 3;  // Example : the mailbox switch is connect to the Pin 3

// Debug mode
boolean DEBUG = true;
  ///////
 //End//
///////

const char website[] PROGMEM = "api.pushingbox.com";
byte Ethernet::buffer[700];
Stash stash;
boolean pinDevid1State = false;  // Save the last state of the Pin for DEVID1

void setup () {
  Serial.begin(9600);
  pinMode(pinDevid1, INPUT);
  
  if(DEBUG){Serial.println("\n[getDHCPandDNS]");}
  
  //
  //***Depending of your Shield, you may have to try this line instead of the second***//
  //if(ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
  if(ether.begin(sizeof Ethernet::buffer, mymac, 10) == 0) 
    if(DEBUG){Serial.println( "Failed to access Ethernet controller");}
  
  // Wait until we have an IP from the DHCP
  while(!ether.dhcpSetup()){
    if(DEBUG){Serial.println("Error: DHCP failed. Can't get an IP address, let's retry...");}
  }
  
  if(DEBUG){
    ether.printIp("My IP: ", ether.myip);
    ether.printIp("Netmask: ", ether.netmask);
    ether.printIp("GW IP: ", ether.gwip);
    ether.printIp("DNS IP: ", ether.dnsip);
  }

  if (!ether.dnsLookup(website))
    if(DEBUG){Serial.println("DNS failed");}
  if(DEBUG){ether.printIp("Server: ", ether.hisip);}
  
 
}

void loop () {
 
    ether.packetLoop(ether.packetReceive());
    
      ////
      // Listening for the pinDevid1 state
      ////
      if (digitalRead(pinDevid1) == HIGH && pinDevid1State == false) // switch on pinDevid1 is ON 
      {
        if(DEBUG){Serial.println("pinDevid1 is HIGH");}
        pinDevid1State = true;
        //Sending request to PushingBox when the pin is HIGHT
        ether.browseUrl(PSTR("/pushingbox?devid="), DEVID1, website, my_callback);
        delay(500);  
      }
       if (digitalRead(pinDevid1) == LOW && pinDevid1State == true) // switch on pinDevid1 is OFF
      {
        if(DEBUG){Serial.println("pinDevid1 is LOW");}
        pinDevid1State = false;
        //Sending request to PushingBox when the pin is LOW
        //ether.browseUrl(PSTR("/pushingbox?devid="), DEVID1, website, my_callback);
        delay(500);
      }
      
}

// called when the client request is complete
static void my_callback (byte status, word off, word len) {
  Serial.println(">>>");
  Ethernet::buffer[off+300] = 0;
  Serial.print((const char*) Ethernet::buffer + off);
  Serial.println("...");
}
