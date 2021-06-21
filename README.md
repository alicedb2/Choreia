# Euclid and Markov at the Pub

Euclidean sequencer with on-scale Markov chain.

### Jacks

- **Input** Clock-in
- **Output** 2 CVs and 2 gates

### Knobs

Each of the 2 sequencer has two pots

- **Pot 1** Cycles through the 16 canonical Euclidean sequences, i.e. events at the integer part of 16k/N where k runs from 0 to 15 and N is the value at the pots between 0 and 16. We define int(16k/0) := 0.
- **Pot 2** Adds a number of shadow steps, i.e. events happen at the integer part of 16(k + s)/N where s runs from 0 to whatever. Here it's from 0 to 16. There's a redundancy k and s but we increase the number of possible sequences to 32.


### Parameters

For now the parameters are hard coded.

- **Scale dispersion** Width of a Cauchy distribution controlling how wide the jumps within the scales are on average,
- **Stay on note** Weight of the probability of playing the same note one after another,
- **Step on first neighbor** Weight of the probability of playing jumping to the first neighbor on the scale,
- **Root note** Weight of the probability of playing the root note.

### Quirk

- The gate duty follows the clock-in duty and is common to both sequencers. Using a drum track gate out like on the BeatStep Pro works a charm.

### TODO

- Two click buttons. 
-- Hold one to change the four parameters of the Markov chain with pots.
-- Hold the other to change the scale and root note with two of the pots, the seed that generates the Markov chain with the third pot, and the fourth pot is left unused for now.
- CV-in jack to control the Euclidean sequence itself.
- Add a second clock-in jack. When only one clock-in is connected both sequences have the same gate duty. When two are connected each sequencer follows its own clock-in duty.

