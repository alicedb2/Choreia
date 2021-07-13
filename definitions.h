#define u8 uint8_t
#define u16 uint16_t

#define EMPTY_LINE "                "
#define EMPTY_CHAR ' '


/// On the Arduino
#define GATE_PIN 6
//#define CVOUT_PIN // Now on the DAC
#define CLOCKIN_PIN 3

#define SWITCH_PIN 9
#define ROOT_PIN A1
#define CHORD_PIN A2

// On the ADC
#define SCALE_PIN 1
#define SHADOW_PIN 2
#define ROTATION_PIN 0
#define EUCL_PIN 3

const uint16_t semitone_cvs_dac[57] = {
  6, // C2
  79,
  151,
  224,
  297,
  368,
  441,
  509,
  584,
  653,
  727,
  799,
  870, // C3
  944,
  1014,
  1087,
  1157,
  1232,
  1302,
  1374,
  1446,
  1516,
  1588,
  1658,
  1733, // C4
  1802,
  1876,
  1946,
  2018,
  2091,
  2162,
  2237,
  2306,
  2381,
  2452,
  2525,
  2595, // C5
  2667,
  2741,
  2810,
  2884,
  2954,
  3027,
  3096,
  3168,
  3240,
  3311,
  3385,
  3453, // C6
  3528,
  3596,
  3669,
  3739,
  3812,
  3884,
  3954,
  4030  // A6
};


byte Filled_lowo[8] =
{
  0b00000, 0b00000, 0b01110, 0b11111, 0b11111, 0b11111, 0b01110, 0b00000
};

byte Filled_higho_underbar[8] =
{
  0b00000, 0b01110, 0b11111, 0b11111, 0b11111, 0b01110, 0b00000, 0b11111
};

 byte Underbar[8] =
{
  0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111
};

const byte Two_bars[8] =
{
  0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111
};

const byte Four_bars[8] =
{
  0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111
};

const byte Six_bars[8] =
{
  0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111
};


#define FILLED_LOWO char(0)
#define FILLED_HIGHO_UNDERBAR char(1)
#define UNDERBAR char(2)



const char key_C[3] = "C ";
const char key_Csharp[3] = "C#";
const char key_D[3] = "D ";
const char key_Dsharp[3] = "D#";
const char key_E[3] = "E ";
const char key_F[3] = "F ";
const char key_Fsharp[3] = "F#";
const char key_G[3] = "G ";
const char key_Gsharp[3] = "G#";
const char key_A[3] = "A ";
const char key_Asharp[3] = "A#";
const char key_B[3] = "B ";

const char *const notes[12] = {
  key_C, key_Csharp, key_D, key_Dsharp, key_E, key_F,
  key_Fsharp, key_G, key_Gsharp, key_A, key_Asharp, key_B
};
