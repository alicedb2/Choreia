////// Chords from https://www.edmprod.com/different-chord-types/

////// Chords are relative to the root note as well

#define         NO_CHORD 0b1111111111111111

// Major chords sound happy and simple.
#define          MAJOR_CHORD 0b000010010001

//Minor chords are considered to be sad, or ‘serious.’
#define          MINOR_CHORD 0b000010001001

//Diminished Chords sound tense and unpleasant.
#define     DIMINISHED_CHORD 0b000001001001

// Major seventh chords are considered to be thoughtful, soft.
#define      MAJOR_7TH_CHORD 0b100010010001

// Minor seventh chords are considered to be moody, or contemplative.
#define      MINOR_7TH_CHORD 0b010010001001

// Dominant seventh chords are considered to be strong and restless.
#define   DOMINANT_7TH_CHORD 0b110010010001

// Sus2 Chords sound bright and nervous.
#define  SUSPENDED_2ND_CHORD 0b000010000101

// Sus4 Chords, like Sus2 chords, sound bright and nervous.
#define  SUSPENDED_4TH_CHORD 0b000010100001

// Augmented chords sound anxious and suspenseful.
#define      AUGMENTED_CHORD 0b000100010001

// Extended chords
#define    MAJOR_NINTH_CHORD 0b100100010010001
#define    MINOR_NINTH_CHORD 0b100010010001001
#define DOMINANT_NINTH_CHORD 0b100010010010001

// Can't fit in uint16_t, meh
//#define MAJOR_ELEVENTH_CHORD 0b100100100010010001
//#define MINOR_ELEVENTH_CHORD 0b100100010010001001
