#define SETEUCL_PIN A0
#define SETEUCLSHADOW_PIN A6
#define SETROTATION_PIN A1
#define SETSCALE_PIN A2

#define BUTTON_PIN 3
#define CLOCKIN_PIN 2
#define CVOUT_PIN 10
#define GATE_PIN A3

#define EMPTY_CHAR ' '
#define EMPTY_LINE "                "

#define EUCL_COLUMN 1

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


//// Tuning sucks.
//// Use a voltmeter and measure voltage
//// between pole of the capacitor of the
//// low pass filter and fiddle with the
//// x value passed to analogWrite(CVOUT1_PIN, x)
//// to get as close to 1/12 mV per semitone.
//// Triple check your x's.
//uint8_t semitone_cvs_1[] = {
//  0,  // 0.00 mV C2
//  6,  // 83 mV
//  12, // 167 mV
//  18, // 250 mV
//  24, // 333 mV
//  30, // 417 mV
//  36, // 500 mV
//  42, // 583 mV
//  49, // 667 mV
//  56, // 750 mV
//  62, // 833 mV
//  68, // 917 0 mV
//  73, // 1000 mV C3
//  78, // 1083 mV
//  83, // 1167 mV
//  88 // 1250 mV
//};
//
//uint8_t semitone_cvs_2[] {
//  0,  // 0.00 mV C0
//  5,  // 83 mV
//  11, // 167 mV
//  17, // 250 mV
//  22, // 333 mV
//  28, // 417 mV
//  34, // 500 mV
//  40, // 583 mV
//  47, // 667 mV
//  53, // 750 mV
//  59, // 833 mV
//  65, // 917 0 mV
//  71, // 1000 mV C1
//  76,
//  81,
//  85
//};

byte Filled_lowo[8] =
{
  0b00000, 0b00000, 0b01110, 0b11111, 0b11111, 0b11111, 0b01110, 0b00000
};

byte Filled_higho[8] =
{
  0b00000, 0b01110, 0b11111, 0b11111, 0b11111, 0b01110, 0b00000, 0b00000
};


byte Filled_lowo_overbar[8] =
{
  0b11111, 0b00000, 0b01110, 0b11111, 0b11111, 0b11111, 0b01110, 0b00000
};

byte Filled_higho_underbar[8] =
{
  0b00000, 0b01110, 0b11111, 0b11111, 0b11111, 0b01110, 0b00000, 0b11111
};


byte Underbar[8] =
{
  0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111
};

byte Overbar[8] =
{
  0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000
};


#define FILLED_LOWO char(0)
#define FILLED_HIGHO char(1)
#define FILLED_LOWO_OVERBAR char(2)
#define FILLED_HIGHO_UNDERBAR char(3)
#define UNDERBAR char(4)
#define OVERBAR char(5)

/////////////

String inData;

////////////
//#define BUTTON_INTERRUPT_PIN 3
//volatile byte button_activated = true;


uint8_t EMPTY_16EVENTS[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

float *markov_matrix;


const uint16_t scales[] = {
  CHROMATIC,  // 0
  MAJOR,  // 1
  MINOR,  // 2
  HARMONIC_MINOR, // 3
  MAJOR_PENTATONIC, // 4
  MINOR_PENTATONIC, // 5
  BLUES,  // 6
  JAPANESE, // 7
  FREYGISH, // 8
  ROMA, // 9
  SCOTTISH, // 10
  ARABIC, // 11
  ALTERED,  // 12
  WHOLE_TONE, // 13
  HW_DIMINISHED,  // 14
  WH_DIMINISHED,  // 15
  IONIAN, // 16
  DORIAN, // 17
  PHRYGIAN, // 18
  LYDIAN, // 19
  MIXOLYDIAN, // 20
  AEOLIAN,  // 21
  LOCRIAN,  // 22
  IONIAN_B2,  // 23
  DORIAN_B5,  // 24
  HARMONIC_PHRYGIAN,  // 25
  PHRYGIAN_MAJOR, // 26
  LYDIAN_B3,  // 27
  MAJOR_LOCRIAN,  // 28
  MINOR_LOCRIAN,  // 29
  SUPER_LOCRIAN,  // 30
  LYDIAN_7B,  // 31
  DIMINISHED, // 32
  MIXOLYDIAN_B13, // 33
  MIXOLYDIAN_B9_B13,  // 34
  LYDIAN_7B_B2, // 35
  BEBOP,  // 36
  BLUES_MAJOR,  // 37
  BLUES_MINOR,  // 38
  BLUES_COMBINED, // 39
  LYDIAN_5, // 40
  JAZZ_MINOR, // 41
  HALF_DIMINISHED,  // 42
  AUGMENTED,  // 43
  HUNGARIAN_MAJOR,  // 44
  HUNGARIAN_MINOR,  // 45
  NEAPOLITAN, // 46
  SPANISH,  // 47
  GREEK,  // 48
  JEWISH1,  // 49
  JEWISH2,  // 50
  INDIAN1,  // 51
  INDIAN2,  // 52
  INDIAN3,  // 53
  INDIAN4,  // 54
  MIDEAST1, // 55
  MIDEAST2, // 56
  MIDEAST3, // 57
  MIDEAST4, // 58
  PENTATONIC_I, // 59
  PENTATONIC_II,  // 60
  PENTATONIC_III, // 61
  PENTATONIC_IV,  // 62
  PENTATONIC_V, // 63
  HIRAJOSHI,  // 64
  INSEN,  // 65
  KOKIN_JOSHI,  // 66
  AKEBONO,  // 67
  RYUKUAN,  // 68
  ABHOGI, // 69
  BHUPKALI, // 70
  HINDOLAM, // 71
  BHUPALAM, // 72
  AMRITAVARSHINI, // 73
  OCTATONIC,  // 74
  ACOUSTIC, // 75
  TRITONE,  // 76
  LEADING_WHOLE_TONE, // 77
  ENIGMATIC,  // 78
  SCRIABIN, // 79
  TCHEREPNIN, // 80
  MESSIAEN_I, // 81
  MESSIANE_II,  // 82
  MESSIANE_III, // 83
  MESSIANE_IV,  // 84
  MESSIANE_V, // 85
  MESSIANE_VI,  // 86
  MESSIANE_VII, // 87
  NATURAL_MAJOR,  // 88
  MAJOR_MINOR,  // 89
  HARMONIC_MAJOR, // 90
  DOUBLE_HARMONIC_MAJOR,  // 91
  NEAPOLITAN_MAJOR, // 92
  MAJOR_LOCRIAN, // 93
  BEBOP_MAJOR,  // 94
  HEXATONIC_1_MAJOR,  // 95
  HEXATONIC_2_MAJOR,  // 96
  PENTATONIC_1_MAJOR, // 97
  PENTATONIC_2_MAJOR, // 98
  PENTATONIC_3_MAJOR, // 99
  NATURAL_MINOR,  // 100
  MINOR_MAJOR,  // 101
  DOUBLE_HARMONIC_MINOR,  // 102
  NEAPOLITAIN_MINOR,  // 103
  MINOR_LOCRIAN, // 104
  BEBOP_MINOR,  // 105
  HEXATONIC_1_MINOR,  // 106
  HEXATONIC_2_MINOR,  // 107
  PENTATONIC_1_MINOR, // 108
  PENTATONIC_2_MINOR, // 109
  PENTATONIC_3_MINOR  // 110
};


const char key_C[2] = "C";
const char key_Csharp[6] = "C#";
const char key_D[2] = "D";
const char key_Dsharp[6] = "D#";
const char key_E[2] = "E";
const char key_F[2] = "F";
const char key_Fsharp[6] = "F#";
const char key_G[2] = "G";
const char key_Gsharp[6] = "G#";
const char key_A[2] = "A";
const char key_Asharp[6] = "A#";
const char key_B[2] = "B";

const char *const notes[12] = {
  key_C, key_Csharp, key_D, key_Dsharp, key_E, key_F,
  key_Fsharp, key_G, key_Gsharp, key_A, key_Asharp, key_B
};
