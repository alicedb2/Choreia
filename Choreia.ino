#include <avr/pgmspace.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <MCP4725.h>
#include <ADS1X15.h>
#include "scales.h"
#include "definitions.h"
#include "src/TrueRandom.h"

MCP4725 MCP(0x60);
ADS1115 ADS(0x48);

const uint8_t rs = 12, en = 11, d4 = 4, d5 = 5, d6 = 7, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

u16 markov_seed;

float *random_uniform_matrix;
float *markov_matrix;
u8 *semitone_vector_in_scale;


u8 PARAM_scale_width = 4;
float PARAM_scale_dispersion = 3.0;
float PARAM_jump_to_first_neighbors = 1.33;
float PARAM_jump_to_root = 0.33;
float PARAM_repeat_note = 0.33;
float PARAM_jump_to_second_neighbors = 1.00;

// We save parameters after a switch flip so that
// we can keep them where they were when we flip back
u8 switched_PARAM_scale_width = 4;
float switched_PARAM_scale_dispersion = 3.0;
float switched_PARAM_jump_to_first_neighbors = 0.33;
float switched_PARAM_jump_to_root = 0.33;
float switched_PARAM_repeat_note = 0.33;
float switched_PARAM_jump_to_second_neighbors = 1.0;



long unsigned int last_markovian_display_refresh;

u8 current_note = 0;
u8 current_semitone = 0;

bool sequencer_alternates = true;
int8_t sequencer_direction = 1;
int8_t sequencer_step, previous_sequencer_step;

bool gate_events[16];
volatile bool advance_sequencer = false;

bool startup = true;
bool GLOBAL_refresh_display = true;

u8 PARAM_nb_euclidean_events = 17;
u8 PARAM_nb_shadow_steps = 17;
int8_t PARAM_sequence_rotation = 17;
u8 PARAM_gate_probability = 101;
u8 PARAM_scale_idx = 127;
u8 PARAM_root = 127;
u16 PARAM_scale = CHROMATIC;

u8 switched_PARAM_nb_euclidean_events = 17;
u8 switched_PARAM_nb_shadow_steps = 17;
int8_t switched_PARAM_sequence_rotation = 17;
u8 switched_PARAM_gate_probability = 101;
u8 switched_PARAM_scale_idx = 127;
u8 switched_PARAM_root = 127;


bool switch_state, last_switch_state;
long unsigned int last_switch_change;
bool fast_double_switch_change;

void setup() {
  ////////////////////////
  pinMode(GATE_PIN, OUTPUT);
  pinMode(CLOCKIN_PIN, INPUT);
  pinMode(GATEPROB_PIN, INPUT);
  pinMode(ROOT_PIN, INPUT);
  pinMode(SWITCH_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(CLOCKIN_PIN), clock_change, CHANGE);

  Serial.flush();
  Serial.begin(115200);
  Serial.flush();

  Wire.begin();
  Wire.setClock(400000);

  MCP.begin();

  ADS.begin();
  ADS.setGain(0);

  lcd.begin(16, 2);
  lcd.createChar(0, Filled_lowo);
  lcd.createChar(1, Filled_higho_underbar);
  lcd.createChar(2, Underbar);

  lcd.createChar(3, Two_bars);
  lcd.createChar(4, Four_bars);
  lcd.createChar(5, Six_bars);

  lcd.clear();


  //////////////////////
  markov_seed = TrueRandom.random(0, 65536);
  randomSeed(markov_seed);

  switch_state = digitalRead(SWITCH_PIN);
  last_switch_state = switch_state;
  last_switch_change = millis();

  initialize_sequencer_variables();
  GLOBAL_refresh_display = true;

  if (switch_state) { /// Euclidean last

    conditional_set_markovian_parameters(ADS.readADC(DISPERSION_PIN), ADS.readADC(REPEATNOTE_PIN), ADS.readADC(SCALEWIDTH_PIN), ADS.readADC(FIRSTNEIGHBOR_PIN), analogRead(JUMPTOROOT_PIN), analogRead(SECONDNEIGHBOR_PIN));
    lcd.setCursor(0, 0);
    lcd.print(EMPTY_LINE);
    lcd.setCursor(0, 1);
    lcd.print(EMPTY_LINE);
    conditional_set_and_display_scale(ADS.readADC(SCALE_PIN));
    conditional_set_and_display_root(analogRead(ROOT_PIN));
    conditional_set_and_display_euclidean_sequence(ADS.readADC(EUCL_PIN), ADS.readADC(SHADOW_PIN), ADS.readADC(ROTATION_PIN));
    conditional_set_and_display_gate_probability(analogRead(GATEPROB_PIN));

  } else {  // Markovian last

    conditional_set_and_display_scale(ADS.readADC(SCALE_PIN));
    conditional_set_and_display_root(analogRead(ROOT_PIN));
    conditional_set_and_display_euclidean_sequence(ADS.readADC(EUCL_PIN), ADS.readADC(SHADOW_PIN), ADS.readADC(ROTATION_PIN));
    conditional_set_and_display_gate_probability(analogRead(GATEPROB_PIN));
    lcd.setCursor(0, 0);
    lcd.print(EMPTY_LINE);
    lcd.setCursor(0, 1);
    lcd.print(EMPTY_LINE);
    conditional_set_markovian_parameters(ADS.readADC(DISPERSION_PIN), ADS.readADC(REPEATNOTE_PIN), ADS.readADC(SCALEWIDTH_PIN), ADS.readADC(FIRSTNEIGHBOR_PIN), analogRead(JUMPTOROOT_PIN), analogRead(SECONDNEIGHBOR_PIN));
  }

  last_markovian_display_refresh = millis();
  startup = false;
  GLOBAL_refresh_display = false;
}


void loop() {
  //    Serial.print(digitalRead(CLOCKIN_PIN));
  //    Serial.print("  ");
  //    Serial.print(ADS.readADC(EUCL_PIN));
  //    Serial.print("  ");
  //    Serial.print(ADS.readADC(ROTATION_PIN));
  //    Serial.print("  ");
  //    Serial.print(ADS.readADC(SCALE_PIN));
  //    Serial.print("  ");
  //    Serial.print(ADS.readADC(SHADOW_PIN));
  //    Serial.print("  ");
  //    Serial.print(analogRead(ROOT_PIN));
  //    Serial.print("  ");
  //    Serial.println(analogRead(GATEPROB_PIN));

  if (advance_sequencer) {
    bool flip_a_coin = TrueRandom.random(0, 101) <= PARAM_gate_probability;

    if (gate_events[sequencer_step] && flip_a_coin) {
      current_note = draw_note_from_markov_seed(current_note);
      current_semitone = semitone_vector_in_scale[current_note];
      print_u8_vector(semitone_vector_in_scale, PARAM_scale_width);
      //      Serial.print(current_note);
      //      Serial.print("  ");
      //      Serial.println(current_semitone);
      MCP.setValue(semitone_cvs_dac[current_semitone]);
      digitalWrite(GATE_PIN, HIGH);
    }


    display_step_indicator();
    update_sequencer_variables(); //must be after display
    advance_sequencer = false;
  }

  //////////// Save binned reading just after the switch change to detect
  //////////// the first time a pot moves after the switch changed.
  //////////// Also keep track of switch state and of if it changed twice quickly
  last_switch_state = switch_state;
  switch_state = digitalRead(SWITCH_PIN);
  if (switch_state != last_switch_state) {
    if (switch_state) {
      // Save the parameters the moment we switch to the other mode
      switched_PARAM_nb_euclidean_events = map(ADS.readADC(EUCL_PIN), 0, 25400, 0, 17);
      switched_PARAM_nb_shadow_steps = map(ADS.readADC(SHADOW_PIN), 0, 25400, 0, 17);;
      switched_PARAM_sequence_rotation =  map(ADS.readADC(ROTATION_PIN), 0, 25400, 0, 17);;
      switched_PARAM_gate_probability = map(analogRead(GATEPROB_PIN), 1023, -1, 0, 101);
      switched_PARAM_scale_idx = map(ADS.readADC(SCALE_PIN), 0, 25400, 0, NUM_SCALES);;
      switched_PARAM_root = map(analogRead(ROOT_PIN), 0, 1023 + 1, 0, 13);//57);
    } else {
      switched_PARAM_scale_width = map(ADS.readADC(SCALEWIDTH_PIN), 0, 25400, 1, 27);
      switched_PARAM_scale_dispersion = pow(map(ADS.readADC(DISPERSION_PIN), 0, 25400, 100, -1) / 57.0, 4);
      switched_PARAM_jump_to_first_neighbors = pow(map(ADS.readADC(FIRSTNEIGHBOR_PIN), 0, 25400, 0, 101) / 57.0, 4);
      switched_PARAM_jump_to_root = pow(map(analogRead(JUMPTOROOT_PIN), 0, 1023 + 1, 0, 101) / 57.0, 4);
      switched_PARAM_repeat_note = pow(map(ADS.readADC(REPEATNOTE_PIN), 0, 25400, 0, 101) / 57.0, 4);
      switched_PARAM_jump_to_second_neighbors = pow(map(analogRead(SECONDNEIGHBOR_PIN), 0, 1023 + 1, 100, -1) / 57.0, 4);
    }

    if (millis() - last_switch_change < 300) {
      fast_double_switch_change = true;
    } else {
      fast_double_switch_change = false;
    }
    last_switch_change = millis();

    lcd.setCursor(0, 0);
    lcd.print(EMPTY_LINE);
    lcd.setCursor(0, 1);
    lcd.print(EMPTY_LINE);
    GLOBAL_refresh_display = true;
  }
  ///////////////////////////////////////////////////////////////////////////////////////

  if (switch_state && last_switch_state && !fast_double_switch_change) {

    conditional_set_and_display_scale(ADS.readADC(SCALE_PIN));
    conditional_set_and_display_root(analogRead(ROOT_PIN));
    conditional_set_and_display_euclidean_sequence(ADS.readADC(EUCL_PIN), ADS.readADC(SHADOW_PIN), ADS.readADC(ROTATION_PIN));
    conditional_set_and_display_gate_probability(analogRead(GATEPROB_PIN));
    
  } else if (!switch_state && !last_switch_state && !fast_double_switch_change) {

    conditional_set_markovian_parameters(ADS.readADC(DISPERSION_PIN), ADS.readADC(REPEATNOTE_PIN), ADS.readADC(SCALEWIDTH_PIN), ADS.readADC(FIRSTNEIGHBOR_PIN), analogRead(JUMPTOROOT_PIN), analogRead(SECONDNEIGHBOR_PIN));

  } else if (fast_double_switch_change) {

    markov_seed = TrueRandom.random(0, 65536);
    randomSeed(markov_seed);
    fast_double_switch_change = false;

  }
  
  GLOBAL_refresh_display = false;

}



void display_step_indicator () {

  if (switch_state && last_switch_state) {
    lcd.setCursor(previous_sequencer_step, 1);

    if (gate_events[previous_sequencer_step]) {
      lcd.write(FILLED_LOWO);
    } else {
      lcd.write(EMPTY_CHAR);
    }

    lcd.setCursor(sequencer_step, 1);

    if (gate_events[sequencer_step]) {
      lcd.write(FILLED_HIGHO_UNDERBAR);
    } else {

      lcd.write(UNDERBAR);
    }
  }

}

void update_sequencer_variables() {
  //////////////////////////////////////
  // ADVANCE/REPOSITION SEQUENCE STEP //
  //////////////////////////////////////
  previous_sequencer_step = sequencer_step;

  sequencer_step = sequencer_step + sequencer_direction;

  if (sequencer_alternates) {
    if (sequencer_step < 0) {
      sequencer_step = 0;
      sequencer_direction = 1;
    } else if (sequencer_step > 15) {
      sequencer_step = 15;
      sequencer_direction = -1;
    }
  } else {
    if (sequencer_direction == 1 && sequencer_step > 15) {
      sequencer_step = 0;
    } else if (sequencer_direction == -1 && sequencer_step < 0) {
      sequencer_step = 15;
    }
  }
}


void conditional_set_markovian_parameters(u16 dispersion_pin_read, u16 repeatnote_pin_read, u16 scalewidth_pin_read, u16 firstneighbor_pin_read, u16 jumptoroot_pin_read, u16 secondneighbor_pin_read) {

  float binned_dispersion = pow(map(dispersion_pin_read, 0, 25400, 100, -1) / 57.0, 4);
  float binned_repeatnote = pow(map(repeatnote_pin_read, 0, 25400, 0, 101) / 57.0, 4);
  u8 binned_scalewidth = map(scalewidth_pin_read, 0, 25400, 1, 27); // Two chromatic octaves
  float binned_firstneighbor = pow(map(firstneighbor_pin_read, 0, 25400, 0, 101) / 57.0, 4);
  float binned_jumptoroot = pow(map(jumptoroot_pin_read, 0, 1023 + 1, 0, 51) / 28.5, 4);
  float binned_secondneighbor = pow(map(secondneighbor_pin_read, 0, 1023 + 1, 51, -1) / 28.5, 4);


  if ((binned_dispersion != PARAM_scale_dispersion && binned_dispersion != switched_PARAM_scale_dispersion) || startup) {
    PARAM_scale_dispersion = binned_dispersion;
    switched_PARAM_scale_dispersion = binned_dispersion;

    //    lcd.setCursor(0, 0);
    //    lcd.print(PARAM_scale_dispersion);

  }

  if ((binned_repeatnote != PARAM_repeat_note && binned_repeatnote != switched_PARAM_repeat_note) || startup) {
    PARAM_repeat_note = binned_repeatnote;
    switched_PARAM_repeat_note = binned_repeatnote;

    //    lcd.setCursor(5, 0);
    //    lcd.print(PARAM_repeat_note);
  }

  if ((binned_scalewidth != PARAM_scale_width && binned_scalewidth != switched_PARAM_scale_width) || startup) {
    PARAM_scale_width = binned_scalewidth;
    switched_PARAM_scale_width = binned_scalewidth;
    regenerate_semitone_vector_in_scale();
    //    lcd.setCursor(10, 0);
    //    lcd.print(PARAM_scale_width);
  }

  if ((binned_firstneighbor != PARAM_jump_to_first_neighbors && binned_firstneighbor != switched_PARAM_jump_to_first_neighbors) || startup) {
    PARAM_jump_to_first_neighbors = binned_firstneighbor;
    switched_PARAM_jump_to_first_neighbors = binned_firstneighbor;

    //    lcd.setCursor(0, 1);
    //    lcd.print(PARAM_jump_to_first_neighbors);

  }

  if ((binned_jumptoroot != PARAM_jump_to_root && binned_jumptoroot != switched_PARAM_jump_to_root) || startup) {
    PARAM_jump_to_root = binned_jumptoroot;
    switched_PARAM_jump_to_root = binned_jumptoroot;
    //    lcd.setCursor(5, 1);
    //    lcd.print(PARAM_jump_to_root);
  }

  if ((binned_secondneighbor != PARAM_jump_to_second_neighbors && binned_secondneighbor != PARAM_jump_to_second_neighbors) || startup) {
    PARAM_jump_to_second_neighbors = binned_secondneighbor;
    switched_PARAM_jump_to_second_neighbors = binned_secondneighbor;

    //    lcd.setCursor(10, 1);
    //    lcd.print(PARAM_jump_to_second_neighbors);
  }

}


void conditional_set_and_display_euclidean_sequence(u16 eucl_pin_read, u16 shadow_pin_read, u16 rotation_pin_read) {
  u8 binned_eucl = map(eucl_pin_read, 0, 25400, 0, 17);
  u8 binned_shadow = map(shadow_pin_read, 0, 25400, 0, 17);
  int8_t binned_rot = map(rotation_pin_read, 0, 25400, 0, 17);
  bool update_me = false;

  if ((binned_eucl != PARAM_nb_euclidean_events && binned_eucl != switched_PARAM_nb_euclidean_events) || startup) {
    PARAM_nb_euclidean_events = binned_eucl;
    switched_PARAM_nb_euclidean_events = binned_eucl;
    update_me = true;
    regenerate_gate_events(PARAM_nb_euclidean_events, PARAM_nb_shadow_steps, PARAM_sequence_rotation);

  }
  if ((binned_shadow != PARAM_nb_shadow_steps && binned_shadow != switched_PARAM_nb_shadow_steps) || startup) {
    PARAM_nb_shadow_steps = binned_shadow;
    switched_PARAM_nb_shadow_steps = binned_shadow;
    update_me = true;
    regenerate_gate_events(PARAM_nb_euclidean_events, PARAM_nb_shadow_steps, PARAM_sequence_rotation);

  }
  if ((binned_rot != PARAM_sequence_rotation && binned_rot != switched_PARAM_sequence_rotation) || startup) {
    PARAM_sequence_rotation = binned_rot;
    switched_PARAM_sequence_rotation = binned_rot;
    update_me = true;
    regenerate_gate_events(PARAM_nb_euclidean_events, PARAM_nb_shadow_steps, PARAM_sequence_rotation);

  }


  if (update_me || GLOBAL_refresh_display) {
    Serial.print("update rot/seq ");
    Serial.print(update_me);
    Serial.println(GLOBAL_refresh_display);

    lcd.setCursor(14, 0);
    lcd.print("  ");

    if (PARAM_sequence_rotation >= 0) {
      if (PARAM_sequence_rotation < 10) {
        lcd.setCursor(15, 0);
      } else {
        lcd.setCursor(14, 0);
      }
    }

    lcd.print(PARAM_sequence_rotation);

    lcd.setCursor(0, 1);
    for (u8 k = 0; k < 16; k++) {
      if (gate_events[k]) {
        lcd.write(FILLED_LOWO);
      } else {
        lcd.write(EMPTY_CHAR);
      }
    }
  }
}


void conditional_set_and_display_gate_probability(u16 gateprob_pin_reading) {
  u8 binned_gateprob = map(gateprob_pin_reading, 1023, -1, 0, 101);
  bool update_me = false;

  if ((binned_gateprob != PARAM_gate_probability && binned_gateprob != switched_PARAM_gate_probability) || startup) {

    PARAM_gate_probability = binned_gateprob;
    switched_PARAM_gate_probability = binned_gateprob;
    update_me = true;
  }

  if (update_me || GLOBAL_refresh_display) {
    Serial.print("update gateprob");
    Serial.print(update_me);
    Serial.println(GLOBAL_refresh_display);

    lcd.setCursor(11, 0);
    lcd.print("   ");
    if (PARAM_gate_probability >= 0 && PARAM_gate_probability <= 9) {
      lcd.setCursor(13, 0);
      lcd.print(PARAM_gate_probability);
    } else if (PARAM_gate_probability >= 10 && PARAM_gate_probability <= 99) {
      lcd.setCursor(12, 0);
      lcd.print(PARAM_gate_probability);
    } else if (PARAM_gate_probability == 100) {
      lcd.setCursor(11, 0);
      lcd.print("100");
    }
  }

}

void conditional_set_and_display_scale(u16 scale_pin_read) {

  u8 binned_read = map(scale_pin_read, 0, 25400, 0, NUM_SCALES);
  char scale_string[16];
  bool update_me = false;

  if ((binned_read != PARAM_scale_idx && binned_read != switched_PARAM_scale_idx) || startup) {
    PARAM_scale_idx = binned_read;
    PARAM_scale = (u16)pgm_read_word_near(scales + PARAM_scale_idx);
    switched_PARAM_scale_idx = binned_read;
    update_me = true;
    Serial.println("regenerating");
    regenerate_semitone_vector_in_scale();
    //    print_u8_vector(semitone_vector_in_scale, PARAM_scale_width);
  }

  if (update_me || GLOBAL_refresh_display) {
    Serial.println("update scale");
    Serial.print(update_me);
    Serial.println(GLOBAL_refresh_display);
    lcd.setCursor(0, 0);
    strcpy_P(scale_string, (char *)pgm_read_word(&(short_scale_names[PARAM_scale_idx])));
    lcd.print(scale_string);

  }

}

void conditional_set_and_display_root(u16 root_pin_read) {
  u8 binned_read = map(root_pin_read, 0, 1023 + 1, 0, 13);
  bool update_me = false;

  if ((binned_read != PARAM_root && binned_read != switched_PARAM_root) || startup) {
    PARAM_root = binned_read;
    switched_PARAM_root = binned_read;
    update_me = true;
    Serial.println("regenerating");
    regenerate_semitone_vector_in_scale();
  }

  if (update_me || GLOBAL_refresh_display) {
    Serial.print(update_me);
    Serial.println(GLOBAL_refresh_display);

    Serial.println("update root");
    lcd.setCursor(8, 0);
    lcd.print("   ");

    lcd.setCursor(8, 0);
    lcd.print(notes[PARAM_root % 12]);

    u8 octave = PARAM_root / 12 + 1;
    u8 note = PARAM_root % 12;

    if (note == 1 || note == 3 || note == 6 || note == 8 || note == 10) {
      lcd.setCursor(10, 0);
    } else {
      lcd.setCursor(9, 0);
    }
    lcd.print(octave);
  }
}


void regenerate_gate_events(u8 nb_euclidean_events, u8 nb_shadow_steps, int8_t rotation) {
  u16 event_location;

  for (u8 k = 0; k < 16; k++) {
    gate_events[k] = false;
  }

  for (u8 k = 0; k < nb_euclidean_events; k++) {
    event_location = (k * (16 + nb_shadow_steps)) / nb_euclidean_events;
    if (event_location >= 16) break;
    event_location = mod(event_location + rotation, 16);
    gate_events[event_location] = true;

  }
}


void regenerate_semitone_vector_in_scale() {
  u8 vector_idx = 0;
  u8 semitone = PARAM_root;
  u16 temp_scale = PARAM_scale;

  if (!startup) free(semitone_vector_in_scale);

  semitone_vector_in_scale = (u8 *)malloc(sizeof(u8[PARAM_scale_width]));

  while (vector_idx < PARAM_scale_width) {
    if (bitRead(temp_scale, 0)) {
      semitone_vector_in_scale[vector_idx] = semitone < 57 ? semitone : 56;
      vector_idx++;
    }
    temp_scale = rotate12Right(temp_scale, 1);
    semitone++;
  }
}

void initialize_sequencer_variables() {

  if (sequencer_alternates) {
    sequencer_direction = 1;
    sequencer_step = 0;
    previous_sequencer_step = 0;

  } else {

    if (sequencer_direction == 1) {
      sequencer_step = 0;
      previous_sequencer_step = 15;

    } else if (sequencer_direction == -1) {
      sequencer_step = 15;
      previous_sequencer_step = 0;
    }
  }

}

// Let's make the altorightm O(PARAM_scale_width) in space
// rather than generating a full markov matrix which
// is O(PARAM_scale_width^2) in space and very restrictive.
u8 draw_note_from_markov_seed(u8 initial_note) {
  ///////// Regenerate markov column from the seed /////////
  randomSeed(markov_seed);

  float cumul_threshold = random(1, 101) / 100.0;

  float *markov_column = (float *)malloc(sizeof(float[PARAM_scale_width]));

  for (u16 i = 0; i < initial_note * PARAM_scale_width; i++) {
    random(2);
  }

  for (u8 i = 0; i < PARAM_scale_width; i++) {
    markov_column[i] = random(1, 101) / (3.141592 * PARAM_scale_dispersion);
    markov_column[i] /= 1.0 + pow(abs(i - initial_note) / PARAM_scale_dispersion, 2);
    markov_column[i] *= abs(i - initial_note) % 2 ? PARAM_jump_to_first_neighbors : 1.0;
    markov_column[i] *= abs(i - initial_note) % 3 == 2 ? PARAM_jump_to_second_neighbors : 1.0;
  }

  markov_column[PARAM_root] *= PARAM_jump_to_root;
  markov_column[initial_note] *= PARAM_repeat_note;

  //  print_float_vector(markov_column, PARAM_scale_width);
  //  Serial.println();

  float column_total = 0.0;
  for (u8 i = 0; i < PARAM_scale_width; i++) {
    column_total += markov_column[i];
  }

  ////////////////// TOTALLY RAD VISUALIZATION ///////////////////
  if (millis() - last_markovian_display_refresh > 50 && !switch_state) {
    last_markovian_display_refresh = millis();

    u8 binned_element;
    int8_t n;

    for (u8 cur = 0; cur < 16; cur++) {

      n = initial_note - 8 + cur;

      if (n < 0 || n > PARAM_scale_width - 1) {
        lcd.setCursor(cur, 0);
        lcd.write(ZEROBARS);
        lcd.setCursor(cur, 1);
        lcd.write(ZEROBARS);
      } else {

        binned_element = constrain(int(markov_column[n] * PARAM_scale_width * (8 + 1) / column_total), 0, 8);

        switch (binned_element) {
          case 0:
            lcd.setCursor(cur, 0);
            lcd.write(ZEROBARS);
            lcd.setCursor(cur, 1);
            lcd.write(ZEROBARS);
            break;
          case 1:
            lcd.setCursor(cur, 0);
            lcd.write(ZEROBARS);
            lcd.setCursor(cur, 1);
            lcd.write(TWOBARS);
            break;
          case 2:
            lcd.setCursor(cur, 0);
            lcd.write(ZEROBARS);
            lcd.setCursor(cur, 1);
            lcd.write(FOURBARS);
            break;
          case 3:
            lcd.setCursor(cur, 0);
            lcd.write(ZEROBARS);
            lcd.setCursor(cur, 1);
            lcd.write(FOURBARS);
            break;
          case 4:
            lcd.setCursor(cur, 0);
            lcd.write(ZEROBARS);
            lcd.setCursor(cur, 1);
            lcd.write(EIGHTBARS);
            break;
          case 5:
            lcd.setCursor(cur, 0);
            lcd.write(TWOBARS);
            lcd.setCursor(cur, 1);
            lcd.write(EIGHTBARS);
            break;
          case 6:
            lcd.setCursor(cur, 0);
            lcd.write(FOURBARS);
            lcd.setCursor(cur, 1);
            lcd.write(EIGHTBARS);
            break;
          case 7:
            lcd.setCursor(cur, 0);
            lcd.write(SIXBARS);
            lcd.setCursor(cur, 1);
            lcd.write(EIGHTBARS);
            break;
          default:
            lcd.setCursor(cur, 0);
            lcd.write(EIGHTBARS);
            lcd.setCursor(cur, 1);
            lcd.write(EIGHTBARS);
            break;
        }
      }
    }
  }


  ////////////////////////////////////////////////////////////////


  float cumul_variate = TrueRandom.random(1, 101) / 100.0;
  float cumul = 0;

  for (u8 i = 0; i < PARAM_scale_width; i++) {
    cumul += markov_column[i] / column_total;
    if (cumul >= cumul_variate) {
      free(markov_column);
      return i;
    }
  }
  free(markov_column);
  return PARAM_scale_width - 1;
}


/////////// Interrupt functions ///////////
void clock_change() {
  if (digitalRead(CLOCKIN_PIN) == HIGH) {
    advance_sequencer = true;
  } else {
    digitalWrite(GATE_PIN, LOW);
  }
}

///////////////////////////////////////////


void print_float_array(float * float_array, uint8_t n, uint8_t m) {
  Serial.print('[');
  for (u8 i = 0; i < n; i++) {
    i > 0 ? Serial.print("  {") : Serial.print('[');
    for (u8 j = 0; j < m; j++) {
      Serial.print(float_array[i * n + j], 2);
      j < m - 1 ? Serial.print(", ") : Serial.print(']');
    }
    i < n - 1 ? Serial.println("") : Serial.print(']');
  }
  Serial.println();
}

void print_float_vector(float * vector, u8 n) {
  Serial.print('[');
  for (u8 i = 0; i < n; i++) {
    Serial.print(vector[i]);
    i <  n - 1 ? Serial.print(", ") : Serial.println(']');
  }
}

void print_u8_vector(u8 * vector, u8 n) {
  Serial.print('[');
  for (u8 i = 0; i < n; i++) {
    Serial.print(vector[i]);
    i <  n - 1 ? Serial.print(", ") : Serial.println(']');
  }
}


uint16_t rotate12Right(uint16_t n, uint16_t d) {
  return 0xfff & ((n >> (d % 12)) | (n << (12 - (d % 12))));
}

int mod(int x, int m) {
  return (x % m + m) % m;
}
