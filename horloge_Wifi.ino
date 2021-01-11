/**************************************************************************
   Horloge connectée à base d'ESP8266 ou d'ESP32
   Affichage de l'heure, date , temperature et humidité sur un afficheur TM1638 (LED & KEY).
   L'heure est obtenue grâce à un serveur NTP
   
   http://domochris.canalblog.com/archives/2021/01/11/38753401.html

   -11012021 rajout contraste
   -11012021 rajout dht22

   code basé sur http://electroniqueamateur.blogspot.com/2018/10/horloge-wi-fi-esp8266.html
   
*****************************************************************************/

#if defined ARDUINO_ARCH_ESP8266  // s'il s'agit d'un ESP8266
#include <ESP8266WiFi.h>
#elif defined ARDUINO_ARCH_ESP32  // s'il s'agit d'un ESP32
#include "WiFi.h"
#endif

#include <time.h>
#include <ErriezTM1638.h>   // voir https://github.com/Erriez/ErriezTM1638

#include <SimpleDHT.h>

// for DHT22, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 12

const char* ssid = "christoams";
const char* password = "cetelem8";

const int decalage = 1;  // la valeur dépend de votre fuseau horaire. Essayez 2 pour la France.
const int delaiDemande = 60 * 60; // nombre de secondes entre deux demandes consécutives au serveur NTP

// Définition des broches liées au module LED & KEY
#define TM1638_CLK_PIN      16
#define TM1638_DIO_PIN      5
#define TM1638_STB0_PIN     4

// Création d'un objet TM1638
TM1638 tm1638(TM1638_CLK_PIN, TM1638_DIO_PIN, TM1638_STB0_PIN);

unsigned long derniereDemande = millis(); // moment de la plus récente demande au serveur NTP
unsigned long derniereMaJ = millis(); // moment de la plus récente mise à jour de l'affichage de l'heure
time_t maintenant;
struct tm * timeinfo;

//contraste
int Brightness = 1;
int BrightnessTmp =1;

//dht22 temperature humidité
int pinDHT22 = 12;
SimpleDHT22 dht22(pinDHT22);
 float temperature = 0;
 float humidity = 0;


void afficheChiffre(int rang, int valeur, int point)
{
  int leRang, laValeur;

  switch (rang) {
    case 1:
      leRang = 0b00000000;
      break;
    case 2:
      leRang = 0b00000010;
      break;
    case 3:
      leRang = 0b00000100;
      break;
    case 4:
      leRang = 0b00000110;
      break;
    case 5:
      leRang = 0b00001000;
      break;
    case 6:
      leRang = 0b000001010;
      break;
    case 7:
      leRang = 0b000001100;
      break;
    case 8:
      leRang = 0b000001110;
      break;
  }

  switch (valeur) {
    case 0:
      laValeur = 0b00111111 + point * 0b10000000;
      break;
    case 1:
      laValeur = 0b00000110 + point * 0b10000000;
      break;
    case 2:
      laValeur = 0b01011011 + point * 0b10000000;
      break;
    case 3:
      laValeur = 0b01001111 + point * 0b10000000;
      break;
    case 4:
      laValeur = 0b01100110 + point * 0b10000000;
      break;
    case 5:
      laValeur = 0b01101101 + point * 0b10000000;
      break;
    case 6:
      laValeur = 0b01111101 + point * 0b10000000;
      break;
    case 7:
      laValeur = 0b00000111 + point * 0b10000000;
      break;
    case 8:
      laValeur = 0b01111111 + point * 0b10000000;
      break;
    case 9:
      laValeur = 0b01101111 + point * 0b10000000;
      break;
    case 10:  // case vide
      laValeur = 0b00000000;
      break;
  }

  tm1638.writeData(leRang, laValeur);
}

void afficheHeure() {

  timeinfo = localtime(&maintenant);
/*
  // les deux premiers chiffres sont vides
  afficheChiffre(1,  10, 0);
  afficheChiffre(2,  10, 1);
  afficheChiffre(3,  timeinfo->tm_hour / 10, 0); // affichage de l'heure: la dizaine
  afficheChiffre(4,  timeinfo->tm_hour % 10, 1); // affichage de l'heure: l'unité
  afficheChiffre(5,  timeinfo->tm_min / 10, 0); // affichage de la minute: la dizaine
  afficheChiffre(6,  timeinfo->tm_min % 10, 1); // affichage de la minute: l'unité
  afficheChiffre(7,  timeinfo->tm_sec / 10, 0); // affichage de la seconde: la dizaine
  afficheChiffre(8,  timeinfo->tm_sec % 10, 0); // affichage de la seconde: l'unité
*/

if (8 < timeinfo->tm_hour < 20) {BrightnessTmp =3;} //contraste mis a 3 entre 8 et 20h
if (20 >timeinfo->tm_hour > 8) {BrightnessTmp =1;} //contraste mis a 1 entre 20h et 8h

if (BrightnessTmp != Brightness) {
  Brightness = BrightnessTmp;
  tm1638.setBrightness(Brightness);
   Serial.print(Brightness);
  Serial.println(" : changement brigh");
}

/*Serial.println(timeinfo->tm_hour);
Serial.println(timeinfo->tm_hour /10);
Serial.println(timeinfo->tm_hour %10);
*/
//Brightness
/*Serial.print((float)temperature); Serial.print(" *C, ");
  Serial.println((float)humidity); Serial.println(" RH%");
*/
  
temperature = temperature*100; //x 100 pour afficher aprés la virgule
/*  Serial.println((int)temperature /1000);
  Serial.println((int)temperature %1000/100);
  Serial.println((int)temperature %100/10);
  Serial.println((int)temperature %10);
  */
  afficheChiffre(1,  timeinfo->tm_hour / 10, 0); // affichage de l'heure: la dizaine
  afficheChiffre(2,  timeinfo->tm_hour % 10, 1); // affichage de l'heure: l'unité
  afficheChiffre(3,  timeinfo->tm_min / 10, 0); // affichage de la minute: la dizaine
  afficheChiffre(4,  timeinfo->tm_min % 10, 0); // affichage de la minute: l'unité
  afficheChiffre(5,  (int)temperature /1000 , 0); //affiche la temperature dizaine
  afficheChiffre(6,  (int)temperature %1000/100, 1);//affiche la temperature unité
  
  afficheChiffre(7,  (int)temperature %100/10, 0);//affiche la temperature 1er chiffre aprés virgule
  afficheChiffre(8,  0, 0);
}


void afficheDate() {

  int reste;

  timeinfo = localtime(&maintenant);

  afficheChiffre(1,  timeinfo->tm_mday / 10, 0); // affichage du jour: la dizaine
  afficheChiffre(2,  timeinfo->tm_mday % 10, 1); // affichage du jour: l'unité
  afficheChiffre(3,  (timeinfo->tm_mon + 1) / 10, 0); // affichage du mois: la dizaine
  afficheChiffre(4,  (timeinfo->tm_mon + 1) % 10, 1); // affichage du mois: l'unité
  reste = timeinfo->tm_year + 1900;
  afficheChiffre(5,  reste / 1000, 0); // affichage de l'année: les milliers
  reste = reste % 1000;
  afficheChiffre(6,  reste / 100, 0); // affichage de l'année: la centaine
  reste = reste % 100;
  afficheChiffre(7,  reste / 10, 0); // affichage de l'année: la dizaine
  reste = reste % 10;
  afficheChiffre(8,  reste, 0); // affichage de l'année: l'unité

}

void afficheTemp() {

Serial.print((float)temperature); Serial.print(" *C, ");
  Serial.println((float)humidity); Serial.println(" RH%");

temperature = temperature*100; //x 100 pour afficher aprés la virgule
humidity = humidity*100; //x 100 pour afficher aprés la virgule
//Serial.println((float)humidity); Serial.println(" RH%");

   afficheChiffre(1,  (int)temperature /1000 , 0); //affiche la temperature dizaine
  afficheChiffre(2,  (int)temperature %1000/100, 1);//affiche la temperature unité
  
  afficheChiffre(3,  (int)temperature %100/10, 0);//affiche la temperature 1er chiffre aprés virgule
  afficheChiffre(4,  0, 0);
  
 afficheChiffre(5,  (int)humidity /1000 , 0); //affiche la temperature dizaine
  afficheChiffre(6,  (int)humidity %1000/100, 1);//affiche la temperature unité
  
  afficheChiffre(7,  (int)humidity %100/10, 0);//affiche la temperature 1er chiffre aprés virgule
  afficheChiffre(8,  0, 0);
 

}


void setup() {
Serial.begin(115200);
  tm1638.begin();
  tm1638.clear();
  tm1638.setBrightness(Brightness);
  tm1638.displayOn();

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  configTime(decalage * 3600, 0, "fr.pool.ntp.org");  //serveurs canadiens
  //configTime(decalage * 3600, 0, "ca.pool.ntp.org");  //serveurs canadiens
  // en Europe, essayez europe.pool.ntp.org ou fr.pool.ntp.org


  while (time(nullptr) <= 100000) {
    delay(1000);
  }

  time(&maintenant);

}


void loop() {

  uint32_t boutons;

  // état des boutons
  boutons = tm1638.getKeys();

  // est-ce le moment de demander l'heure NTP?
  if ((millis() - derniereDemande) >=  delaiDemande * 1000 ) {
    time(&maintenant);
    derniereDemande = millis();
  }

  // est-ce que millis() a débordé?
  if (millis() < derniereDemande ) {
    time(&maintenant);
    derniereDemande = millis();
  }

  // est-ce le moment de raffraichir la date indiquée?
  if ((millis() - derniereMaJ) >=   1000 ) {
    maintenant = maintenant + 1;

    if (boutons == 0) {  // aucun bouton enfoncé
      afficheHeure();
    }
    if (boutons == 1) {  // bouton S1 enfoncé
      afficheDate();
    }
    if (boutons == 256) {  // bouton S2 enfoncé
      afficheTemp();
     
    }

    derniereMaJ = millis();
  }
 int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
       return;
  }
 

}
