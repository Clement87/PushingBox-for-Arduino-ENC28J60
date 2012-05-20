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
static byte mymac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x19};   // Be sure this address is unique in your network

//Your secret DevID from PushingBox.com. You can use multiple DevID  on multiple Pin if you want
//#define DEVID1 "Your_DevID_Here"        //Scenario : "The mailbox is open"
#define DEVID1 "v586BA8C2E39BD1D"

//Numeric Pin where you connect your switch
#define pinDevid1 3  // Example : the mailbox switch is connect to the Pin 3

// Debug mode
#define DEBUG true
  ///////
 //End//
///////

char website[] PROGMEM = "api.pushingbox.com";
byte Ethernet::buffer[700];
boolean pinDevid1State = false;


// called when the client request is complete
static void my_result_cb (byte status, word off, word len) {
  if(DEBUG){Serial.print("<<< reply ");}
  if(DEBUG){Serial.println((const char*) Ethernet::buffer + off);}
}

void setup () {
  Serial.begin(9600);
  
  pinMode(pinDevid1, INPUT);
  
  if(DEBUG){Serial.println("\n[getDHCPandDNS]");}
  if (ether.begin(sizeof Ethernet::buffer, mymac, 10) == 0) 
    if(DEBUG){Serial.println( "Failed to access Ethernet controller");}
    
  if (!ether.dhcpSetup())
    if(DEBUG){Serial.println("DHCP failed");}
  
  if(DEBUG){
    ether.printIp("My IP: ", ether.myip);
    ether.printIp("Netmask: ", ether.mymask);
    ether.printIp("GW IP: ", ether.gwip);
    ether.printIp("DNS IP: ", ether.dnsip);
  }

  if (!ether.dnsLookup(website))
    if(DEBUG){Serial.println("DNS failed");}
  if(DEBUG){ether.printIp("Server: ", ether.hisip);}
}

void loop () {
  
  // DHCP expiration is a bit brutal, because all other ethernet activity and
  // incoming packets will be ignored until a new lease has been acquired
  if (ether.dhcpExpired() && !ether.dhcpSetup())
    if(DEBUG){Serial.println("DHCP failed");}
    
  ether.packetLoop(ether.packetReceive());
  
      ////
      // Listening for the pinDevid1 state
      ////
      if (digitalRead(pinDevid1) == HIGH && pinDevid1State == false) // switch on pinDevid1 is ON 
      {
        if(DEBUG){Serial.println("pinDevid1 is HIGH");}
        pinDevid1State = true;
        //Sending request to PushingBox when the pin is HIGHT
        ether.browseUrl(PSTR("/pushingbox?devid="), DEVID1, website, my_result_cb);
        delay(500);
      }
       if (digitalRead(pinDevid1) == LOW && pinDevid1State == true) // switch on pinDevid1 is OFF
      {
        if(DEBUG){Serial.println("pinDevid1 is LOW");}
        pinDevid1State = false;
        //Sending request to PushingBox when the pin is LOW
        //ether.browseUrl(PSTR("/pushingbox?devid="), DEVID1, website, my_result_cb);
        delay(500);
      }
}
