#include <LiquidCrystal.h>


// ===============================
// BROCHES
// ===============================

// === IN PINS ===

const uint8_t p_sensor_pin = A7;    // capteur de pression    
const uint8_t pot_1_pin = A5;       // potentiometres 1 et 2:
const uint8_t pot_2_pin = A6;  

const uint8_t but_mode_pin = A0;         // bouton mode
const uint8_t but_config_pin = A1;      // bouton config
const uint8_t but_fire_pin = A2;      // bouton fire
const uint8_t but_elec_pin = A3;       // bouton electrolyse (moins obligatoire)

// === IN PINS ===

const uint8_t LED_elec_pin = 8;     // electrolyse activée
const uint8_t LED_ready_pin = 9;    // pret à tirer


const uint8_t ev_1_pin = 10;         // relais electrovannes 1 et 2
const uint8_t ev_2_pin = 11;    
const uint8_t elec_pin = 12;         // relais electrolyse
const uint8_t fire_pin = 13;         // relais shot ?




LiquidCrystal lcd(7, 6, 5, 4, 3, 2);


// ===============================
// Variables
// ===============================
  
  // physiques (laisser ?)
  float p = 0;
  int carb = 0;
  int comb = 0;
  unsigned long t0 = millis();
  unsigned long t = millis();


  // capteurs
  int p_sensor = 0;
  int pot_1 = 0;
  int pot_2 = 0;

  // boutons
  bool mode = HIGH;   // Long/bref
  bool config = HIGH; // OS
  bool fire = HIGH;   // OS
  bool elec = HIGH;   // ON/OFF
  bool prev_mode = HIGH;
  bool prev_config = HIGH;
  bool prev_elec = HIGH;
  unsigned long last_mode_LOW = millis();

  // autres
  byte mode_num = 0; // 0 : electrolyse I, 1 : Electrolyse II, 2 : Fire I, 3 : other 
  bool elec_state = 0;
  char buffer[17];
  bool is_config = 0;
  byte config_num = 0; // 0: O2 H2, 1: Air, GPL


// ===============================
// SETUP
// ===============================

void setup() {

  // === Analogique ===
  // Pas besoin de pinMode() obligatoire pour analogRead() sur A0, A1, A2, 
  // mais INPUT explicite pour la clarté :
  pinMode(p_sensor_pin, INPUT);
  pinMode(pot_1_pin, INPUT);
  pinMode(pot_2_pin, INPUT);

  // === boutons ===
  // INPUT_PULLUP active la résistance interne : le bouton lit LOW quand appuyé (relier au GND).
  // Si tu as déjà mis des résistances externes sur ton circuit, remplace par INPUT.
  pinMode(but_mode_pin, INPUT_PULLUP);
  pinMode(but_config_pin, INPUT_PULLUP);
  pinMode(but_fire_pin, INPUT_PULLUP);
  pinMode(but_elec_pin, INPUT_PULLUP);

  // === leds ===
  pinMode(LED_elec_pin, OUTPUT);
  pinMode(LED_ready_pin, OUTPUT);

  // === sorties ===
  pinMode(ev_1_pin, OUTPUT);
  pinMode(ev_2_pin, OUTPUT);
  pinMode(elec_pin, OUTPUT);
  pinMode(fire_pin, OUTPUT);

  // === LCD ===
  lcd.begin(16, 2);
  lcd.setCursor(2, 0);
  lcd.print("Patator V0.0");
  lcd.setCursor(0, 1);
  lcd.print("Lancement...");

  delay(2000);
  lcd.clear();
}


// ===============================
// LOOP
// ===============================

void loop() {
  // Mise à jour du temps écoulé
  t = millis() - t0;

  // --------------------------------
  // 1. Lectures 
  // --------------------------------
  // Analogique
  p_sensor = analogRead(p_sensor_pin); // Valeur brute 0 à 1023
  pot_1    = analogRead(pot_1_pin);    // Valeur brute 0 à 1023
  // pot_2    = analogRead(pot_2_pin);    // Valeur brute 0 à 1023

  // Digital
  mode   = digitalRead(but_mode_pin);
  config = digitalRead(but_config_pin);
  fire   = digitalRead(but_fire_pin);
  elec   = digitalRead(but_elec_pin);


  // --------------------------------
  // 2. Physique
  // --------------------------------

  p = (p_sensor - 98.0)*3.0/(515.0 - 98.0);


  // mode   | 0 : electrolyse I, 1 : Electrolyse II, 2 : Fire I, 3 : other 
  // config | 0: O2, H2 | 1: Air, GPL

  // mode   
  if (mode != prev_mode) {
    prev_mode = mode;
    if (mode == LOW) {
      last_mode_LOW = t;
    }
    
    // étude des boutons

    // mode
    if (mode == HIGH && t - last_mode_LOW < 1000){
      lcd.setCursor(0,0);
      lcd.print("Change mode");
      mode_num++;
      is_config = 0;
    }
    else if (mode == HIGH && t - last_mode_LOW >= 1000) {
      // config 
      lcd.setCursor(0,0);
      lcd.print("Config         ");
      is_config = 1;
    }
    else { 
      is_config = 0;
    }

  }


  // config 
  if (config != prev_config) {
    prev_config = config;
    if (config == LOW && is_config){
      // changement de config
      lcd.setCursor(0,0);
      lcd.print("Change config");
      config_num++;

    }
  
  }


  // fire 
  if (fire == LOW){
    // fire if...
    lcd.setCursor(0,0);
    lcd.print("fire !     ");
  }

  // elec 
  if (elec != prev_elec) {
    prev_elec = elec;
    if (elec == LOW) {
      // change elecstate
        elec_state = !elec_state;
        lcd.setCursor(0,0);
        snprintf(buffer, sizeof(buffer), "Elec: %s", elec_state ? "ON " : "OFF");
        lcd.print(buffer);
    }
  }

  
  
    // affichage
    switch (mode_num % 4) {
  case 0: // elctrolyse I
    
    lcd.setCursor(0,0);
    lcd.print("p : ");
    lcd.print(p, 2);
    lcd.print(" est ");
    lcd.print(0);
    lcd.setCursor(0,1);
    lcd.print("reel ");
    lcd.print(0);
    lcd.print(" actif ");
    lcd.print(elec_state);

    break; // Indispensable pour sortir du switch !

  case 1: // electrolyse II
    lcd.setCursor(0,0);
    lcd.print("tot t ");
    lcd.print(t);
    lcd.print(" | tot prod ");
    lcd.print(0);
    lcd.setCursor(0,1);
    lcd.print("---");
    lcd.print(0);
    lcd.print(" | ---");
    lcd.print(0);
    break;

  case 2: // Fire
    lcd.setCursor(0,0);
    lcd.print("Prog ");
    lcd.print(0);
    lcd.print("% | Mun ");
    lcd.print(4);
    lcd.setCursor(0,1);

    if (config_num % 2 == 0) {
      lcd.print("H2 ");
      lcd.print(0);
      lcd.print("g | O2 ");
      lcd.print(0);
      lcd.print("g");
    }
    else if (config_num % 2 == 1) {
      lcd.print("GPL ");
      lcd.print(pot_1);
      lcd.print("g | AIR ");
      lcd.print(0);  // mettre pot 2 pour version finale
      lcd.print("g");
    }
    break;

  case 3 : // other
    lcd.clear();
    break;
  
  default : // retirer ? (gemini t'en penses quoi ?)
    Serial.println("erreur, mode num");
    break;
  }



  
  delay(100); // Petite pause pour stabiliser l'affichage
}








