

///// Most scales from Native Instruments KompleteKontrol guide

/////   Mask are relative to the root note
/////   e.g. mask over note in C   B A G FE D C  (black notes are implicit)
/////                      1011010110110101

// Tuning from C0 to C2, dependent on low pass filter
//uint8_t semitone_cvs[] = {0, 4, 10, 15, 20, 25, 31, 38, 44, 51, 56, 63,
//                          69, 74, 79, 88, 93, 97, 101, 105, 109, 104, 116,
//                          119
//                         };

// Main scales                        
#define                CHROMATIC 0b111111111111
#define                    MAJOR 0b101010110101
#define                    MINOR 0b010110101101
#define           HARMONIC_MINOR 0b100110101101
#define         MAJOR_PENTATONIC 0b001010010101
#define         MINOR_PENTATONIC 0b010010101001
#define                    BLUES 0b010011101001
#define                 JAPANESE 0b000110001101
#define                 FREYGISH 0b010110110011
#define                   ROMANI 0b100111001101
#define                 SCOTTISH 0b001010100101 //++
#define                   ARABIC 0b100110110011
#define                  ALTERED 0b010101011011
#define               WHOLE_TONE 0b010101010101
#define            HW_DIMINISHED 0b011011011011
#define            WH_DIMINISHED 0b101101101101

// Mode scales
#define                   IONIAN 0b101010110101
#define                   DORIAN 0b011010101101
#define                 PHRYGIAN 0b010110101011
#define                   LYDIAN 0b101011010101
#define               MIXOLYDIAN 0b011010110101
#define                  AEOLIAN 0b010110101101
#define                  LOCRIAN 0b010101101011
#define                IONIAN_B2 0b101010110011
#define                DORIAN_B5 0b011001101101
#define        HARMONIC_PHRYGIAN 0b100110101011
#define           PHRYGIAN_MAJOR 0b101010101101
#define                LYDIAN_B3 0b101011001101
#define            MAJOR_LOCRIAN 0b010101110101
#define            MINOR_LOCRIAN 0b010101101101
#define            SUPER_LOCRIAN 0b010101011011

// Jazz scales
#define                LYDIAN_7B 0b011011010101
//#define                  ALTERED 0b010101011011
#define               DIMINISHED 0b011011011011
#define           MIXOLYDIAN_B13 0b010110110101
#define        MIXOLYDIAN_B9_B13 0b010110110011
#define             LYDIAN_7B_B2 0b011010101011
#define                    BEBOP 0b011010110101
//#define               WHOLE_TONE 0b010101010101
#define              BLUES_MAJOR 0b001010011101
#define              BLUES_MINOR 0b010011101001
#define           BLUES_COMBINED 0b011011111101
#define                 LYDIAN_5 0b101101010101
#define               JAZZ_MINOR 0b101010101101
#define          HALF_DIMINISHED 0b010101101101
#define                AUGMENTED 0b100110011001

// World scales
#define          HUNGARIAN_MAJOR 0b100111001101
#define          HUNGARIAN_MINOR 0b011011011001
#define               NEAPOLITAN 0b100110101011
#define                  SPANISH 0b010110111011
#define                    GREEK 0b010110011101
#define                  JEWISH1 0b001011011011
#define                  JEWISH2 0b011011001101
#define                  INDIAN1 0b100111001101
#define                  INDIAN2 0b101011001101
#define                  INDIAN3 0b001110100111
#define                  INDIAN4 0b110010111001
#define                 MIDEAST1 0b100110110011
#define                 MIDEAST2 0b100101110011
#define                 MIDEAST3 0b011001101011
#define                 MIDEAST4 0b011001110011

// 5-tone scales
#define             PENTATONIC_I 0b001010010101
#define            PENTATONIC_II 0b010010100101
#define           PENTATONIC_III 0b010100101001
#define            PENTATONIC_IV 0b001010100101
#define             PENTATONIC_V 0b010010101001
#define                HIRAJOSHI 0b000110001101
#define                    INSEN 0b010010100011
#define               KOKINJOSHI 0b000110100101
#define                  AKEBONO 0b001010001101
#define                  RYUKUAN 0b100010110001
#define                   ABHOGI 0b001000101101
#define                 BHUPKALI 0b000110010101
#define                 HINDOLAM 0b010100101001
#define                 BHUPALAM 0b000110001011
#define           AMRITAVARSHINI 0b100011010001

// Modern scales
#define                OCTATONIC 0b101101101101
#define                 ACOUSTIC 0b011011010101
#define                  TRITONE 0b010011010011
#define       LEADING_WHOLE_TONE 0b110101010101
#define                ENIGMATIC 0b110101010011
#define                 SCRIABIN 0b011001010101
#define               TCHEREPNIN 0b101110111011
#define               MESSIAEN_I 0b010101010101
#define              MESSIANE_II 0b011011011011
#define             MESSIANE_III 0b110111011101
#define              MESSIANE_IV 0b100111100111
#define               MESSIANE_V 0b100011100011
#define              MESSIANE_VI 0b110101110101
#define             MESSIANE_VII 0b101111101111

// Major scales
#define            NATURAL_MAJOR 0b101010110101
//#define                   LYDIAN 0b101011010101
//#define               MIXOLYDIAN 0b011010110101
#define              MAJOR_MINOR 0b010110110101
#define           HARMONIC_MAJOR 0b100110110101
#define    DOUBLE_HARMONIC_MAJOR 0b100110110011
#define         NEAPOLITAN_MAJOR 0b101010110011
//#define            MAJOR_LOCRIAN 0b010101110101
//#define              BLUES_MAJOR 0b001010011101
#define              BEBOP_MAJOR 0b101110110101
#define        HEXATONIC_1_MAJOR 0b101010010101
#define        HEXATONIC_2_MAJOR 0b001010110101
#define       PENTATONIC_1_MAJOR 0b001010010101
#define       PENTATONIC_2_MAJOR 0b100010110001
#define       PENTATONIC_3_MAJOR 0b101010010001

// Minor scales
#define            NATURAL_MINOR 0b010110101101
//#define                   DORIAN 0b011010101101
//#define                 PHRYGIAN 0b010110101011
#define              MINOR_MAJOR 0b101010101101
//#define           HARMONIC_MINOR 0b100110101101
#define    DOUBLE_HARMONIC_MINOR 0b100111001101
#define        NEAPOLITAIN_MINOR 0b100110101011
#define            MINOR_LOCRIAN 0b010101101101
//#define              BLUES_MINOR 0b010011101001
#define              BEBOP_MINOR 0b110110101101
#define        HEXATONIC_1_MINOR 0b010110001101
#define        HEXATONIC_2_MINOR 0b000110101101
#define       PENTATONIC_1_MINOR 0b000110001101
#define       PENTATONIC_2_MINOR 0b010010101001
#define       PENTATONIC_3_MINOR 0b010110001001


// We only got 8 characters
const char Chromatic_string[] PROGMEM = "Chromatc";
const char Major_string[] PROGMEM = "Major   ";
const char Minor_string[] PROGMEM = "Minor   ";
const char Harm_Min_string[] PROGMEM = "HarmoMin";
const char Maj_Penta_string[] PROGMEM = "MajPenta";
const char Min_Penta_string[] PROGMEM = "MinPenta";
const char Blues_string[] PROGMEM = "Blues   ";
const char Japanese_string[] PROGMEM = "Japanese";
const char Freygish_string[] PROGMEM = "Freygish";
const char Romani_string[] PROGMEM = "Romani  ";
const char Scottish_string[] PROGMEM = "Scottish";
const char Arabic_string[] PROGMEM = "Arabic  ";
const char Altered_string[] PROGMEM = "Altered ";
const char Whl_Tone_string[] PROGMEM = "WhlTone ";
const char H_W_Dim_string[] PROGMEM = "HWDimin ";
const char W_H_Dim_string[] PROGMEM = "WHDimin ";

const char Ionian_string[] PROGMEM = "Ionian  ";
const char Dorian_string[] PROGMEM = "Dorian  ";
const char Phrygian_string[] PROGMEM = "Phrygian";
const char Lydian_string[] PROGMEM = "Lydian  ";
const char Mixolydian_string[] PROGMEM = "Mixolydi";
const char Aeolian_string[] PROGMEM = "Aeolian ";
const char Locrian_string[] PROGMEM = "Locrian ";
const char Ionian_b2_string[] PROGMEM = "Ionianb2";
const char Dorian_b5_string[] PROGMEM = "Dorianb5";
const char Harm_Phry_string[] PROGMEM = "HarmPhry";
const char Phry_Maj_string[] PROGMEM = "PhrygMaj";
const char Lydian_b3_string[] PROGMEM = "Lydianb3";
const char Maj_Locr_string[] PROGMEM = "MajLocri";
const char Min_Locr_string[] PROGMEM = "MinLocri";
const char Super_Locr_string[] PROGMEM = "SuprLocr";

const char Lydian_7b_string[] PROGMEM = "Lydianb7";
const char Diminished_string[] PROGMEM = "Dmnished";
const char Mix_b13_string[] PROGMEM = "MixolB13";
const char Mix_b9_b13_string[] PROGMEM = "Mixb9B13";
const char Lyd_7b_b2_string[] PROGMEM = "Lydib7B2";
const char Bebop_string[] PROGMEM = "Bebop   ";
const char Blues_Maj_string[] PROGMEM = "BluesMaj";
const char Blues_Min_string[] PROGMEM = "BluesMin";
const char Blues_Comb_string[] PROGMEM = "BluesCmb";
const char Lydian_5_string[] PROGMEM = "Lydian#5";
const char Jazz_Min_string[] PROGMEM = "JazzMin";
const char Half_Dim_string[] PROGMEM = "HalfDim";
const char Augmented_string[] PROGMEM = "Augmnted";

const char Hungar_Min_string[] PROGMEM = "HungrMin";
const char Hungar_Maj_string[] PROGMEM = "HungrMaj";
const char Neapolitan_string[] PROGMEM = "Neapolit";
const char Spanish_string[] PROGMEM = "Spanish ";
const char Greek_string[] PROGMEM = "Greek   ";
const char Jewish_1_string[] PROGMEM = "Jewish1 ";
const char Jewish_2_string[] PROGMEM = "Jewish2 ";
const char Indian_1_string[] PROGMEM = "Indian1 ";
const char Indian_2_string[] PROGMEM = "Indian2 ";
const char Indian_3_string[] PROGMEM = "Indian3 ";
const char Indian_4_string[] PROGMEM = "Indian4 ";
const char Mid_East_1_string[] PROGMEM = "MidEast1";
const char Mid_East_2_string[] PROGMEM = "MidEast2";
const char Mid_East_3_string[] PROGMEM = "MidEast3";
const char Mid_East_4_string[] PROGMEM = "MidEast4";

const char Pent_I_string[] PROGMEM = "PentaI  ";
const char Pent_II_string[] PROGMEM = "PentaII ";
const char Pent_III_string[] PROGMEM = "PentaIII";
const char Pent_IV_string[] PROGMEM = "PentaIV ";
const char Pent_V_string[] PROGMEM = "PentaV  ";
const char Hirajoshi_string[] PROGMEM = "Hirajosh";
const char Insen_string[] PROGMEM = "Insen   ";
const char KokinJoshi_string[] PROGMEM = "Koknjosh";
const char Akebono_string[] PROGMEM = "Akebono ";
const char Ryukuan_string[] PROGMEM = "Ryukuan ";
const char Abhogi_string[] PROGMEM = "Abhogi  ";
const char Bhupkali_string[] PROGMEM = "Bhupkali";
const char Hindolam_string[] PROGMEM = "Hindolam";
const char Bhupalam_string[] PROGMEM = "Bhupalam";
const char Amritavars_string[] PROGMEM = "Amritava";

const char Octatonic_string[] PROGMEM = "Octatonc";
const char Acoustic_string[] PROGMEM = "Acoustic";
const char Tritone_string[] PROGMEM = "Tritone ";
const char LeadingWhl_string[] PROGMEM = "LeadngWh";
const char Enigmatic_string[] PROGMEM = "Enigmatc";
const char Scriabin_string[] PROGMEM = "Scriabin";
const char Tcherepnin_string[] PROGMEM = "Tcherepn";
const char Messiaen_1_string[] PROGMEM = "Messiae1";
const char Messiaen_2_string[] PROGMEM = "Messiae2";
const char Messiaen_3_string[] PROGMEM = "Messiae3";
const char Messiaen_4_string[] PROGMEM = "Messiae4";
const char Messiaen_5_string[] PROGMEM = "Messiae5";
const char Messiaen_6_string[] PROGMEM = "Messiae6";
const char Messiaen_7_string[] PROGMEM = "Messiae7";

const char Nat_Maj_string[] PROGMEM = "NaturMaj";
const char Maj_Min_string[] PROGMEM = "MajMin";
const char Harm_Maj_string[] PROGMEM = "HarmoMaj";
const char DblHarmMaj_string[] PROGMEM = "DblHrmMj";
const char NaepolMaj_string[] PROGMEM = "NaeplMaj";
const char Major_Loc_string[] PROGMEM = "MajorLoc";
const char BluesMaj_string[] PROGMEM = "BluesMaj";
const char Bebop_Maj_string[] PROGMEM = "BebopMaj";
const char Hexa_1_Maj_string[] PROGMEM = "Hexa1Maj";
const char Hexa_2_Maj_string[] PROGMEM = "Hexa2Maj";
const char Penta1_Maj_string[] PROGMEM = "Penta1Mj";
const char Penta2_Maj_string[] PROGMEM = "Penta2Mj";
const char Penta3_Maj_string[] PROGMEM = "Penta3Mj";

const char Nat_Min_string[] PROGMEM = "NaturMin";
const char Min_Maj_string[] PROGMEM = "MinMaj  ";
const char DblHarmMin_string[] PROGMEM = "DblHrmMn";
const char NeapolMin_string[] PROGMEM = "NeapolMn";
const char Minor_Loc_string[] PROGMEM = "MinorLoc";
const char Bebop_Min_string[] PROGMEM = "BebopMin";
const char Hexa_1_Min_string[] PROGMEM = "Hexa1Min";
const char Hexa_2_Min_string[] PROGMEM = "Hexa2Min";
const char Penta1_Min_string[] PROGMEM = "Penta1Mn";
const char Penta2_Min_string[] PROGMEM = "Penta2Mn";
const char Penta3_Min_string[] PROGMEM = "Penta3Mn";

#define NUM_SCALES 111

const uint16_t scales[NUM_SCALES] = {
  CHROMATIC,  // 0
  MAJOR,  // 1
  MINOR,  // 2
  HARMONIC_MINOR, // 3
  MAJOR_PENTATONIC, // 4
  MINOR_PENTATONIC, // 5
  BLUES,  // 6
  JAPANESE, // 7
  FREYGISH, // 8
  ROMANI, // 9
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
  KOKINJOSHI,  // 66
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


const char *const short_scale_names[NUM_SCALES] PROGMEM = {
  Chromatic_string, Major_string, Minor_string, Harm_Min_string, Maj_Penta_string,
  Min_Penta_string, Blues_string, Japanese_string, Freygish_string, Romani_string,
  Scottish_string, Arabic_string, Altered_string, Whl_Tone_string, H_W_Dim_string, W_H_Dim_string,

  Ionian_string, Dorian_string, Phrygian_string, Lydian_string, Mixolydian_string,
  Aeolian_string, Locrian_string, Ionian_b2_string, Dorian_b5_string, Harm_Phry_string,
  Phry_Maj_string, Lydian_b3_string, Maj_Locr_string, Min_Locr_string, Super_Locr_string,

  Lydian_7b_string, 
  //Altered_string, 
  Diminished_string, Mix_b13_string, Mix_b9_b13_string,
  Lyd_7b_b2_string, Bebop_string, 
  //Whl_Tone_string, 
  Blues_Maj_string, Blues_Min_string,
  Blues_Comb_string, Lydian_5_string, Jazz_Min_string, Half_Dim_string, Augmented_string,

  Hungar_Min_string, Hungar_Maj_string, Neapolitan_string, Spanish_string, Greek_string,
  Jewish_1_string, Jewish_2_string, Indian_1_string, Indian_2_string, Indian_3_string,
  Indian_4_string, Mid_East_1_string, Mid_East_2_string, Mid_East_3_string, Mid_East_4_string,

  Pent_I_string, Pent_II_string, Pent_III_string, Pent_IV_string, Pent_V_string,
  Hirajoshi_string, Insen_string, KokinJoshi_string, Akebono_string, Ryukuan_string,
  Abhogi_string, Bhupkali_string, Hindolam_string, Bhupalam_string, Amritavars_string,

  Octatonic_string, Acoustic_string, Augmented_string, Tritone_string, LeadingWhl_string,
  Enigmatic_string, Scriabin_string, Tcherepnin_string, Messiaen_1_string, Messiaen_2_string,
  Messiaen_3_string, Messiaen_4_string, Messiaen_5_string, Messiaen_6_string, Messiaen_7_string,

  Nat_Maj_string, 
  //Lydian_string, Mixolydian_string, 
  Maj_Min_string, Harm_Maj_string,
  DblHarmMaj_string, NaepolMaj_string, 
  //Major_Loc_string, BluesMaj_string, 
  Bebop_Maj_string,
  Hexa_1_Maj_string, Hexa_2_Maj_string, Penta1_Maj_string, Penta2_Maj_string, Penta3_Maj_string,

  Nat_Min_string, 
  //Dorian_string, Phrygian_string, 
  Min_Maj_string, 
  //Harm_Min_string,
  DblHarmMin_string, NeapolMin_string, Minor_Loc_string, 
  //Blues_Min_string, 
  Bebop_Min_string,
  Hexa_1_Min_string, Hexa_2_Min_string, Penta1_Min_string, Penta2_Min_string, Penta3_Min_string
};
