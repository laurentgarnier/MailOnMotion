#include "Arduino.h"
#include "driver/rtc_io.h"
#include "mail_management.h"
#include "wifi_management.h"

#define nbAcquisitions 5
#define pinPIR GPIO_NUM_13
#define builtInLed GPIO_NUM_4

char ssidWifi[] = "CG";
char passwordWifi[] = "12ImpEglAnt69";


// Tableau des acquisition définit en PSRAM pour le volume (nbAcquisitions*25ko)
EXT_RAM_ATTR acquisitionResult acquisitions[nbAcquisitions];

int timingDernierMouvementEnMs = 0;
const int intervalMinimumEntreDeuxEnvoiEnSeconde = 900; // 15mn

const int nbTentativesEnvoiMail = 5;

void logMemoire(String header){
  Serial.println("\n\r========= " + header +" =========");
  Serial.println("    Total heap:" + String(ESP.getHeapSize()));
  Serial.println("    Free heap:" + String(ESP.getFreeHeap()));
  Serial.println("    Total PSRAM:" + String(ESP.getPsramSize()));
  Serial.println("    Free PSRAM:" + String(ESP.getFreePsram()));
  Serial.println("========= FIN =========\n\r");
}

void libererBuffers(){
  for(int indexBuffer = 0; indexBuffer < nbAcquisitions; indexBuffer++)
  {
    free(acquisitions[indexBuffer].buffer);
  }
  logMemoire("Apres liberation des buffers");
}

void gererMouvement(){
  // Envoi du mail uniquement toutes les intervalMinimumEntreDeuxEnvoiEnSeconde sauf la 1ere fois
  if(((millis() - timingDernierMouvementEnMs) < intervalMinimumEntreDeuxEnvoiEnSeconde * 1000)
    && timingDernierMouvementEnMs > 0) return;

  Serial.println("OK pour acquisitions et envoi");
  logMemoire("Before acquisitions");

  // allumage de la led
  digitalWrite(builtInLed, HIGH);

  // Lancement des acquisitions
  for(int indexAcquisition = 0; indexAcquisition < nbAcquisitions; indexAcquisition++)
  {
    Serial.println("Acquisition #" + String(indexAcquisition+1));
    acquisitions[indexAcquisition] = takePicture();
    delay(1000);
  }

  // Extinction de la led
  digitalWrite(builtInLed, LOW);
  
  logMemoire("Apres acquisitions");

  int nbEnvoi = 0;
  bool resultatEnvoi = false;
  // Envoi du mail
  do
  {
    resultatEnvoi = sendMail(acquisitions, nbAcquisitions);
    Serial.println("Resultat envoi " + String(resultatEnvoi));
    nbEnvoi++;
    Serial.println("Tentative " + String(nbEnvoi));
  } while (resultatEnvoi == false && nbEnvoi < nbTentativesEnvoiMail);
  
  logMemoire("Apres envoi mail");
  delay(1000);
  libererBuffers();
  if(resultatEnvoi == false) return;

  // le timing du dernier envoi est mis à jour uniquement si OK
  timingDernierMouvementEnMs = millis();
}

void blinkBuiltinLed(int repetition){
  for(int indexRepetition = 0; indexRepetition < repetition; indexRepetition++)
  {
    digitalWrite(builtInLed, HIGH);
    delay(500);
    digitalWrite(builtInLed, LOW);
    delay(500);
  }
}

void setup() {

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("");
  
  pinMode(pinPIR, INPUT);
  pinMode (builtInLed, OUTPUT);

  initCamera();
  // Connection au wifi
  connectToWifi(ssidWifi, passwordWifi);
  blinkBuiltinLed(4);
} 
 
void loop() {
  int state = digitalRead(pinPIR);
  if(state == HIGH)
    gererMouvement();
}