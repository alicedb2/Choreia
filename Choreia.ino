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

// Set to values outside the range
// to force refresh in setup()
u8 PARAM_scale_idx = 127;
u8 PARAM_root = 127;
u16 PARAM_scale = CHROMATIC;
u16 scale_and_chord_mask;

u8 PARAM_scale_width = 6;

float PARAM_scale_dispersion = 2.0;
float PARAM_step_on_first_neighbors = 1.0;
float PARAM_root_weight = 0.66;
float PARAM_stay_on_note = 0.33;

u8 current_note = 0;
u8 current_semitone = 0;

bool sequencer_alternates = true;
int8_t sequencer_direction = 1;
int8_t sequencer_step, previous_sequencer_step;

bool gate_events[16];
volatile bool advance_sequencer = false;

u8 PARAM_nb_euclidean_events = 17;
u8 PARAM_nb_shadow_steps = 17;
int8_t PARAM_sequence_rotation = 17;
u8 PARAM_gate_probability = 101;

void setup() {
  ////////////////////////
  pinMode(GATE_PIN, OUTPUT);
  pinMode(CLOCKIN_PIN, INPUT);

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
  lcd.clear();


  //////////////////////
  markov_seed = TrueRandom.random(0, 65536);
  randomSeed(markov_seed);


  initialize_sequencer_variables();
  conditional_set_and_display_scale(ADS.readADC(SCALE_PIN));
  conditional_set_and_display_root(analogRead(ROOT_PIN));
  conditional_set_and_display_euclidean_sequence(ADS.readADC(EUCL_PIN), ADS.readADC(SHADOW_PIN), ADS.readADC(ROTATION_PIN));
  conditional_set_and_display_gate_probability(analogRead(GATEPROB_PIN));

}


void loop() {
  //    lcd.setCursor(0,0);
  //    lcd.print(digitalRead(CLOCKIN_PIN));
  //  Serial.print(ADS.readADC(EUCL_PIN));
  //  Serial.print("  ");
  //  Serial.print(ADS.readADC(ROTATION_PIN));
  //  Serial.print("  ");
  //  Serial.print(ADS.readADC(SCALE_PIN));
  //  Serial.print("  ");
  //  Serial.print(ADS.readADC(SHADOW_PIN));
  //  Serial.print("  ");
  //  Serial.print(analogRead(ROOT_PIN));
  //  Serial.print("  ");
  //  Serial.println(analogRead(CHORD_PIN));

  if (advance_sequencer) {

    advance_sequencer = false;

    if (gate_events[sequencer_step] && TrueRandom.random(0, 101) <= PARAM_gate_probability) {
      current_note = draw_note_from_markov_seed(current_note);
      current_semitone = semitone_vector_in_scale[current_note];
      MCP.setValue(semitone_cvs_dac[current_semitone]);
      digitalWrite(GATE_PIN, HIGH);
    }

    display_and_update_sequencer_variables();
  }

  conditional_set_and_display_scale(ADS.readADC(SCALE_PIN));
  conditional_set_and_display_root(analogRead(ROOT_PIN));
  conditional_set_and_display_euclidean_sequence(ADS.readADC(EUCL_PIN), ADS.readADC(SHADOW_PIN), ADS.readADC(ROTATION_PIN));
  conditional_set_and_display_gate_probability(analogRead(GATEPROB_PIN));

}

void display_and_update_sequencer_variables() {

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


void conditional_set_and_display_euclidean_sequence(u16 eucl_pin_read, u16 shadow_pin_read, u16 rotation_pin_read) {
  u8 binned_eucl = map(eucl_pin_read, 0, 25400, 0, 17);
  u8 binned_shadow = map(shadow_pin_read, 0, 25400, 0, 17);
  int8_t binned_rot = map(rotation_pin_read, 0, 25400, 0, 17);

  if ((binned_eucl != PARAM_nb_euclidean_events) || (binned_shadow != PARAM_nb_shadow_steps) || (binned_rot != PARAM_sequence_rotation)) {

    PARAM_nb_euclidean_events = binned_eucl;
    PARAM_nb_shadow_steps = binned_shadow;
    PARAM_sequence_rotation = binned_rot;

    regenerate_gate_events(PARAM_nb_euclidean_events, PARAM_nb_shadow_steps, PARAM_sequence_rotation);

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
  } // End binned != PARAM
}


void conditional_set_and_display_gate_probability(u16 gateprob_pin_reading) {
  u8 binned_gateprob = map(gateprob_pin_reading, 1023, -1, 0, 101);

  if (binned_gateprob != PARAM_gate_probability) {
    PARAM_gate_probability = binned_gateprob;

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

  if (binned_read != PARAM_scale_idx) {
    PARAM_scale_idx = binned_read;
    PARAM_scale = scales[PARAM_scale_idx];

    free(semitone_vector_in_scale);
    regenerate_semitone_vector_in_scale(PARAM_scale, PARAM_root);
    //    print_u8_vector(semitone_vector_in_scale, PARAM_scale_width);

    lcd.setCursor(0, 0);
    strcpy_P(scale_string, (char *)pgm_read_word(&(short_scale_names[PARAM_scale_idx])));
    lcd.print(scale_string);

  }

}

void conditional_set_and_display_root(u16 root_pin_read) {
  u8 binned_read = map(root_pin_read, 0, 1023 + 1, 0, 57);

  if (binned_read != PARAM_root) {
    PARAM_root = binned_read;

    free(semitone_vector_in_scale);
    regenerate_semitone_vector_in_scale(PARAM_scale, PARAM_root);

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


void regenerate_semitone_vector_in_scale(u16 scale, u8 root) {
  u8 vector_idx = 0;
  u8 semitone = root;

  semitone_vector_in_scale = (u8 *)malloc(sizeof(u8[PARAM_scale_width]));

  while (vector_idx < PARAM_scale_width) {
    if (bitRead(scale, 0)) {
      semitone_vector_in_scale[vector_idx] = semitone < 57 ? semitone : 56;
      vector_idx++;
    }
    scale = rotate12Right(scale, 1);
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
    markov_column[i] *= abs(i - initial_note) % 2 ? PARAM_step_on_first_neighbors : 1.0;
  }

  markov_column[PARAM_root] *= PARAM_root_weight;
  markov_column[initial_note] *= PARAM_stay_on_note;

  //  print_float_vector(markov_column, PARAM_scale_width);
  //  Serial.println();

  float column_total = 0.0;
  for (u8 i = 0; i < PARAM_scale_width; i++) {
    column_total += markov_column[i];
  }
  //////////////////////////////////////////////////////////

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
