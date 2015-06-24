/*

This code uses the s-gps test code, but has my settings.

*/
 
//Change here your data
//const char pin[]="your_pin"; //This is commented out because it wasn't referenced in the original version
const char apn[]="fast.t-mobile.com";
const char user_name[]="your_user_name";
const char password[]="your_password";
const char gps_server[]="198.37.116.205";

int8_t answer;
int onModulePin= 2;
char gps_data[100];
int counter;
char aux_str[150];


void setup(){

  pinMode(onModulePin, OUTPUT);
  Serial.begin(115200);    

  Serial.println("Starting...");
  power_on();

  delay(5000);

  Serial.println("Connecting to the network...");

  while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
    sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

  // sets APN, user name and password
  snprintf(aux_str, sizeof(aux_str), "AT+CGSOCKCONT=1,\"IP\",\"%s\"", apn);
  sendATcommand(aux_str, "OK", 2000);

  // if not authentication requiered comment or remove the next two lines
  snprintf(aux_str, sizeof(aux_str), "AT+CSOCKAUTH=1,1,\"%s\",\"%s\"", user_name, password);
  sendATcommand(aux_str, "OK", 2000);


  // sets GPS server
  snprintf(aux_str, sizeof(aux_str), "AT+CGPSURL=\"%s\"", gps_server);
  sendATcommand(aux_str, "OK", 2000);

  // without certificate
  sendATcommand("AT+CGPSSSL=0", "OK", 2000);

  // starts GPS session in MS-based mode
  answer = sendATcommand("AT+CGPS=1,2","OK",1000); 
  if (answer == 0)
  {
    Serial.println("Error starting the GPS");
    Serial.println("The code stucks here!!");
    while(1);
  }
}
void loop(){

  answer = sendATcommand("AT+CGPSINFO","+CGPSINFO:",1000);    // request info from GPS
  if (answer == 1)
  {

    counter = 0;
    do{
      while(Serial.available() == 0);
      gps_data[counter] = Serial.read();
      counter++;
    }
    while(gps_data[counter - 1] != '\r');
    gps_data[counter] = '\0';
    if(gps_data[0] == ',')
    {
      Serial.println("No GPS data available");  
    }
    else
    {
      Serial.print("GPS data:");
      Serial.println(gps_data);  
      Serial.println("");
    }       

  }
  else
  {
    Serial.println("Error"); 
  }

  delay(5000);
}

void power_on(){

  uint8_t answer=0;

  // checks if the module is started
  answer = sendATcommand("AT", "OK", 2000);
  if (answer == 0)
  {
    // power on pulse
    digitalWrite(onModulePin,HIGH);
    delay(3000);
    digitalWrite(onModulePin,LOW);

    // waits for an answer from the module
    while(answer == 0){    
      // Send AT every two seconds and wait for the answer
      answer = sendATcommand("AT", "OK", 2000);    
    }
  }

}


int8_t sendATcommand(char* ATcommand, char* expected_answer1, unsigned int timeout)
{

  uint8_t x=0,  answer=0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  while( Serial.available() > 0) Serial.read();    // Clean the input buffer

  Serial.println(ATcommand);    // Send the AT command 


    x = 0;
  previous = millis();

  // this loop waits for the answer
  do{

    if(Serial.available() != 0){    
      response[x] = Serial.read();
      x++;
      // check if the desired answer is in the response of the module
      if (strstr(response, expected_answer1) != NULL)    
      {
        answer = 1;
      }
    }
    // Waits for the answer with time out
  }
  while((answer == 0) && ((millis() - previous) < timeout));    

  return answer;
}
        

