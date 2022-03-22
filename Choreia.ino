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

bool tuning_mode = false;

const uint8_t rs = 12, en = 11, d4 = 4, d5 = 5, d6 = 7, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

u16 markov_seed;

// This seed is used to regenerate the random markov transition matrix 
// from scratch at each note play
xorshift32_state markov_rng_state;
u32 markov_rng_seed;

// This is used to draw transitions between notes, i.e. generate the bass line
// Eventually we will use the seed to reset the sequence
// of notes at each bar instead of just letting the bass line
// evolve indefinitely
xorshift32_state draw_rng_state;
u32 draw_rng_seed;

// Note used anymore
// float *random_uniform_matrix;
// float *markov_matrix;

// Holds the indexes of the first, second, third, etc semitones in scale.
// So for chromatic its 0.1.2.3.4..., but e.g. for major it's 0.2.4.5.7.9.11...
// It's as long as PARAM_scale_width and is generated using the scale template in PARAM_scale
u8 *semitone_vector_in_scale;

u8 current_note = 0; // What it says. It's the semitone that's currently played on the DAC


bool startup = true; // indicate we are starting up and to initialize stuff.
bool GLOBAL_refresh_display = true; // Sometimes we wish to refresh all the things (including we we are starting up)


// Those parameters control the transition kernel
// They are modified by the pots/encoders when in the "melodic" mode (switch on the front panel ON)
u8 PARAM_scale_width = 4;
float PARAM_scale_dispersion = 3.0;          // Controls how far subsequently played notes/semitones are from each other on average
float PARAM_jump_to_first_neighbors = 1.33;  // Reweight how likely we are to jump to neighboring notes
float PARAM_jump_to_root = 0.33;             // Reweight how likely we are to jump back to the root note
float PARAM_repeat_note = 0.33;              // Reweight how likely we are to repeat the same note
float PARAM_jump_to_second_neighbors = 1.00; // Reweight how likely we are to jump to second neighboring notes
                                             // *** will eventually change that to jumping to the 5th

// We save parameters after a switch flip so that
// we can keep them where they were when we flip back
// *** This whole gimmick will become unnecessary once we switch to rotary encoders
u8 switched_PARAM_scale_width = 4;
float switched_PARAM_scale_dispersion = 3.0;
float switched_PARAM_jump_to_first_neighbors = 0.33;
float switched_PARAM_jump_to_root = 0.33;
float switched_PARAM_repeat_note = 0.33;
float switched_PARAM_jump_to_second_neighbors = 1.0;


// 
char rad_viz_line1[17] = EMPTY_LINE;
char rad_viz_line2[17] = EMPTY_LINE;


// There's no outside control for those yet
// so it's hardcoded for now.
bool sequencer_alternates = true;  // If true sequencer goes forward then backward
int8_t sequencer_direction = 1; // 1 is forward, -1 is backward
int8_t sequencer_step, previous_sequencer_step; // we need the previous sequencer step to correctly
                                                // update the sequencer display 
                                                // i.e. remove the underline from the previous step
                                                //      when we advance.

bool gate_events[16]; // 16 ZEROS and ONES, a transition from the previous note is drawn randomly and
                      // the resulting note is played when theres a ONE at gate_events[sequencer_step]
volatile bool advance_sequencer = false;  // volatile because it's modified during an interrupt
                                          // when CLOCKIN_PIN goes high it's set to TRUE and the main loop
                                          // does all its things (advance, transition, play note, update display)

// Those parameters control the beat
// They are accessible in the sequencer mode when the switch on the front panel is OFF
// ** I don't remember why I'm initializing them out of their range, weird
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

// Keeps the state of the switch on the front panel
// which controls the mode we are in (melodic/sequencer)
bool switch_state, last_switch_state;
long unsigned int last_switch_change;
bool fast_double_switch_change; // Did we quickly change the switch? If so generate a new markov transition matrix

long unsigned int tic;

void setup() {
  ////////////////////////

  analogReference(EXTERNAL);

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

  if (tuning_mode) {
    input_and_play_semitone();
  }

  //////////////////////

  markov_rng_seed = TrueRandom.random(0, 65536) * TrueRandom.random(0, 65536);
  Serial.println(markov_rng_seed);
  draw_rng_seed = TrueRandom.random(0, 65536) * TrueRandom.random(0, 65536);
  draw_rng_state = {draw_rng_seed};
  Serial.println(draw_rng_seed);

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

  startup = false;
  GLOBAL_refresh_display = false;
}


void loop() {

// Debug stuff
//  Serial.print(digitalRead(CLOCKIN_PIN));
//  Serial.print("  ");
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
//  Serial.println(analogRead(GATEPROB_PIN));

  if (advance_sequencer) {

    // Flip a biased coin to know if we play a note that's active in the euclidean sequence. 
    // Should be moved inside the if.
    bool flip_a_coin = (1.0 * xorshift32(&draw_rng_state)) / MAXU32 <= PARAM_gate_probability / 100.0;
    if (gate_events[sequencer_step] && flip_a_coin) {

      draw_and_play_note_from_markov_seed();
    }


    if (switch_state) {
      display_step_indicator();
    }
    update_sequencer_variables(); // advance sequencer one step. must be called after display.
    advance_sequencer = false;
  }

  //////////// Save binned reading just after the switch change to detect
  //////////// the first time a pot moves after the switch changed.
  //////////// This whole gimmick is ugly and we'll get ride of it with rotary encoders.
  //////////// Also keep track of switch state and of if it changed twice quickly
  //////////// in which case we reseed the markov transition kernel
  last_switch_state = switch_state;
  switch_state = digitalRead(SWITCH_PIN);
  if (switch_state != last_switch_state) {
    if (switch_state) {
      // Save the parameters the moment we switch to the other mode
      switched_PARAM_nb_euclidean_events = map(ADS.readADC(EUCL_PIN), 0, ADC_SLIGHTLY_ABOVE_5V, 0, 17);
      switched_PARAM_nb_shadow_steps = map(ADS.readADC(SHADOW_PIN), 0, ADC_SLIGHTLY_ABOVE_5V, 0, 17);;
      switched_PARAM_sequence_rotation =  map(ADS.readADC(ROTATION_PIN), 0, ADC_SLIGHTLY_ABOVE_5V, 0, 17);;
      switched_PARAM_gate_probability = map(analogRead(GATEPROB_PIN), 1023, -1, 0, 101);
      switched_PARAM_scale_idx = map(ADS.readADC(SCALE_PIN), 0, ADC_SLIGHTLY_ABOVE_5V, 0, NUM_SCALES);;
      switched_PARAM_root = map(analogRead(ROOT_PIN), 0, 1023 + 1, 0, 13);//57);
    } else {
      switched_PARAM_scale_width = map(ADS.readADC(SCALEWIDTH_PIN), 0, ADC_SLIGHTLY_ABOVE_5V, 1, 27);
      switched_PARAM_scale_dispersion = pow(map(ADS.readADC(DISPERSION_PIN), 0, ADC_SLIGHTLY_ABOVE_5V, 100, -1) / 57.0, 4);
      switched_PARAM_jump_to_first_neighbors = pow(map(ADS.readADC(FIRSTNEIGHBOR_PIN), 0, ADC_SLIGHTLY_ABOVE_5V, 0, 101) / 57.0, 4);
      switched_PARAM_jump_to_root = pow(map(analogRead(JUMPTOROOT_PIN), 0, 1023 + 1, 0, 101) / 57.0, 4);
      switched_PARAM_repeat_note = pow(map(ADS.readADC(REPEATNOTE_PIN), 0, ADC_SLIGHTLY_ABOVE_5V, 0, 101) / 57.0, 4);
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

  if (switch_state) {

    conditional_set_and_display_scale(ADS.readADC(SCALE_PIN));
    conditional_set_and_display_root(analogRead(ROOT_PIN));
    conditional_set_and_display_euclidean_sequence(ADS.readADC(EUCL_PIN), ADS.readADC(SHADOW_PIN), ADS.readADC(ROTATION_PIN));
    conditional_set_and_display_gate_probability(analogRead(GATEPROB_PIN));

  } else if (!switch_state) {

    conditional_set_markovian_parameters(ADS.readADC(DISPERSION_PIN), ADS.readADC(REPEATNOTE_PIN), ADS.readADC(SCALEWIDTH_PIN), ADS.readADC(FIRSTNEIGHBOR_PIN), analogRead(JUMPTOROOT_PIN), analogRead(SECONDNEIGHBOR_PIN));

  }

  if (fast_double_switch_change) {
    markov_rng_seed = TrueRandom.random(0, 65536) * TrueRandom.random(0, 65536);
    fast_double_switch_change = false;
  }

  GLOBAL_refresh_display = false;

}

// Move the little underbar from one step to the next
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

  /// This (x/57)^4 thingy is to scale the digital read so that we curve differently between 0 to 1 and between 1 to whatever
  /// This allow a little more precision between 0 and 1 compared to above 1.
  float binned_dispersion = pow(map(dispersion_pin_read, 0, ADC_SLIGHTLY_ABOVE_5V, 100, -1) / 57.0, 4);
  float binned_repeatnote = pow(map(repeatnote_pin_read, 0, ADC_SLIGHTLY_ABOVE_5V, 0, 101) / 57.0, 4);
  u8 binned_scalewidth = map(scalewidth_pin_read, 0, ADC_SLIGHTLY_ABOVE_5V, 1, 27); // Up to two chromatic octaves
  float binned_firstneighbor = pow(map(firstneighbor_pin_read, 0, ADC_SLIGHTLY_ABOVE_5V, 0, 101) / 57.0, 4);
  float binned_jumptoroot = pow(map(jumptoroot_pin_read, 0, 1023 + 1, 0, 51) / 28.5, 4);
  float binned_secondneighbor = pow(map(secondneighbor_pin_read, 0, 1023 + 1, 51, -1) / 28.5, 4);


  /// So much bullshit because of the pots and the modes. Please switch to rotary encoders asap.
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
  u8 binned_eucl = map(eucl_pin_read, 0, ADC_SLIGHTLY_ABOVE_5V, 0, 17);
  u8 binned_shadow = map(shadow_pin_read, 0, ADC_SLIGHTLY_ABOVE_5V, 0, 17);
  int8_t binned_rot = map(rotation_pin_read, 0, ADC_SLIGHTLY_ABOVE_5V, 0, 17);
  
  
  bool update_me = false; // Track whether there's been a change in any of the sequencer parameter
                          // in which case we refresh the display

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

  u8 binned_read = map(scale_pin_read, 0, ADC_SLIGHTLY_ABOVE_5V, 0, NUM_SCALES);
  char scale_string[16];
  bool update_me = false;

  if ((binned_read != PARAM_scale_idx && binned_read != switched_PARAM_scale_idx) || startup) {
    PARAM_scale_idx = binned_read;
    PARAM_scale = (u16)pgm_read_word_near(scales + PARAM_scale_idx); // Get the binary encoded scale in PROGMEM
    switched_PARAM_scale_idx = binned_read;
    update_me = true;
//    Serial.println("regenerating");
    regenerate_semitone_vector_in_scale();
    //    print_u8_vector(semitone_vector_in_scale, PARAM_scale_width);
  }


  if (update_me || GLOBAL_refresh_display) {

    lcd.setCursor(0, 0);
    strcpy_P(scale_string, (char *)pgm_read_word(&(short_scale_names[PARAM_scale_idx]))); // Get the scale name string in PROGMEM
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
    regenerate_semitone_vector_in_scale();
  }

  if (update_me || GLOBAL_refresh_display) {

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

    ///////////////////////
    /// Euclidean magic ///
    ///////////////////////

    // nb_shadow_steps adds some spice and break away from the 16 (plus empty) canonical euclidean sequences
    // I don't think anyone else does that.
    event_location = (k * (16 + nb_shadow_steps)) / nb_euclidean_events;
    if (event_location >= 16) break; // We only consider sequences with up to 16 (indexed 0 to 15) steps. 
                                     // We could increase that in the future
    event_location = mod(event_location + rotation, 16); // Rotate sequence to the right if necessary
    gate_events[event_location] = true;
    
    ///////////////////////

  }
}


// Generate vector of semitone indices given the binary encoded scale and the root note.
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

// Let's make the altorightm O(PARAM_scale_width) in memory
// rather than generating a full markov matrix which
// is O(PARAM_scale_width^2) and thus very restrictive.
void draw_and_play_note_from_markov_seed() {
  ///////// Regenerate markov column from the seed /////////

  float *markov_column = (float *)malloc(sizeof(float[PARAM_scale_width]));
    
  u8 initial_note = current_note;

  // That's the trick for not having to store the full markov
  // transition matrix with PARAM_scale_width^2 elements.
  // Start from the seed and reach the first element of the column
  // associated with the transition kernel from note/semitone initial_note to a new note/semitone.
  // This way we always generate the same sequence of random elements in the markov transition matrix
  markov_rng_state = {markov_rng_seed};
  for (u16 i = 0; i < initial_note * PARAM_scale_width; i++) {
    xorshift32(&markov_rng_state);
  }


  for (u8 i = 0; i < PARAM_scale_width; i++) {

    // Start with a uniform kernel, namely set all elements of the transition
    // kernel to a random number between 0 and 1 over the full scale width
    markov_column[i] = (1.0 * xorshift32(&markov_rng_state)) / MAXU32;

    // Reweight flat kernel using a Cauchy kernel centered on the current note 
    // https://en.wikipedia.org/wiki/Cauchy_distribution
    // It's faster than Gaussian which would require exponentiation and stuff
    // It's also more heavy-tailed, so gives more probability for more interesting jump to far notes
    markov_column[i] /= (3.141592 * PARAM_scale_dispersion);
    markov_column[i] /= 1.0 + pow(abs(i - initial_note) / PARAM_scale_dispersion, 2);

    // Reweight kernel at first neighbors
    markov_column[i] *= abs(i - initial_note) % 2 ? PARAM_jump_to_first_neighbors : 1.0;

    // Reweight kernel at second neighbor. Should be changed to the 5th in scale at some point.
    markov_column[i] *= abs(i - initial_note) % 3 == 2 ? PARAM_jump_to_second_neighbors : 1.0;
  }

  // Reweight kernel for the transition to the root or to the same note
  markov_column[PARAM_root] *= PARAM_jump_to_root;
  markov_column[initial_note] *= PARAM_repeat_note;

  // Renormalize column (i.e. the transition kernel) to 1
  // (we'll do that by dividing by column_total below
  float column_total = 0.0;
  float max_element = 0.0;
  for (u8 i = 0; i < PARAM_scale_width; i++) {
    max_element = max(max_element, markov_column[i]);
    column_total += markov_column[i];
  }

  // Randomly draw a jump from the transition kernel
  float cumul_variate = (1.0 * xorshift32(&draw_rng_state)) / MAXU32;
  float cumul = 0;

  for (u8 i = 0; i < PARAM_scale_width; i++) {
    cumul += markov_column[i] / column_total;
    if (cumul >= cumul_variate || i == PARAM_scale_width - 1) {
      current_note = i;
      break;
    }
  }

  // Play the new note to which we jumped to
  u8 semitone_to_play = semitone_vector_in_scale[current_note];
  MCP.setValue(semitone_cvs_dac[semitone_to_play]);
  digitalWrite(GATE_PIN, HIGH);

  free(markov_column);


  ////////////////// TOTALLY RAD VISUALIZATION! //////////////////
  ////////////////// (when in the melodic mode) //////////////////
  // We basically draw a very coarse representation of the transition kernel
  if (!switch_state) {

    u8 binned_element;
    int8_t n;

    for (u8 cur = 0; cur < 16; cur++) {

      n = initial_note - 8 + cur;

      if (n < 0 || n > PARAM_scale_width - 1) {
        //        lcd.setCursor(cur, 0);
        //        lcd.write(ZEROBARS);
        //        lcd.setCursor(cur, 1);
        //        lcd.write(ZEROBARS);
        rad_viz_line1[cur] = ZEROBARS;
        rad_viz_line2[cur] = ZEROBARS;

      } else {

        binned_element = constrain(int(markov_column[n] * (8 + 1) / max_element), 0, 8);

        switch (binned_element) {
          case 0:
            //            lcd.setCursor(cur, 0);
            //            lcd.write(ZEROBARS);
            //            lcd.setCursor(cur, 1);
            //            lcd.write(ZEROBARS);
            rad_viz_line1[cur] = ZEROBARS;
            rad_viz_line2[cur] = ZEROBARS;
            break;
          case 1:
            //            lcd.setCursor(cur, 0);
            //            lcd.write(ZEROBARS);
            //            lcd.setCursor(cur, 1);
            //            lcd.write(TWOBARS);
            rad_viz_line1[cur] = ZEROBARS;
            rad_viz_line2[cur] = TWOBARS;
            break;
          case 2:
            //            lcd.setCursor(cur, 0);
            //            lcd.write(ZEROBARS);
            //            lcd.setCursor(cur, 1);
            //            lcd.write(FOURBARS);
            rad_viz_line1[cur] = ZEROBARS;
            rad_viz_line2[cur] = FOURBARS;
            break;
          case 3:
            //            lcd.setCursor(cur, 0);
            //            lcd.write(ZEROBARS);
            //            lcd.setCursor(cur, 1);
            //            lcd.write(SIXBARS);
            rad_viz_line1[cur] = ZEROBARS;
            rad_viz_line2[cur] = SIXBARS;
            break;
          case 4:
            //            lcd.setCursor(cur, 0);
            //            lcd.write(ZEROBARS);
            //            lcd.setCursor(cur, 1);
            //            lcd.write(EIGHTBARS);
            rad_viz_line1[cur] = ZEROBARS;
            rad_viz_line2[cur] = EIGHTBARS;
            break;
          case 5:
            //            lcd.setCursor(cur, 0);
            //            lcd.write(TWOBARS);
            //            lcd.setCursor(cur, 1);
            //            lcd.write(EIGHTBARS);
            rad_viz_line1[cur] = TWOBARS;
            rad_viz_line2[cur] = EIGHTBARS;
            break;
          case 6:
            //            lcd.setCursor(cur, 0);
            //            lcd.write(FOURBARS);
            //            lcd.setCursor(cur, 1);
            //            lcd.write(EIGHTBARS);
            rad_viz_line1[cur] = FOURBARS;
            rad_viz_line2[cur] = EIGHTBARS;
            break;
          case 7:
            //            lcd.setCursor(cur, 0);
            //            lcd.write(SIXBARS);
            //            lcd.setCursor(cur, 1);
            //            lcd.write(EIGHTBARS);
            rad_viz_line1[cur] = SIXBARS;
            rad_viz_line2[cur] = EIGHTBARS;
            break;
          default:
            //            lcd.setCursor(cur, 0);
            //            lcd.write(EIGHTBARS);
            //            lcd.setCursor(cur, 1);
            //            lcd.write(EIGHTBARS);
            rad_viz_line1[cur] = EIGHTBARS;
            rad_viz_line2[cur] = EIGHTBARS;
            break;
        }
      }
    }
    lcd.setCursor(0, 0);
    lcd.print(rad_viz_line1);
    lcd.setCursor(0, 1);
    lcd.println(rad_viz_line2);
  }


  ////////////////////////////////////////////////////////////////
}


/////////// Interrupt functions ///////////
// Set advance_sequencer to signal the main loop to advance the sequencer
// Also the clock-in pin is used to control the duty-cycle
// so that as long as its high the ouput GATE pin stays high
// otherwise it goes low. The gate pin will be set high when
// the main loop plays the note.
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


// Shift the 12 least-significant bits of a uint16
// which is useful to find the semitones of a scale
// which we encoded in those bits.
uint16_t rotate12Right(uint16_t n, uint16_t d) {
  return 0xfff & ((n >> (d % 12)) | (n << (12 - (d % 12))));
}

int mod(int x, int m) {
  return (x % m + m) % m;
}


// Used to tune the ADC (find the digital reads at exactly 1V/oct=83.3mV/semitone from each others)
// and the DAC (this is done by ear using another tuned intrument or a tuner, find the digital values
// corresponding to each semitone.
void input_and_play_semitone() {
  static String inData;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tuning mode");

  Serial.println("");
  for (;;) {
    char received = ' '; // Each character received
    inData = ""; // Clear recieved buffer
    Serial.print("Semitone: ");

    while (received != '\n') { // When new line character is received (\n = LF, \r = CR)
      if (Serial.available() > 0) // When character in serial buffer read it
      {
        received = Serial.read();
        Serial.print(received); // Echo each received character, terminal dont need local echo
        inData += received; // Add received character to inData buffer
      }
    }
    inData.trim(); // Eliminate \n, \r, blank and other not “printable”
    Serial.println();
    MCP.setValue(inData.toInt());

  }

}


uint32_t xorshift32(struct xorshift32_state *state)
{
  /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
  uint32_t x = state->a;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  return state->a = x;
}
