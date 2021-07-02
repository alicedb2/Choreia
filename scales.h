

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
#define                CHROMATIC 0b1111111111111111
#define                    MAJOR 0b0101101010110101
#define                    MINOR 0b1101010110101101
#define           HARMONIC_MINOR 0b1101100110101101
#define         MAJOR_PENTATONIC 0b0101001010010101
#define         MINOR_PENTATONIC 0b1001010010101001
#define                    BLUES 0b1001010011101001
#define                 JAPANESE 0b1101000110001101
#define                 FREYGISH 0b0011010110110011
#define                     ROMA 0b1101100111001101
#define                 SCOTTISH 0b0101001010100101
#define                   ARABIC 0b0011100110110011
#define                  ALTERED 0b1011010101011011
#define               WHOLE_TONE 0b0101010101010101
#define            HW_DIMINISHED 0b1011011011011011
#define            WH_DIMINISHED 0b1101101101101101

// Mode scales
#define                   IONIAN 0b0101101010110101
#define                   DORIAN 0b1101011010101101
#define                 PHRYGIAN 0b1011010110101011
#define                   LYDIAN 0b0101101011010101
#define               MIXOLYDIAN 0b0101011010110101
#define                  AEOLIAN 0b1101010110101101
#define                  LOCRIAN 0b1011010101101011
#define                IONIAN_B2 0b0011101010110011
#define                DORIAN_B5 0b1101011001101101
#define        HARMONIC_PHRYGIAN 0b1011100110101011
#define           PHRYGIAN_MAJOR 0b1101101010101101
#define                LYDIAN_B3 0b1101101011001101
#define            MAJOR_LOCRIAN 0b0101010101110101
#define            MINOR_LOCRIAN 0b1101010101101101
#define            SUPER_LOCRIAN 0b1011010101011011

// Jazz scales
#define                LYDIAN_7B 0b0101011011010101
#define               DIMINISHED 0b1011011011011011
#define           MIXOLYDIAN_B13 0b0101010110110101
#define        MIXOLYDIAN_B9_B13 0b0011010110110011
#define             LYDIAN_7B_B2 0b1011011010101011
#define                    BEBOP 0b0101011010110101
#define              BLUES_MAJOR 0b1101001010011101
#define              BLUES_MINOR 0b1001010011101001
#define           BLUES_COMBINED 0b1101011011111101
#define                 LYDIAN_5 0b0101101101010101
#define               JAZZ_MINOR 0b1101101010101101
#define          HALF_DIMINISHED 0b1101010101101101
#define                AUGMENTED 0b1001100110011001

// World scales
#define          HUNGARIAN_MAJOR 0b1101100111001101
#define          HUNGARIAN_MINOR 0b1001011011011001
#define               NEAPOLITAN 0b1011100110101011
#define                  SPANISH 0b1011010110111011
#define                    GREEK 0b1101010110011101
#define                  JEWISH1 0b1011001011011011
#define                  JEWISH2 0b1101011011001101
#define                  INDIAN1 0b1101100111001101
#define                  INDIAN2 0b1101101011001101
#define                  INDIAN3 0b0111001110100111
#define                  INDIAN4 0b1001110010111001
#define                 MIDEAST1 0b0011100110110011
#define                 MIDEAST2 0b0011100101110011
#define                 MIDEAST3 0b1011011001101011
#define                 MIDEAST4 0b0011011001110011

// 5-tone scales
#define             PENTATONIC_I 0b0101001010010101
#define            PENTATONIC_II 0b0101010010100101
#define           PENTATONIC_III 0b1001010100101001
#define            PENTATONIC_IV 0b0101001010100101
#define             PENTATONIC_V 0b1001010010101001
#define                HIRAJOSHI 0b1101000110001101
#define                    INSEN 0b0011010010100011
#define              KOKIN_JOSHI 0b0101000110100101
#define                  AKEBONO 0b1101001010001101
#define                  RYUKUAN 0b0001100010110001
#define                   ABHOGI 0b1101001000101101
#define                 BHUPKALI 0b0101000110010101
#define                 HINDOLAM 0b1001010100101001
#define                 BHUPALAM 0b1011000110001011
#define           AMRITAVARSHINI 0b0001100011010001

// Modern scales
#define                OCTATONIC 0b1101101101101101
#define                 ACOUSTIC 0b0101011011010101
#define                  TRITONE 0b0011010011010011
#define       LEADING_WHOLE_TONE 0b0101110101010101
#define                ENIGMATIC 0b0011110101010011
#define                 SCRIABIN 0b0101011001010101
#define               TCHEREPNIN 0b1011101110111011
#define               MESSIAEN_I 0b0101010101010101
#define              MESSIANE_II 0b1011011011011011
#define             MESSIANE_III 0b1101110111011101
#define              MESSIANE_IV 0b0111100111100111
#define               MESSIANE_V 0b0011100011100011
#define              MESSIANE_VI 0b0101110101110101
#define             MESSIANE_VII 0b1111101111101111

// Major scales
#define            NATURAL_MAJOR 0b0101101010110101
#define              MAJOR_MINOR 0b0101010110110101
#define           HARMONIC_MAJOR 0b0101100110110101
#define    DOUBLE_HARMONIC_MAJOR 0b0011100110110011
#define         NEAPOLITAN_MAJOR 0b0011101010110011
#define              BEBOP_MAJOR 0b0101101110110101
#define        HEXATONIC_1_MAJOR 0b0101101010010101
#define        HEXATONIC_2_MAJOR 0b0101001010110101
#define       PENTATONIC_1_MAJOR 0b0101001010010101
#define       PENTATONIC_2_MAJOR 0b0001100010110001
#define       PENTATONIC_3_MAJOR 0b0001101010010001

// Minor scales
#define            NATURAL_MINOR 0b1101010110101101
#define              MINOR_MAJOR 0b1101101010101101
#define    DOUBLE_HARMONIC_MINOR 0b1101100111001101
#define        NEAPOLITAIN_MINOR 0b1011100110101011
#define            MINOR_LOCRIAN 0b1101010101101101
#define              BEBOP_MINOR 0b1101110110101101
#define        HEXATONIC_1_MINOR 0b1101010110001101
#define        HEXATONIC_2_MINOR 0b1101000110101101
#define       PENTATONIC_1_MINOR 0b1101000110001101
#define       PENTATONIC_2_MINOR 0b1001010010101001
#define       PENTATONIC_3_MINOR 0b1001010110001001


// We only got 8 characters
const char Chromatic_string[] PROGMEM = "Chromatc";
const char Major_string[] PROGMEM = "Major";
const char Minor_string[] PROGMEM = "Minor";
const char Harm_Min_string[] PROGMEM = "HarmoMin";
const char Maj_Penta_string[] PROGMEM = "MajPenta";
const char Min_Penta_string[] PROGMEM = "MinPenta";
const char Blues_string[] PROGMEM = "Blues";
const char Japanese_string[] PROGMEM = "Japanese";
const char Freygish_string[] PROGMEM = "Freygish";
const char Romani_string[] PROGMEM = "Romani";
const char Arabic_string[] PROGMEM = "Arabic";
const char Altered_string[] PROGMEM = "Altered";
const char Whl_Tone_string[] PROGMEM = "WhlTone";
const char H_W_Dim_string[] PROGMEM = "HWDimin";
const char W_H_Dim_string[] PROGMEM = "WHDimin";

const char Ionian_string[] PROGMEM = "Ionian";
const char Dorian_string[] PROGMEM = "Dorian";
const char Phrygian_string[] PROGMEM = "Phrygian";
const char Lydian_string[] PROGMEM = "Lydian";
const char Mixolydian_string[] PROGMEM = "Mixolydi";
const char Aeolian_string[] PROGMEM = "Aeolian";
const char Locrian_string[] PROGMEM = "Locrian";
const char Ionian_b2_string[] PROGMEM = "Ionianb2";
const char Dorian_b5_string[] PROGMEM = "Dorianb5";
const char Harm_Phry_string[] PROGMEM = "HarmPhry";
const char Phry_Maj_string[] PROGMEM = "PhrygMaj";
const char Lydian_b3_string[] PROGMEM = "Lydianb3";
const char Maj_Locr_string[] PROGMEM = "MajLocri";
const char Min_Locr_string[] PROGMEM = "MinLocri";
const char Super_Locr_string[] PROGMEM = "SuprLocr";

const char Lydian_7b_string[] PROGMEM = "Lydian7b";
const char Diminished_string[] PROGMEM = "Dmnished";
const char Mix_b13_string[] PROGMEM = "Mixolb13";
const char Mix_b9_b13_string[] PROGMEM = "Mixb9b13";
const char Lyd_7b_b2_string[] PROGMEM = "Lydi7bb2";
const char Bebop_string[] PROGMEM = "Bebop";
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
const char Spanish_string[] PROGMEM = "Spanish";
const char Greek_string[] PROGMEM = "Greek";
const char Jewish_1_string[] PROGMEM = "Jewish1";
const char Jewish_2_string[] PROGMEM = "Jewish2";
const char Indian_1_string[] PROGMEM = "Indian1";
const char Indian_2_string[] PROGMEM = "Indian2";
const char Indian_3_string[] PROGMEM = "Indian3";
const char Indian_4_string[] PROGMEM = "Indian4";
const char Mid_East_1_string[] PROGMEM = "MidEast1";
const char Mid_East_2_string[] PROGMEM = "MidEast2";
const char Mid_East_3_string[] PROGMEM = "MidEast3";
const char Mid_East_4_string[] PROGMEM = "MidEast4";

const char Pent_I_string[] PROGMEM = "PentaI";
const char Pent_II_string[] PROGMEM = "PentaII";
const char Pent_III_string[] PROGMEM = "PentaIII";
const char Pent_IV_string[] PROGMEM = "PentaIV";
const char Pent_V_string[] PROGMEM = "PentaV";
const char Hirajoshi_string[] PROGMEM = "Hirajosh";
const char Insen_string[] PROGMEM = "Insen";
const char KokinJoshi_string[] PROGMEM = "Koknjosh";
const char Akebono_string[] PROGMEM = "Akebono";
const char Ryukuan_string[] PROGMEM = "Ryukuan";
const char Abhogi_string[] PROGMEM = "Abhogi";
const char Bhupkali_string[] PROGMEM = "Bhupkali";
const char Hindolam_string[] PROGMEM = "Hindolam";
const char Bhupalam_string[] PROGMEM = "Bhupalam";
const char Amritavars_string[] PROGMEM = "Amritava";

const char Octatonic_string[] PROGMEM = "Octatonc";
const char Acoustic_string[] PROGMEM = "Acoustic";
const char Tritone_string[] PROGMEM = "Tritone";
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
const char Min_Maj_string[] PROGMEM = "MinMaj";
const char DblHarmMin_string[] PROGMEM = "DblHrmMn";
const char NeapolMin_string[] PROGMEM = "NeapolMn";
const char Minor_Loc_string[] PROGMEM = "MinorLoc";
const char Bebop_Min_string[] PROGMEM = "BebopMin";
const char Hexa_1_Min_string[] PROGMEM = "Hexa1Min";
const char Hexa_2_Min_string[] PROGMEM = "Hexa2Min";
const char Penta1_Min_string[] PROGMEM = "Penta1Mn";
const char Penta2_Min_string[] PROGMEM = "Penta2Mn";
const char Penta3_Min_string[] PROGMEM = "Penta3Mn";

#define NUM_SCALES 120

const char *const short_scale_names[NUM_SCALES] PROGMEM = {
  Chromatic_string, Major_string, Minor_string, Harm_Min_string, Maj_Penta_string,
  Min_Penta_string, Blues_string, Japanese_string, Freygish_string, Romani_string,
  Arabic_string, Altered_string, Whl_Tone_string, H_W_Dim_string, W_H_Dim_string,

  Ionian_string, Dorian_string, Phrygian_string, Lydian_string, Mixolydian_string,
  Aeolian_string, Locrian_string, Ionian_b2_string, Dorian_b5_string, Harm_Phry_string,
  Phry_Maj_string, Lydian_b3_string, Maj_Locr_string, Min_Locr_string, Super_Locr_string,

  Lydian_7b_string, Altered_string, Diminished_string, Mix_b13_string, Mix_b9_b13_string,
  Lyd_7b_b2_string, Bebop_string, Whl_Tone_string, Blues_Maj_string, Blues_Min_string,
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

  Nat_Maj_string, Lydian_string, Mixolydian_string, Maj_Min_string, Harm_Maj_string,
  DblHarmMaj_string, NaepolMaj_string, Major_Loc_string, BluesMaj_string, Bebop_Maj_string,
  Hexa_1_Maj_string, Hexa_2_Maj_string, Penta1_Maj_string, Penta2_Maj_string, Penta3_Maj_string,

  Nat_Min_string, Dorian_string, Phrygian_string, Min_Maj_string, Harm_Min_string,
  DblHarmMin_string, NeapolMin_string, Minor_Loc_string, Blues_Min_string, Bebop_Min_string,
  Hexa_1_Min_string, Hexa_2_Min_string, Penta1_Min_string, Penta2_Min_string, Penta3_Min_string
};
