Microphone Module and Noise Mitigation
--------------------------------------

The project uses a MAX9418 microphone module, which provides a convenient output level that can be sampled directly by the Pico’s ADC input. During testing, it was observed that enabling the Pico W’s radio introduced significant power supply noise, which coupled into the microphone signal and degraded audio quality. This issue was effectively mitigated by placing a 1 kΩ resistor in series between the Pico’s 3.3 V supply and the microphone’s VCC pin. Combined with the capacitors already present on the microphone module, this forms a simple low-pass filter that attenuates high-frequency noise from the power supply. Since the MAX9418 draws very little current, the voltage drop across the resistor is minimal, so the microphone still receives sufficient voltage for proper operation while the PSU noise is strongly suppressed.

Capturing Audio from the ADC
----------------------------

Audio input is captured directly from the built-in ADC of the microcontroller. To efficiently handle continuous sampling without missing data, the ADC is configured to use DMA (Direct Memory Access) into a ping-pong buffer, allowing one half of the buffer to be filled while the other is processed. DMA completion interrupts are used to switch between the two buffer halves, signaling to the application that a new block of samples is ready. The user code continuously checks for the availability of a new buffer, and it is guaranteed one buffer’s worth of time to complete processing before the next block arrives. Sampling is performed at 10 kHz, which is more than sufficient to capture speech frequencies, ensuring clear and intelligible voice transmission over the walkie-talkie link while minimizing CPU load.

Audio Output Stage
------------------

Audio output is handled by a MAX98357A digital audio amplifier, which can be obtained very cheaply while still providing excellent audio quality. The device is driven directly from a digital I²S interface, eliminating the need for a DAC or analog filtering stages and keeping the audio path simple and noise-resistant. As a switch-mode (Class-D) amplifier, the MAX98357A is highly efficient, which is particularly important for a battery-powered walkie-talkie and helps to extend operating time between charges. The amplifier is paired with a 20 mm miniature speaker; while small, these speakers sound surprisingly good when mounted in a sealed enclosure, which significantly improves low-frequency response and overall clarity.

A-law Compression
-----------------

To efficiently transmit voice over our walkie-talkie, we use A-law compression to reduce the bit depth of the audio signal. The incoming audio from the microphone is captured as 12-bit PCM samples, which provide high fidelity but require significant bandwidth if transmitted directly. A-law compression logarithmically reduces the dynamic range of the signal, emphasizing quieter sounds and slightly compressing louder ones. This allows the audio to be encoded into 8-bit values, reducing the data rate while preserving intelligibility and clarity. The compression process is reversible, so the original waveform can be approximately reconstructed on the receiving side.

Our implementation includes both compression and expansion curves, which map the original 12-bit PCM values to 8-bit A-law values and back. These curves are carefully generated and visualized using Python, ensuring that the compression maintains voice quality and avoids distortion in low-amplitude signals. By combining A-law compression with digital transmission, the walkie-talkie achieves a balance between audio clarity and efficient bandwidth usage, making the system both compact and practical for real-time communication.

.. image:: images/walkie_talkie/a_law_compression.png

.. image:: images/walkie_talkie/a_law_expansion.png


Wi-Fi Connection Modes
----------------------

The firmware supports two different Wi-Fi communication modes. The first uses the WiFiManager code previously developed for another project. This provides a very clean and user-friendly way to configure Wi-Fi credentials and requires only two additional source files to be added to the project. It has proven to be reliable and easy to integrate, and has become the preferred (“go-to”) method for Wi-Fi configuration across multiple projects.

The second mode is a pseudo peer-to-peer (P2P) configuration, designed to allow the walkie-talkies to operate when no external Wi-Fi router is available. In this mode, one unit takes the role of an access point (AP) while the other operates as a station (STA). To allow a single common firmware build to run on both devices, an automatic role-negotiation process is used. On first startup, each device initially attempts to connect as an STA; if this fails, it reconfigures itself as an AP. If both attempts fail, the device waits for a random delay before retrying, which helps prevent both units from synchronising into the same role and blocking connection establishment. Once a connection is successfully made, the negotiated role is stored in flash using the emulated EEPROM library, allowing much faster reconnection on subsequent power cycles.

stateDiagram-v2
    [*] --> Init

    Init --> Try_STA : Power on / Reset

    Try_STA --> Connected_STA : STA connection successful
    Try_STA --> Try_AP : STA connection failed

    Try_AP --> Connected_AP : AP started and peer connected
    Try_AP --> Random_Wait : AP start or connection failed

    Random_Wait --> Try_STA : Random delay elapsed

    Connected_STA --> Store_Role
    Connected_AP --> Store_Role

    Store_Role -->
