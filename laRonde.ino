#include <avr/pgmspace.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <MCP4725.h>
#include <ADS1X15.h>

#include "scales.h"
#include "definitions.h"


MCP4725 MCP(0x60);
//ADS1115 ADS(0x48);

const uint8_t rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

bool led_state = false;

uint8_t eucl_gate_events[16];

int8_t eucl_sequence_step;
int8_t eucl_previous_sequence_step;
uint8_t current_nb_eucl_events = 0;
uint8_t current_eucl_shadow_steps = 0;
int8_t current_sequence_rotation = 0;
uint8_t current_scale = 0;


////////////////////////////////////////////////////////////////
// Those big glitchy pots suck so let's use a running average //
////////////////////////////////////////////////////////////////
#define RUNNING_AVERAGE_LENGTH 7
uint16_t circ_buffer_euclset_reads[RUNNING_AVERAGE_LENGTH];
uint16_t circ_buffer_shadow_reads[RUNNING_AVERAGE_LENGTH];
uint16_t circ_buffer_sequence_rotation_reads[RUNNING_AVERAGE_LENGTH];
uint16_t circ_buffer_scale_reads[RUNNING_AVERAGE_LENGTH];

uint8_t curr_euclset_buffer_idx = 0;
uint8_t curr_shadow_buffer_idx = 0;
uint8_t curr_sequence_rotation_buffer_idx = 0;
uint8_t curr_scale_buffer_idx = 0;
////////////////////////////////////////////////////////////////


/////////////////////////////////////////
// Global tunable sequencer parameters //
/////////////////////////////////////////
bool tuning_mode = false;

volatile bool tic_toc = false;
volatile bool eucl_advance = false;
volatile bool reset_gate1 = false;

//volatile long int interrupt_millis = 0;

long int scale = ABHOGI;

uint8_t root_note = 6;


// scale_width=16 is the maximum the 2KB dynamic memory
// of the Arduino nano will take considering
// TODO state space = notes in the scale, then use array states[state] = semitone_in_scale
uint8_t scale_width = 16;
uint8_t gate_probability = 100; // Deprecated. The Euclidean sequence is already good enough
//             as it is and more musical anyway.


// BPM to delay
uint8_t eucl_steps_per_bar = 16;

// The gate duty has been deprecated. Instead it follows the
// duty of clock-in.
// 7% seems like the shortest duty cycle the DAC will transmit
//float gate1_duty = 50;
//float gate2_duty = 50;

bool eucl_sequence_alternates = true;

int8_t eucl_sequence_direction = 1;

/////////////////////////////////////////
/////////////////////////////////////////
/////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:

  //  randomSeed(42);

  pinMode(SETEUCL_PIN, INPUT);
  pinMode(SETEUCLSHADOW_PIN, INPUT);
  pinMode(SETROTATION_PIN, INPUT);
  pinMode(SETSCALE_PIN, INPUT);

  pinMode(CLOCKIN_PIN, INPUT);
  pinMode(CVOUT_PIN, OUTPUT);
  pinMode(GATE_PIN, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(CLOCKIN_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(CLOCKIN_PIN), tictoc, CHANGE);

  Serial.flush();
  Serial.begin(115200);
  Serial.flush();

  Wire.begin();
  Wire.setClock(400000);

  MCP.begin();
  //  Serial.println(MCP.isConnected());

  //  ADS.begin();
  //  ADS.setGain(0);
  //  Serial.println(ADS.isConnected());

  //////////////////////////////////////////////
  /// Start LCD and Upload custom characters ///
  //////////////////////////////////////////////
  lcd.begin(16, 2);
  lcd.createChar(0, Filled_lowo);
  lcd.createChar(1, Filled_higho);
  lcd.createChar(2, Filled_lowo_overbar);
  lcd.createChar(3, Filled_higho_underbar);
  lcd.createChar(4, Underbar);
  lcd.createChar(5, Overbar);
  lcd.clear();

  if (tuning_mode) {
    input_and_play_semitone();
  }

  uint16_t circ_buffer_euclset_reads[RUNNING_AVERAGE_LENGTH];
  uint8_t curr_euclset_buffer_idx = 0;

  memset(circ_buffer_euclset_reads, analogRead(SETEUCL_PIN), RUNNING_AVERAGE_LENGTH);
  memset(circ_buffer_shadow_reads, analogRead(SETEUCLSHADOW_PIN), RUNNING_AVERAGE_LENGTH);
  memset(circ_buffer_sequence_rotation_reads, analogRead(SETROTATION_PIN), RUNNING_AVERAGE_LENGTH);
  memset(circ_buffer_scale_reads, analogRead(SETSCALE_PIN), RUNNING_AVERAGE_LENGTH);

  ////////////////////////////////////////////////
  /// Generate random Markov transition matrix ///
  ////////////////////////////////////////////////
  //
  // Parameters are
  //
  // uint16_t scale_mask            : Scale
  //
  // float scale_dispersion         : Cauchy distribution width, i.e. how big jumps
  //                                  from one note to another will be on average.
  //
  // float stay_on_note             : Reweight the probability of staying on a note.
  //
  // float step_on_first_neighbors, : Reweight the probability of stepping on notes
  //                                  that are first neighbors,
  //                                  i.e. from C to D or fro C to B (if major scale)
  //
  // float root_note_reweight       : Reweight the probability of playing the root note.
  //
  // uint8_t n, uint8_t m           : Dimensions of the markov matrix, usually 13.

  markov_matrix = generate_random_markov_matrix_from_scale
                  (scale,
                   2.0,   // scale_dispersion
                   0.33,  // stay_on_note
                   0.33,  // step_on_first_neighbors
                   0.33,   // root_note_reweight
                   scale_width, scale_width);


  //////////////////////////////////////////
  // Initialize sequencer state variables //
  //////////////////////////////////////////

  initialize_step_variables(eucl_sequence_alternates, eucl_sequence_direction, eucl_sequence_step, eucl_previous_sequence_step);

  copy_array_uint8(EMPTY_16EVENTS, eucl_gate_events, 16);

  read_and_set_euclidean_sequence(circular_running_average(analogRead(SETEUCL_PIN), circ_buffer_euclset_reads, curr_euclset_buffer_idx, RUNNING_AVERAGE_LENGTH),
                                  circular_running_average(analogRead(SETEUCLSHADOW_PIN), circ_buffer_shadow_reads, curr_shadow_buffer_idx, RUNNING_AVERAGE_LENGTH),
                                  circular_running_average(analogRead(SETROTATION_PIN), circ_buffer_sequence_rotation_reads, curr_sequence_rotation_buffer_idx, RUNNING_AVERAGE_LENGTH),
                                  circular_running_average(analogRead(SETSCALE_PIN), circ_buffer_scale_reads, curr_scale_buffer_idx, RUNNING_AVERAGE_LENGTH),
                                  current_nb_eucl_events,
                                  current_eucl_shadow_steps,
                                  current_sequence_rotation,
                                  current_scale,
                                  eucl_steps_per_bar, eucl_gate_events, FILLED_LOWO, 1);
}



///////////////////////////////////
///////////////////////////////////
///////////////////////////////////
///////////////////////////////////
///////////////////////////////////
///////////////////////////////////
///////////////////////////////////
///////////////////////////////////

void loop() {
  static bool eucl_first_sequence_step = true;

  /////////////////////////////////////
  // READ AND SET EUCLIDEAN SEQUENCE //
  /////////////////////////////////////

  read_and_set_euclidean_sequence(circular_running_average(analogRead(SETEUCL_PIN), circ_buffer_euclset_reads, curr_euclset_buffer_idx, RUNNING_AVERAGE_LENGTH),
                                  circular_running_average(analogRead(SETEUCLSHADOW_PIN), circ_buffer_shadow_reads, curr_shadow_buffer_idx, RUNNING_AVERAGE_LENGTH),
                                  circular_running_average(analogRead(SETROTATION_PIN), circ_buffer_sequence_rotation_reads, curr_sequence_rotation_buffer_idx, RUNNING_AVERAGE_LENGTH),
                                  circular_running_average(analogRead(SETSCALE_PIN), circ_buffer_scale_reads, curr_scale_buffer_idx, RUNNING_AVERAGE_LENGTH),
                                  current_nb_eucl_events,
                                  current_eucl_shadow_steps,
                                  current_sequence_rotation,
                                  current_scale,
                                  eucl_steps_per_bar, eucl_gate_events, FILLED_LOWO, 1);

  /////////////////
  // GATE EVENTS //
  /////////////////
  conditional_play_note(CVOUT_PIN, GATE_PIN,
                        eucl_advance,
                        eucl_first_sequence_step,
                        eucl_sequence_step,
                        eucl_previous_sequence_step,
                        eucl_sequence_alternates,
                        eucl_sequence_direction,
                        eucl_gate_events,
                        reset_gate1,
                        FILLED_LOWO,
                        FILLED_HIGHO_UNDERBAR,
                        UNDERBAR,
                        EMPTY_CHAR,
                        1);


  conditional_reset_gate(GATE_PIN, reset_gate1);
}


/////////////////////////////

void conditional_play_note(
  uint8_t cv_pin, uint8_t gate_pin,
  //  uint8_t *semitone_cvs,
  volatile bool &advance,
  bool &first_sequence_step,
  int8_t &eucl_sequence_step,
  int8_t &eucl_previous_sequence_step,
  bool eucl_sequence_alternates,
  int8_t &eucl_sequence_direction,
  uint8_t *eucl_gate_events,
  volatile bool &reset_gate,
  char step_char,
  char bar_step_char,
  char bar_char,
  char empty_char,
  uint8_t column) {

  bool gate_condition;
  uint8_t current_semitone;

  if (advance) {
    first_sequence_step = false;

    advance = false;

    gate_condition = (random(0, 101) <= gate_probability);

    if (gate_condition && eucl_gate_events[eucl_sequence_step % 16]) {

      /////////////////////
      // PITCH CV CHANGE //
      /////////////////////
      uint8_t semitone_cv;

      current_semitone = draw_semitone_from_markov_matrix(markov_matrix, current_semitone, scale_width);


      MCP.setValue(semitone_cvs_dac[current_semitone]);
      //      Serial.println(semitone_cvs_dac[current_semitone]);
      //      Serial.println(MCP.getValue());
      //      analogWrite(cv_pin, semitone_cvs[current_semitone]);
      digitalWrite(gate_pin, HIGH);

      //      current_midi_note = midi_root_note + current_semitone;
      //      midiWrite(0x90 | midi_channel, current_midi_note, midi_note_velocity);

      digitalWrite(LED_BUILTIN, HIGH);

    }


    //////////////////////////
    // ON/OFF STEPS DISPLAY //
    //////////////////////////

    on_off_step_display(eucl_sequence_step, eucl_previous_sequence_step, eucl_gate_events,
                        step_char, bar_step_char, bar_char, empty_char, column);


    //////////////////////////////////////
    // ADVANCE/REPOSITION SEQUENCE STEP //
    //////////////////////////////////////
    eucl_previous_sequence_step = eucl_sequence_step;
    eucl_sequence_step = eucl_sequence_step + eucl_sequence_direction;

    if (eucl_sequence_alternates) {

      if (eucl_sequence_step < 0) {
        eucl_sequence_step = 0;
        eucl_sequence_direction = 1;
      } else if (eucl_sequence_step > 15) {
        eucl_sequence_step = 15;
        eucl_sequence_direction = -1;
      }

    } else {

      if (eucl_sequence_direction == 1 && eucl_sequence_step > 15) {
        eucl_sequence_step = 0;
      } else if (eucl_sequence_direction == -1 && eucl_sequence_step < 0) {
        eucl_sequence_step = 15;
      }

    }

  }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
void on_off_step_display(
  int eucl_sequence_step,
  int eucl_previous_sequence_step,
  uint8_t *gate_events,
  char step_char,
  char bar_step_char,
  char bar_char,
  char empty_char,
  uint8_t column)
{

  lcd.setCursor(eucl_previous_sequence_step, column);
  if (gate_events[eucl_previous_sequence_step]) {
    lcd.write(step_char);
  } else {
    lcd.write(empty_char);
  }


  lcd.setCursor(eucl_sequence_step, column);
  if (gate_events[eucl_sequence_step]) {
    lcd.write(bar_step_char);
  } else {
    lcd.write(bar_char);
  }
}


///////////////////////////////////////////////////////////////////////////////////////
void conditional_reset_gate(uint8_t gate_pin, volatile bool &reset_gate) {
  if (reset_gate) {
    reset_gate = false;
    digitalWrite(gate_pin, LOW);
    //    midiWrite(0x80 | midi_channel, current_midi_note, 0);
    digitalWrite(LED_BUILTIN, LOW);
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
void read_and_set_euclidean_sequence(uint16_t new_euclset_read, uint16_t new_shadow_read, uint16_t new_sequence_rotation_read, uint16_t new_scale_read, uint8_t &nb_eucl_events, uint8_t &eucl_shadow_steps, int8_t &sequence_rotation, uint8_t &scale, uint8_t steps_per_bar, uint8_t *gate_events, char step_char, uint8_t column) {
  uint8_t new_nbevents, new_shadow_steps;
  int8_t new_sequence_rotation;
  uint8_t new_scale;
  char scale_string[16];
  
  uint8_t event_location;

  uint16_t in_max = 1023, out_max = 16;

  

  new_nbevents = map(new_euclset_read, 0, in_max + 1 , 0, out_max + 1);

  new_shadow_steps = map(new_shadow_read, 0, in_max + 1, 0, out_max + 1);

  new_sequence_rotation = map(new_sequence_rotation_read, 0, in_max + 1, -16, 16 + 1);

  new_scale = map(new_scale_read, 0, in_max + 1, 0, NUM_SCALES);

  if ((new_nbevents != nb_eucl_events) ||
      (new_shadow_steps != eucl_shadow_steps) ||
      (new_sequence_rotation != sequence_rotation) ||
      (new_scale != scale)) {

    copy_array_uint8(EMPTY_16EVENTS, gate_events, 16);

    nb_eucl_events = new_nbevents;
    eucl_shadow_steps = new_shadow_steps;
    sequence_rotation = new_sequence_rotation;
    scale = new_scale;

    lcd.setCursor(0, 0);
    lcd.print(EMPTY_LINE);

    lcd.setCursor(0, 0);
    strcpy_P(scale_string, (char *)pgm_read_word(&(short_scale_names[scale])));
    lcd.print(scale_string);

    lcd.setCursor(9, 0);
    lcd.print(notes[root_note]);

    lcd.setCursor(12, 0);
    lcd.print("r");
    lcd.print(sequence_rotation);

    lcd.setCursor(0, column);
    lcd.print(EMPTY_LINE);


    for (uint8_t k = 0; k < nb_eucl_events; k++) {
      event_location = int(k * (16 + eucl_shadow_steps) * 1.0 / nb_eucl_events);

      if (event_location < 16) {
        event_location = mod(event_location + sequence_rotation, 16);

        gate_events[event_location] = HIGH;

        lcd.setCursor(event_location, column);
        lcd.write(step_char);
      }
    }


  }
}


/////////////////////////////////
uint16_t circular_running_average(uint16_t new_read, uint16_t *circ_buffer, uint8_t &curr_buffer_idx, uint8_t buffer_length) {
  //  Serial.println(new_read);
  //  delay(1000);
  uint16_t total = 0;
  uint16_t average;
  curr_buffer_idx = (++curr_buffer_idx) % buffer_length;
  circ_buffer[curr_buffer_idx] = new_read;

  for (uint8_t k = 0; k < buffer_length; k++) {
    total += circ_buffer[k];
  }

  average = (uint16_t)(int(total / buffer_length));

  return average;
}


//////////////////////////////
void initialize_step_variables(bool eucl_sequence_alternates, int8_t &eucl_sequence_direction, int8_t &eucl_sequence_step, int8_t &eucl_previous_sequence_step) {

  if (eucl_sequence_alternates) {
    eucl_sequence_direction = 1;
    eucl_sequence_step = 0;
    eucl_previous_sequence_step = 0;

  } else if (eucl_sequence_direction == 1) {
    eucl_sequence_step = 0;
    eucl_previous_sequence_step = 15;

  } else if (eucl_sequence_direction == -1) {
    eucl_sequence_step = 15;
    eucl_previous_sequence_step = 0;
  }

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t random_semitone_from_scale(uint16_t scale_mask) {
  uint8_t semitone;
  bool semitone_bit;

  semitone_bit = 0;
  do {
    semitone = random(0, scale_width);
  } while (bitRead(scale_mask, semitone) == 0);

  return semitone;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
void print_uint8_array(uint8_t *uint8_array, uint8_t len) {
  Serial.print('.');
  for (int j = 0; j < len; j++) {
    Serial.print(uint8_array[j]);
  }
  Serial.println("..");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
void print_float_array(float * float_array, uint8_t n, uint8_t m) {
  Serial.print(".{");
  for (int i = 0; i < n; i++) {
    i > 0 ? Serial.print("  {") : Serial.print('{');
    for (int j = 0; j < m; j++) {
      Serial.print(float_array[i * n + j], 2);
      j < m - 1 ? Serial.print(", ") : Serial.print('}');
    }
    i < n - 1 ? Serial.println("") : Serial.print("}");
  }
  Serial.println("..");
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t numeral_to_semitone_from_scale(uint8_t numeral, uint16_t scale_mask) {
  uint8_t semitone = 0;
  uint8_t nb_off = 0, nb_on = 0;
  do {
    if (bitRead(scale_mask, semitone % 12) == 1) {
      nb_on += 1;
    } else {
      nb_off += 1;
    };
  } while (nb_on < numeral);
  return numeral;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
float *generate_random_markov_matrix_from_scale
(uint16_t scale_mask,
 float scale_dispersion,
 float stay_on_note,
 float step_on_first_neighbors,
 float root_note_reweight,
 uint8_t n, uint8_t m) {

  float *random_matrix = (float *)malloc(sizeof(int[n][m]));

  // We fill the matrix column by column rather than row by row
  for (int j = 0; j < m; j++) {
    for (int i = 0; i < n; i++) {
      random_matrix[i * n + j] = bitRead(scale_mask, i) && bitRead(scale_mask, j) ? 1.0 * random(1, 101) : 0.0;
      random_matrix[i * n + j] /= 3.141592 * scale_dispersion * (1.0 + pow(distance_on_scale(i, j, scale_mask) / scale_dispersion, 2));
      random_matrix[i * n + j] *= distance_on_scale(i, j, scale_mask) % 2 ? step_on_first_neighbors : 1.0;
      if (i % 13 == 0) {
        random_matrix[j] *= root_note_reweight;
      }
    }
    random_matrix[j * n + j] *= stay_on_note;
  }

  normalize_columns(random_matrix, n, m);

  return random_matrix;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t distance_on_scale(int8_t semitone_low, int8_t semitone_high, uint16_t scale_mask) {
  uint8_t s0;
  uint8_t dist = 0;

  if (semitone_low == semitone_high) {
    return 0;
  } else if (semitone_low > semitone_high) {
    s0 = semitone_low;
    semitone_low = semitone_high;
    semitone_high = s0;
  }

  uint16_t rotated_scale =  rotr(scale_mask, semitone_low);

  for (int i = 0; i < semitone_high - semitone_low; i++) {
    dist += bitRead(rotated_scale, 0);
    rotated_scale = rotr(rotated_scale, 1);
  }

  return dist - 1;

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
void normalize_columns(float * unnormalized_matrix, uint8_t n, uint8_t m) {
  float column_total;

  for (int j = 0; j < m; j++) {
    column_total = 0.0;
    for (int i = 0; i < n; i++) {
      column_total += 1.0 * unnormalized_matrix[i * n + j];
    }

    for (int i = 0; i < m; i++) {
      unnormalized_matrix[i * n + j] /= column_total > 0.0 ? column_total : 1.0;
    }
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t draw_semitone_from_markov_matrix(float * markov_matrix, uint8_t initial_semitone, uint8_t n) {

  float cumul_threshold = random(1, 101) / 100.0;

  //  Serial.println("");
  //  Serial.print(cumul_threshold);
  //  Serial.print(" | ");


  float cumul = 0.0;
  for (int i = 0; i < n; i++) {
    if (cumul_threshold <= cumul) {
      return i - 1 ;
    }
    cumul += markov_matrix[i * n + initial_semitone];
    //    Serial.print(markov_matrix[i * n + initial_semitone]);
    //    Serial.print("++ ");
    //    Serial.print(cumul);
    //    Serial.print(" ");
  }

  for (int i = n - 1; i >= 0; i--) {
    if (markov_matrix[i * n + initial_semitone] > 0.0) {
      return i;
    }
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void input_and_play_semitone() {
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

//void button_click() {
//  button_activated = !button_activated;
//}


///////////////
//void setupPWM16() {
//  DDRB |= _BV(PB1) | _BV(PB2);        /* set pins as outputs */
//  TCCR1A = _BV(COM1A1) | _BV(COM1B1)  /* non-inverting PWM */
//           | _BV(WGM11);                   /* mode 14: fast PWM, TOP=ICR1 */
//  TCCR1B = _BV(WGM13) | _BV(WGM12)
//           | _BV(CS11);                    /* prescaler: clock / 8 */
//  ICR1 = 0xffff;                      /* TOP counter value (freeing OCR1A*/
//}
//
//void analogWrite16(uint8_t pin, uint16_t val)
//{
//  switch (pin) {
//    case  9: OCR1A = val; break;
//    case 10: OCR1B = val; break;
//  }
//}


/////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t rotl(uint16_t n, uint16_t b)
{
  return (n << b) | (n >> (16 - b));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t rotr(uint16_t n, uint16_t b)
{
  return (n >> b) | (n << (16 - b));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void copy_array_uint8(uint8_t* src, uint8_t* dst, uint16_t len) {
  for (uint16_t i = 0; i < len; i++) {
    *dst++ = *src++;
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
void copy_array_bool(bool * src, bool * dst, uint16_t len) {
  for (uint16_t i = 0; i < len; i++) {
    *dst++ = *src++;
  }
}

////////////////////////////////////////

int mod(int x, int m) {
  return (x % m + m) % m;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
void tictoc() {

  // Assume we begin in the toc state (tic_toc == false)
  tic_toc = !tic_toc;

  if (tic_toc) {
    eucl_advance = true;

    reset_gate1 = false;

  } else {
    reset_gate1 = true;
  }

}
