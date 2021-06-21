

///// Most scales from Native Instruments KompleteKontrol guide

/////   Mask are relative to the root note
/////   e.g. mask over note in C   B A G FE D C  (black notes are implicit)
/////                      1011010110110101

// Tuning from C0 to C2, dependent on low pass filter
//uint8_t semitone_cvs[] = {0, 4, 10, 15, 20, 25, 31, 38, 44, 51, 56, 63,
//                          69, 74, 79, 88, 93, 97, 101, 105, 109, 104, 116,
//                          119
//                         };

// Custom scales
#define                 CUSTOM_1 0b1011010010001011

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
#define            MAJOR_LOCRIAN 0b0111011101010111
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
#define           MAJOR_LOCARIAN 0b0101010101110101
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
#define           MINOR_LOCARIAN 0b1101010101101101
#define              BEBOP_MINOR 0b1101110110101101
#define        HEXATONIC_1_MINOR 0b1101010110001101
#define        HEXATONIC_2_MINOR 0b1101000110101101
#define       PENTATONIC_1_MINOR 0b1101000110001101
#define       PENTATONIC_2_MINOR 0b1001010010101001
#define       PENTATONIC_3_MINOR 0b1001010110001001
