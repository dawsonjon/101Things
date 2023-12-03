DIY Multi-Effect Guitar Unit: Affordable, Easy, and Versatile
============================================================

Unlock Your Guitar's Potential on a Budget
------------------------------------------

Are you a guitarist looking to explore a myriad of tones without breaking the
bank? Look no further! Introducing our *DIY Multi-Effect Guitar Unit* — a
cost-effective, straightforward solution for musicians who crave versatility
without the complexity.

The DIY Multi-Effect Guitar Unit uses a minimalistic design that harnesses
the power of the Raspberry Pi Pico and just a handful of additional components.
With a total cost that amounts to just a few pounds, this project is not only
budget-friendly but also remarkably simple to build. Regardless of your
technical background, this unit is engineered to be accessible to anyone with
a passion for music and a desire to experiment with their sound.

The secret to its affordability lies in the utilization of the Raspberry Pi
Pico, a versatile microcontroller that serves as the brain of our DIY unit.
Despite its modest cost, the Raspberry Pi Pico provides a robust platform for
implementing a wide array of guitar effects usually found in much more expensive
units.

So, whether you're a seasoned electronics enthusiast or someone just dipping
their toes into the world of DIY projects, the design allows you to create a
feature-rich, multi-effect guitar unit without the headache of a convoluted
assembly process or a hefty price tag.


Effects
-------

The unit includes most common effects, may of which can be used simultaneously.

+ 5-Band Graphic Equalizer
+ Multiple Distortion Effects
+ Delay
+ Echo
+ Reverb
+ Chorus
+ Flanger
+ Phaser
+ Tremollo
+ Vibrato
+ Multiple Patch Memories

Hardware Overview
-----------------

**Signal Path Amplification**

The signal processing in our DIY Multi-Effect Guitar Pedal begins with a
non-inverting amplifier with a high input impedance to avoid interference
with the guitar's natural tone. This amplifier offers 20dB of gain, strengthening
the input signal for subsequent processing.

**Analog-to-Digital Conversion (ADC)**

After amplification, the signal enters the built-in Analog-to-Digital Converter
(ADC), providing a resolution of 12 bits. Operating at a sampling rate of 320
kHz, the ADC captures the intricacies of the guitar output with precision.

**Oversampling for Enhanced Signal Quality**

To further enhance signal quality, the design incorporates a 16x oversampling
technique. This technique adds an additional 2 bits to the Signal-to-Noise
Ratio (SNR) and helps minimize aliasing effects. Oversampling at a higher rate
than strictly necessary aids in reducing unwanted artifacts and contributes to
the overall clarity of the processed sound.

**PWM Audio**

A Pulse Witdh Modulated (PWM) digital IO pin is used to output the audio. The
PWM is configured to provide a 12-bit audio output. Subsequent to the
oversampling stage, the signal is directed through a first-order low-pass
filter designed to eliminate any residual switching waveforms. This crucial
filtering step ensures a cleaner output signal, free from unwanted artifacts
and interference.

Given the crude nature of the PWM audio output, the quality of the audio is
suprisingly good. The PWM signal is strong enough to drive headphones directly.
The unit could be used as a headphone practice amplifier, or it could be
integrated into an effects pedal, or even into a guitar amplifier.

**User Interface**

There isn't anything particularly unusual about the user interface. A 128x64
OLED display uses an ssd1306-based I2C interface. These are fairly ubiquitous
these days and have replaced the HD44780 as the go-to cheap/simple display. The
I2C interface certainly helps reduce the pin count. Cost is a key driver, so I
have combined this with a few tactile push buttons to build a menu driven
interface.

Software Overview
-----------------

The Raspberry Pi Pico has dual core ARM processor, each core running at 120MHz.
This is fairly fast for a microcontroller in this price range and is more than
capable of performing some audio processing. However Memory and CPU are still a
relatively limited resource. The Pi Pico doesn't have a Floating Point Unit, so
the design only uses fixed point arrithmetic in the signal path. This gives us
more headroom and allows us to fit in more effects.

The signal path is divided into 5 main functional blocks:

+ DC removal and preamplifier
+ 5-Band Graphic Equaliser
+ Waveshaping (distortion)
+ Delay (delay, echo, reverb)
+ Modulation (Chorus, Phaser, Flanger, Tremollo, Vibrato)

Graphic Equaliser
-----------------

For my first attempt at a graphic equalizer, I used a bank of 5 filters, each
covering a portion of the spectrum. I opted for IIR filters, known for their
efficiency. The approach worked quite well when processing some test audio on a
laptop, but it couldn't keep up on the Pi Pico. This is probably because I used
a floating-point implementation. I considered converting this to a fixed-point
implementation, but designing stable IIR filters using fixed-point arithmetic
can be fairly tricky.

In the final solution, I utilized the Fast Fourier Transform (FFT) algorithm to
perform the filtering in the frequency domain. Although the process of
converting from the time domain to the frequency domain and back again is
complex, the filtering operation is much simpler in the frequency domain,
leading to an overall improvement for reasonably large filters.

The data is broken down into overlapping chunks of 64 samples. Each chunk is
windowed using a Hann window to reduce spectral leakage. Subsequently, each
chunk is transformed into the frequency domain using the FFT function,
separating the signal into 32 frequency bins. Each bin can then be scaled
according to a gain value before being transformed back to the time domain
using an inverse FFT.

Graphic equalizer bands are typically sized on a logarithmic scale. The first
band uses a single frequency bin, the second uses 2, and the subsequent bands
use 4, 8, and 18, respectively. This logarithmic scaling ensures that each band
covers the same number of musical tones.


Modulation Effects
------------------

In the realm of guitar effects, modulation effects play a pivotal role in
shaping the sonic landscape, adding depth, movement, and character to your
playing. These effects manipulate the audio signal in real-time, creating
dynamic variations that go beyond the static nature of traditional distortion
or delay effects.

**1. Phaser:**

A phaser effect alters the phase of the input signal, creating a distinctive
swirling or sweeping sound. By splitting the signal and modulating the phase
relationship between the two paths, phasers produce a rich, evolving texture
often associated with spacey or psychedelic tones.

**2. Chorus:**

Chorus imparts a lush and shimmering quality to your sound by duplicating the
signal, introducing slight pitch and time variations, and blending it with the
original. The result is akin to a choir of subtly detuned voices, producing a
thicker, more expansive sound that simulates the effect of multiple instruments
playing in unison.

**3. Flanger:**

Similar to a chorus, a flanger duplicates the signal but introduces a short
delay that varies over time. This creates a sweeping, jet-like sound
characterized by a distinctive whooshing effect. Flangers are commonly used to
add a sense of motion and intensity to guitar solos or rhythm parts.

**4. Tremolo:**

Tremolo modulates the volume of the signal at a rhythmic pace, creating a
pulsating effect. It's a classic effect that can range from subtle rhythmic
pulsing to more pronounced amplitude modulation. Tremolo can add a vintage,
surf-rock vibe or a dynamic rhythmic element to your playing.

**5. Vibrato:**

Vibrato, often confused with tremolo, modulates the pitch of the signal. It
introduces slight variations in pitch, simulating the natural vibrato produced
by a guitarist's finger movements. Vibrato adds expressiveness and warmth to
sustained notes, making it a popular choice for lead guitar playing.

**Diving into Sonic Exploration:**

With our DIY Multi-Effect Guitar Pedal, you gain access to these modulation
effects and more, allowing you to explore a vast sonic landscape. Whether
you're aiming for the lush textures of chorus, the spatial swirls of a phaser,
or the dynamic pulsations of tremolo, these modulation effects open up a world
of creative possibilities for your musical journey.




