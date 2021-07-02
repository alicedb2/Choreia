# Alician Choreia

Euclidean sequencer with in-scale Markov chain.

### Jacks

- **Input** Clock-in
- **Output** 2 CVs and 2 gates

### Knobs

Each of the 2 sequencer has two pots

- **Pot 1** Cycles through the 16 canonical Euclidean sequences, i.e. events at the integer part of 16k/N where k runs from 0 to 15 and N is the value at the pots between 0 and 16. When N = 0 the sequence is empty.
- **Pot 2** Adds a number of shadow steps, i.e. events happen at the integer part of 16(k + s)/N where s runs from 0 to whatever. Here it's from 0 to 16. This increase the total number of possible sequences to more than 200.


### Parameters

For now the parameters are hard coded.

The generation of the Markov transition matrix takes five parameters:
- **Scale width** How far across notes the Markov chain will, i.e. 13 would be from C0 to C1. For now the scale width can only go to 20, so from C0 to G1, because of the 2KB limitation of the Arduino nano's dynamic memory, i.e. 20 * 20 * 4 bytes per floats.
- **Scale dispersion** Width of a Cauchy distribution controlling how wide the jumps within the scales are on average,
- **Stay on note** Weight of the probability of playing the same note one after another,
- **Step on first neighbor** Weight of the probability of playing the first neighbor of a note in the scale after itself,
- **Root note** Weight of the probability of playing the root note whatever the current note is.


### Quirk

- The gate duty follows the clock-in duty and is common to both sequencers. Using a drum track gate out like on the BeatStep Pro works a charm.

### TODO

- Get rid of the maximum scale width of 20 by only considering the state space of notes that are on the scale. Simply use a map from kth note on the scale to semitone rather than including all semitones and wasting space with empty rows and columns in the Markov transition matrix.
- Two click buttons. 
    - Hold one to change the four parameters of the Markov chain with pots.
    - Hold the other to change the scale and root note with two of the pots, the seed that generates the Markov chain with the third pot, and the fourth pot is left unused for now.
- CV-in jack to control the Euclidean sequence itself.
- Add a second clock-in jack. When only one clock-in is connected both sequences have the same gate duty. When two are connected each sequencer follows its own clock-in duty.

