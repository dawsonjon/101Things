#include "music.hpp"
const note_t SquadronTheme[] = {
    {523, 1500},   // c5
    {523, 1500},   // c5
    {523, 1500},   // c5
    {698, 1500},   // f5
    {784, 1500},   // g5
    {932, 1500},   // a#5
    {880, 3000},   // a5
    {1047, 12000}, // c6
    {0, 3000},     // p5
    {523, 1500},   // c5
    {523, 1500},   // c5
    {523, 1500},   // c5
    {698, 1500},   // f5
    {784, 1500},   // g5
    {932, 1500},   // a#5
    {880, 12000},  // a5
    {0, 3000},     // p5
    {523, 1500},   // c5
    {523, 1500},   // c5
    {523, 1500},   // c5
    {698, 1500},   // f5
    {784, 1500},   // g5
    {932, 1500},   // a#5
    {880, 1500},   // a5
    {698, 1500},   // f5
    {0, 1500},     // p5
    {784, 1500},   // g5
    {932, 1500},   // a#5
    {880, 3000},   // a5
    {698, 1500},   // f5
    {0, 1500},     // p5
    {784, 1500},   // g5
    {932, 1500},   // a#5
    {880, 3000},   // a5
    {698, 1500},   // f5
    {0, 1500},     // p5
    {880, 1500},   // a5
    {1047, 1500},  // c6
    {1175, 12000}, // d6
};
const note_t AdamsFamily[] = {
    {1047, 1875},  // c6
    {1397, 3750},  // f6
    {1760, 1875},  // a6
    {1397, 3750},  // f6
    {1047, 1875},  // c6
    {988, 3750},   // b5
    {1568, 7500},  // g6
    {1397, 1875},  // f6
    {1319, 3750},  // e6
    {1568, 1875},  // g6
    {1319, 3750},  // e6
    {659, 1875},   // e5
    {880, 3750},   // a5
    {1397, 7500},  // f6
    {1047, 1875},  // c6
    {1397, 3750},  // f6
    {1760, 1875},  // a6
    {1397, 3750},  // f6
    {1047, 1875},  // c6
    {988, 3750},   // b5
    {1568, 7500},  // g6
    {1397, 1875},  // f6
    {1319, 3750},  // e6
    {1047, 1875},  // c6
    {1175, 3750},  // d6
    {1319, 1875},  // e6
    {1397, 15000}, // f6
    {1047, 1875},  // c6
    {1175, 1875},  // d6
    {1319, 1875},  // e6
    {1397, 1875},  // f6
    {0, 15000},    // p5
    {1175, 1875},  // d6
    {1319, 1875},  // e6
    {1480, 1875},  // f#6
    {1568, 1875},  // g6
    {0, 15000},    // p5
    {1175, 1875},  // d6
    {1319, 1875},  // e6
    {1480, 1875},  // f#6
    {1568, 1875},  // g6
    {0, 3750},     // p5
    {1175, 1875},  // d6
    {1319, 1875},  // e6
    {1480, 1875},  // f#6
    {1568, 1875},  // g6
    {0, 3750},     // p5
    {1047, 3750},  // c6
    {1319, 1875},  // e6
    {1397, 15000}, // f6
};
const note_t AirwolfTheme[] = {
    {659, 6000},  // e5
    {880, 1500},  // a5
    {988, 1500},  // b5
    {1175, 1500}, // d6
    {1319, 6000}, // e6
    {1568, 1500}, // g6
    {1480, 1500}, // f#6
    {1175, 1500}, // d6
    {1319, 6000}, // e6
    {1568, 1500}, // g6
    {1480, 1500}, // f#6
    {1175, 1500}, // d6
    {1319, 6000}, // e6
    {1175, 3000}, // d6
    {1480, 1500}, // f#6
    {988, 6000},  // b5
    {880, 6000},  // a5
    {784, 3000},  // g5
    {880, 1500},  // a5
    {740, 3000},  // f#5
    {587, 1500},  // d5
    {784, 6000},  // g5
    {1047, 1500}, // c6
    {1175, 1500}, // d6
    {1397, 1500}, // f6
    {784, 6000},  // g5
    {1047, 1500}, // c6
    {1976, 1500}, // b6
    {1397, 1500}, // f6
    {1568, 6000}, // g6
    {1047, 1500}, // c6
    {1976, 1500}, // b6
    {1397, 1500}, // f6
    {1568, 6000}, // g6
    {1397, 3000}, // f6
    {880, 1500},  // a5
    {1175, 6000}, // d6
    {1047, 6000}, // c6
    {988, 3000},  // b5
    {1175, 1500}, // d6
    {880, 3000},  // a5
    {698, 1500},  // f5
    {1568, 6000}, // g6
    {1047, 1500}, // c6
    {1175, 1500}, // d6
    {1397, 1500}, // f6
    {1568, 6000}, // g6
    {1047, 1500}, // c6
    {1976, 1500}, // b6
    {1397, 1500}, // f6
};
const note_t ATeamTheme[] = {
    {1397, 2400}, // f6
    {1397, 1200}, // f6
    {1047, 2400}, // c6
    {1397, 9600}, // f6
    {0, 2400},    // p5
    {932, 2400},  // a#5
    {1047, 4800}, // c6
    {698, 9600},  // f5
    {932, 1200},  // a#5
    {1047, 1200}, // c6
    {1397, 2400}, // f6
    {1047, 2400}, // c6
    {1568, 2400}, // g6
    {1397, 9600}, // f6
    {0, 2400},    // p5
    {1245, 2400}, // d#6
    {1175, 1200}, // d6
    {1047, 1200}, // c6
    {932, 4800},  // a#5
    {1047, 9600}, // c6
    {1397, 2400}, // f6
    {1397, 1200}, // f6
    {1047, 2400}, // c6
    {1397, 9600}, // f6
    {0, 2400},    // p5
    {880, 2400},  // a5
    {932, 4800},  // a#5
    {1047, 2400}, // c6
    {698, 9600},  // f5
    {932, 2400},  // a#5
    {880, 2400},  // a5
    {0, 2400},    // p5
    {698, 2400},  // f5
    {932, 4800},  // a#5
    {880, 4800},  // a5
    {932, 4800},  // a#5
    {880, 4800},  // a5
    {698, 2400},  // f5
    {784, 4800},  // g5
    {698, 4800},  // f5
    {1047, 4800}, // c6
    {932, 4800},  // a#5
    {1245, 4800}, // d#6
    {1397, 9600}, // f6
    {698, 2400},  // f5
};
const note_t BackToTheFuture[] = {
    {0, 3750},    // p5
    {523, 1875},  // c5
    {0, 937},     // p5
    {784, 3750},  // g5
    {0, 937},     // p5
    {1047, 937},  // c6
    {0, 937},     // p5
    {932, 3750},  // a#5
    {0, 937},     // p5
    {880, 937},   // a5
    {0, 937},     // p5
    {784, 937},   // g5
    {0, 937},     // p5
    {880, 1875},  // a5
    {0, 937},     // p5
    {784, 1875},  // g5
    {0, 937},     // p5
    {698, 1875},  // f5
    {0, 937},     // p5
    {784, 15000}, // g5
    {0, 15000},   // p5
    {784, 3750},  // g5
    {0, 937},     // p5
    {523, 3750},  // c5
    {0, 937},     // p5
    {740, 7500},  // f#5
    {0, 937},     // p5
    {784, 468},   // g5
    {0, 468},     // p5
    {880, 468},   // a5
    {0, 468},     // p5
    {784, 1875},  // g5
    {0, 468},     // p5
    {659, 1875},  // e5
    {0, 468},     // p5
    {523, 1875},  // c5
    {0, 468},     // p5
    {740, 3750},  // f#5
    {0, 937},     // p5
    {784, 468},   // g5
    {0, 468},     // p5
    {880, 468},   // a5
    {0, 468},     // p5
    {784, 1875},  // g5
    {0, 468},     // p5
    {587, 1875},  // d5
    {0, 468},     // p5
    {784, 1875},  // g5
    {0, 468},     // p5
    {1175, 1875}, // d6
    {0, 468},     // p5
    {1175, 3750}, // d6
    {0, 937},     // p5
    {1109, 3750}, // c#6
    {0, 937},     // p5
    {988, 468},   // b5
    {0, 468},     // p5
    {1109, 468},  // c#6
    {0, 468},     // p5
    {1175, 7500}, // d6
};
const note_t BeatlesAllyouneedislove[] = {
    {1568, 2142}, // g6
    {0, 1071},    // p5
    {1568, 2142}, // g6
    {1568, 2142}, // g6
    {0, 1071},    // p5
    {1568, 2142}, // g6
    {0, 1071},    // p5
    {0, 1071},    // p5
    {1568, 2142}, // g6
    {0, 8571},    // p5
    {1976, 1071}, // b6
    {0, 1071},    // p5
    {0, 1071},    // p5
    {1865, 2142}, // a#6
    {1760, 2142}, // a6
    {1661, 1071}, // g#6
    {1568, 2142}, // g6
    {0, 4285},    // p5
    {1568, 2142}, // g6
    {0, 1071},    // p5
    {1568, 2142}, // g6
    {1568, 2142}, // g6
    {0, 1071},    // p5
    {1568, 2142}, // g6
    {0, 2142},    // p5
    {1568, 2142}, // g6
    {0, 4285},    // p5
    {0, 2142},    // p5
    {1976, 2142}, // b6
    {0, 1071},    // p5
    {1865, 2142}, // a#6
    {1760, 1071}, // a6
    {1661, 2142}, // g#6
    {1568, 2142}, // g6
    {0, 4285},    // p5
    {1568, 2142}, // g6
    {0, 1071},    // p5
    {1568, 2142}, // g6
    {1661, 2142}, // g#6
    {0, 1071},    // p5
    {1661, 2142}, // g#6
    {0, 2142},    // p5
    {1760, 4285}, // a6
    {0, 4285},    // p5
    {1319, 8571}, // e6
    {0, 4285},    // p5
    {1175, 2142}, // d6
    {1319, 1071}, // e6
    {1175, 2142}, // d6
    {0, 2142},    // p5
    {1047, 1071}, // c6
    {1047, 2142}, // c6
};
const note_t BeeGeesStayingAlive[] = {
    {1047, 1500},  // c6
    {1047, 1500},  // c6
    {1047, 1500},  // c6
    {1047, 1500},  // c6
    {1047, 1500},  // c6
    {932, 1500},   // a#5
    {932, 1500},   // a#5
    {932, 3000},   // a#5
    {831, 1500},   // g#5
    {831, 1500},   // g#5
    {831, 1500},   // g#5
    {831, 1500},   // g#5
    {784, 1500},   // g5
    {784, 1500},   // g5
    {784, 3000},   // g5
    {831, 1500},   // g#5
    {831, 1500},   // g#5
    {831, 6000},   // g#5
    {784, 1500},   // g5
    {831, 1500},   // g#5
    {831, 1500},   // g#5
    {831, 6000},   // g#5
    {1047, 6000},  // c6
    {932, 6000},   // a#5
    {831, 6000},   // g#5
    {784, 6000},   // g5
    {784, 1500},   // g5
    {831, 1500},   // g#5
    {831, 1500},   // g#5
    {831, 6000},   // g#5
    {784, 1500},   // g5
    {831, 1500},   // g#5
    {831, 1500},   // g#5
    {831, 6000},   // g#5
    {1047, 6000},  // c6
    {932, 6000},   // a#5
    {831, 6000},   // g#5
    {784, 6000},   // g5
    {831, 1500},   // g#5
    {831, 1500},   // g#5
    {831, 1500},   // g#5
    {1047, 12000}, // c6
    {932, 12000},  // a#5
    {1047, 1500},  // c6
    {932, 1500},   // a#5
    {831, 24000},  // g#5
    {784, 12000},  // g5
    {1047, 6000},  // c6
    {932, 3000},   // a#5
    {1047, 6000},  // c6
};
const note_t BeethoventhSymphony[] = {
    {784, 1200},  // g5
    {784, 1200},  // g5
    {784, 1200},  // g5
    {622, 1200},  // d#5
    {831, 1200},  // g#5
    {831, 1200},  // g#5
    {831, 1200},  // g#5
    {784, 1200},  // g5
    {1245, 1200}, // d#6
    {1245, 1200}, // d#6
    {1245, 1200}, // d#6
    {1047, 4800}, // c6
    {784, 1200},  // g5
    {784, 1200},  // g5
    {784, 1200},  // g5
    {587, 1200},  // d5
    {831, 1200},  // g#5
    {831, 1200},  // g#5
    {831, 1200},  // g#5
    {784, 1200},  // g5
    {1397, 1200}, // f6
    {1397, 1200}, // f6
    {1397, 1200}, // f6
    {1175, 4800}, // d6
    {1568, 1200}, // g6
    {1568, 1200}, // g6
    {1397, 1200}, // f6
    {1245, 1200}, // d#6
    {622, 1200},  // d#5
    {622, 1200},  // d#5
    {698, 1200},  // f5
    {784, 1200},  // g5
    {1568, 1200}, // g6
    {1568, 1200}, // g6
    {1397, 1200}, // f6
    {1245, 1200}, // d#6
    {622, 1200},  // d#5
    {622, 1200},  // d#5
    {698, 1200},  // f5
    {784, 1200},  // g5
    {1568, 1200}, // g6
    {1568, 1200}, // g6
    {1397, 1200}, // f6
    {1245, 2400}, // d#6
    {0, 4800},    // p5
    {1047, 2400}, // c6
    {0, 4800},    // p5
    {1568, 9600}, // g6
};
const note_t BeverlyHillsCopTheme[] = {
    {1319, 4800}, // e6
    {1568, 2400}, // g6
    {1319, 2400}, // e6
    {1319, 1200}, // e6
    {1760, 2400}, // a6
    {1319, 2400}, // e6
    {1175, 2400}, // d6
    {1319, 4800}, // e6
    {1976, 2400}, // b6
    {1319, 2400}, // e6
    {1319, 1200}, // e6
    {2093, 2400}, // c7
    {1976, 2400}, // b6
    {1568, 2400}, // g6
    {1319, 2400}, // e6
    {1976, 2400}, // b6
    {2637, 2400}, // e7
    {1319, 1200}, // e6
    {1175, 2400}, // d6
    {1175, 1200}, // d6
    {988, 2400},  // b5
    {1480, 2400}, // f#6
    {1319, 4800}, // e6
};
const note_t BlurSong[] = {
    {1047, 2400}, // c6
    {1047, 2400}, // c6
    {1047, 1200}, // c6
    {1047, 1200}, // c6
    {932, 2400},  // a#5
    {932, 2400},  // a#5
    {932, 2400},  // a#5
    {932, 1200},  // a#5
    {932, 1200},  // a#5
    {1245, 2400}, // d#6
    {1245, 2400}, // d#6
    {1397, 2400}, // f6
    {1568, 4800}, // g6
    {1568, 2400}, // g6
    {1568, 2400}, // g6
    {1568, 2400}, // g6
    {0, 2400},    // p5
    {1047, 2400}, // c6
    {1047, 2400}, // c6
    {1047, 1200}, // c6
    {1047, 1200}, // c6
    {932, 2400},  // a#5
    {932, 2400},  // a#5
    {932, 2400},  // a#5
    {932, 1200},  // a#5
    {932, 1200},  // a#5
    {1245, 2400}, // d#6
    {1245, 2400}, // d#6
    {1397, 2400}, // f6
    {1568, 4800}, // g6
    {1568, 2400}, // g6
    {1568, 2400}, // g6
    {1568, 2400}, // g6
};
const note_t ETTheme[] = {
    {988, 1500},  // b5
    {932, 1500},  // a#5
    {988, 1500},  // b5
    {932, 1500},  // a#5
    {831, 1500},  // g#5
    {698, 3000},  // f5
    {740, 1500},  // f#5
    {932, 1500},  // a#5
    {831, 1500},  // g#5
    {784, 3000},  // g5
    {587, 1500},  // d5
    {740, 6000},  // f#5
    {1976, 1500}, // b6
    {1865, 1500}, // a#6
    {1976, 1500}, // b6
    {1865, 1500}, // a#6
    {1661, 1500}, // g#6
    {1397, 3000}, // f6
    {1480, 1500}, // f#6
    {1865, 1500}, // a#6
    {1661, 1500}, // g#6
    {1568, 3000}, // g6
    {1175, 1500}, // d6
    {1480, 6000}, // f#6
    {1245, 1500}, // d#6
    {1175, 1500}, // d6
    {1245, 1500}, // d#6
    {1175, 1500}, // d6
    {1047, 1500}, // c6
    {880, 3000},  // a5
    {932, 1500},  // a#5
    {1245, 1500}, // d#6
    {1568, 1500}, // g6
    {1760, 3000}, // a6
    {1480, 1500}, // f#6
    {1760, 6000}, // a6
    {0, 1500},    // p5
    {0, 750},     // p5
    {1760, 3000}, // a6
    {1480, 1500}, // f#6
    {1760, 6000}, // a6
};
const note_t FlashdanceWhatAFeeling[] = {
    {698, 2142},  // f5
    {880, 2142},  // a5
    {932, 4285},  // a#5
    {932, 4285},  // a#5
    {0, 17142},   // p5
    {0, 2142},    // p5
    {698, 2142},  // f5
    {880, 2142},  // a5
    {1047, 4285}, // c6
    {932, 2142},  // a#5
    {932, 2142},  // a#5
    {698, 2142},  // f5
    {784, 4285},  // g5
    {0, 17142},   // p5
    {698, 2142},  // f5
    {880, 2142},  // a5
    {932, 4285},  // a#5
    {932, 4285},  // a#5
    {1047, 4285}, // c6
    {932, 2142},  // a#5
    {880, 2142},  // a5
    {932, 4285},  // a#5
    {932, 4285},  // a#5
    {1047, 4285}, // c6
    {698, 2142},  // f5
    {698, 2142},  // f5
    {698, 17142}, // f5
};
const note_t FormulaTheme[] = {
    {880, 7500},  // a5
    {0, 3750},    // p5
    {880, 1875},  // a5
    {988, 1875},  // b5
    {1047, 3750}, // c6
    {988, 1875},  // b5
    {880, 1875},  // a5
    {784, 1875},  // g5
    {880, 1875},  // a5
    {988, 3750},  // b5
    {659, 30000}, // e5
    {880, 7500},  // a5
    {0, 3750},    // p5
    {880, 1875},  // a5
    {988, 1875},  // b5
    {1047, 3750}, // c6
    {988, 1875},  // b5
    {880, 1875},  // a5
    {784, 1875},  // g5
    {880, 1875},  // a5
    {988, 3750},  // b5
    {659, 15000}, // e5
    {0, 7500},    // p5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
    {659, 1875},  // e5
};
const note_t FriendsTheme[] = {
    {1109, 6000},  // c#6
    {1109, 3000},  // c#6
    {988, 3000},   // b5
    {880, 3000},   // a5
    {784, 6000},   // g5
    {784, 6000},   // g5
    {880, 3000},   // a5
    {988, 6000},   // b5
    {880, 6000},   // a5
    {0, 6000},     // p5
    {1109, 6000},  // c#6
    {1109, 3000},  // c#6
    {988, 3000},   // b5
    {880, 3000},   // a5
    {784, 6000},   // g5
    {784, 6000},   // g5
    {880, 3000},   // a5
    {988, 6000},   // b5
    {880, 6000},   // a5
    {659, 6000},   // e5
    {659, 3000},   // e5
    {880, 6000},   // a5
    {988, 6000},   // b5
    {1175, 6000},  // d6
    {1109, 3000},  // c#6
    {988, 6000},   // b5
    {880, 6000},   // a5
    {880, 3000},   // a5
    {659, 6000},   // e5
    {880, 6000},   // a5
    {988, 6000},   // b5
    {988, 12000},  // b5
    {0, 12000},    // p5
    {659, 3000},   // e5
    {880, 6000},   // a5
    {988, 6000},   // b5
    {1175, 6000},  // d6
    {1109, 3000},  // c#6
    {988, 6000},   // b5
    {880, 6000},   // a5
    {1175, 3000},  // d6
    {1109, 6000},  // c#6
    {988, 6000},   // b5
    {880, 6000},   // a5
    {1319, 12000}, // e6
};
const note_t GhostbustersTheme[] = {
    {554, 1200},  // c#5
    {0, 1200},    // p5
    {554, 2400},  // c#5
    {659, 1200},  // e5
    {698, 1200},  // f5
    {831, 1200},  // g#5
    {0, 1200},    // p5
    {988, 2400},  // b5
    {0, 2400},    // p5
    {932, 2400},  // a#5
    {0, 1200},    // p5
    {554, 1200},  // c#5
    {0, 1200},    // p5
    {554, 2400},  // c#5
    {659, 1200},  // e5
    {698, 1200},  // f5
    {831, 1200},  // g#5
    {0, 1200},    // p5
    {988, 1200},  // b5
    {0, 1200},    // p5
    {988, 1200},  // b5
    {0, 1200},    // p5
    {932, 4800},  // a#5
    {0, 2400},    // p5
    {1109, 1200}, // c#6
    {1109, 1200}, // c#6
    {1397, 2400}, // f6
    {1109, 1200}, // c#6
    {0, 1200},    // p5
    {1245, 2400}, // d#6
    {988, 1200},  // b5
};
const note_t HappyDaysTheme[] = {
    {523, 3750},  // c5
    {698, 3750},  // f5
    {880, 3750},  // a5
    {1047, 3750}, // c6
    {1175, 1875}, // d6
    {1319, 3750}, // e6
    {1175, 3750}, // d6
    {0, 3750},    // p5
    {587, 3750},  // d5
    {784, 3750},  // g5
    {932, 3750},  // a#5
    {1175, 3750}, // d6
    {1319, 1875}, // e6
    {1397, 3750}, // f6
    {1319, 3750}, // e6
    {0, 3750},    // p5
    {659, 3750},  // e5
    {880, 3750},  // a5
    {1109, 3750}, // c#6
    {1319, 3750}, // e6
    {1397, 1875}, // f6
    {1568, 3750}, // g6
    {1397, 3750}, // f6
    {0, 3750},    // p5
    {1175, 1875}, // d6
    {1175, 3750}, // d6
    {1319, 3750}, // e6
    {0, 3750},    // p5
    {1175, 3750}, // d6
    {1319, 3750}, // e6
    {0, 3750},    // p5
    {1319, 1875}, // e6
    {1319, 1875}, // e6
    {1319, 3750}, // e6
    {1397, 3750}, // f6
    {1480, 1875}, // f#6
    {1568, 7500}, // g6
    {0, 7500},    // p5
    {1397, 3750}, // f6
    {1397, 3750}, // f6
    {1568, 1875}, // g6
    {1760, 3750}, // a6
    {1568, 3750}, // g6
    {1397, 3750}, // f6
    {1568, 1875}, // g6
    {1397, 3750}, // f6
    {1175, 3750}, // d6
    {1175, 3750}, // d6
    {1175, 1875}, // d6
    {1319, 7500}, // e6
};
const note_t InspectorGadgetTheme[] = {
    {587, 1071},  // d5
    {659, 2142},  // e5
    {698, 2142},  // f5
    {784, 1071},  // g5
    {880, 2142},  // a5
    {698, 2142},  // f5
    {831, 2142},  // g#5
    {659, 2142},  // e5
    {784, 2142},  // g5
    {698, 2142},  // f5
    {587, 1071},  // d5
    {659, 1071},  // e5
    {698, 1071},  // f5
    {784, 1071},  // g5
    {880, 2142},  // a5
    {1175, 2142}, // d6
    {2217, 4285}, // c#7
    {0, 4285},    // p5
    {587, 1071},  // d5
    {659, 1071},  // e5
    {698, 1071},  // f5
    {784, 1071},  // g5
    {880, 2142},  // a5
    {698, 2142},  // f5
    {831, 2142},  // g#5
    {659, 2142},  // e5
};
const note_t JamesBondTheme[] = {
    {659, 2142},  // e5
    {740, 1071},  // f#5
    {740, 1071},  // f#5
    {740, 2142},  // f#5
    {740, 4285},  // f#5
    {659, 2142},  // e5
    {659, 2142},  // e5
    {659, 2142},  // e5
    {659, 2142},  // e5
    {784, 1071},  // g5
    {784, 1071},  // g5
    {784, 2142},  // g5
    {784, 4285},  // g5
    {740, 2142},  // f#5
    {740, 2142},  // f#5
    {740, 2142},  // f#5
    {659, 2142},  // e5
    {740, 1071},  // f#5
    {740, 1071},  // f#5
    {740, 2142},  // f#5
    {740, 4285},  // f#5
    {659, 2142},  // e5
    {659, 2142},  // e5
    {659, 2142},  // e5
    {659, 2142},  // e5
    {784, 1071},  // g5
    {784, 1071},  // g5
    {784, 2142},  // g5
    {784, 4285},  // g5
    {740, 2142},  // f#5
    {698, 2142},  // f5
    {659, 2142},  // e5
    {1245, 2142}, // d#6
    {1175, 8571}, // d6
    {988, 2142},  // b5
    {880, 2142},  // a5
    {988, 17142}, // b5
};
const note_t MissionImpossi[] = {
    {1245, 1666},  // d#6
    {1047, 1666},  // c6
    {784, 13333},  // g5
    {1245, 1666},  // d#6
    {1047, 1666},  // c6
    {740, 13333},  // f#5
    {1245, 1666},  // d#6
    {1047, 1666},  // c6
    {698, 13333},  // f5
    {622, 1666},   // d#5
    {698, 1666},   // f5
    {0, 13333},    // p5
    {831, 1666},   // g#5
    {784, 1666},   // g5
    {1480, 13333}, // f#6
    {831, 1666},   // g#5
    {784, 1666},   // g5
    {1397, 13333}, // f6
    {831, 1666},   // g#5
    {784, 1666},   // g5
    {1319, 13333}, // e6
    {1245, 1666},  // d#6
    {1175, 1666},  // d6
    {0, 6666},     // p5
};
const note_t Muppet[] = {
    {1047, 2400}, // c6
    {1047, 2400}, // c6
    {880, 2400},  // a5
    {988, 2400},  // b5
    {880, 1200},  // a5
    {988, 2400},  // b5
    {784, 2400},  // g5
    {0, 2400},    // p5
    {1047, 2400}, // c6
    {1047, 2400}, // c6
    {880, 2400},  // a5
    {988, 1200},  // b5
    {880, 1200},  // a5
    {0, 1200},    // p5
    {784, 2400},  // g5
    {0, 2400},    // p5
    {659, 2400},  // e5
    {659, 2400},  // e5
    {784, 2400},  // g5
    {698, 2400},  // f5
    {659, 1200},  // e5
    {698, 2400},  // f5
    {1047, 1200}, // c6
    {523, 1200},  // c5
    {587, 1200},  // d5
    {659, 2400},  // e5
    {659, 1200},  // e5
    {659, 1200},  // e5
    {0, 1200},    // p5
    {659, 1200},  // e5
    {784, 2400},  // g5
    {0, 4800},    // p5
    {1047, 2400}, // c6
    {1047, 2400}, // c6
    {880, 2400},  // a5
    {988, 2400},  // b5
    {880, 1200},  // a5
    {988, 2400},  // b5
    {784, 2400},  // g5
    {0, 2400},    // p5
    {1047, 2400}, // c6
    {1047, 2400}, // c6
    {880, 2400},  // a5
    {988, 1200},  // b5
    {880, 2400},  // a5
    {784, 2400},  // g5
    {0, 2400},    // p5
    {659, 2400},  // e5
    {659, 2400},  // e5
    {784, 2400},  // g5
    {698, 2400},  // f5
    {659, 1200},  // e5
    {698, 2400},  // f5
    {1047, 1200}, // c6
    {523, 1200},  // c5
    {587, 1200},  // d5
    {659, 2400},  // e5
    {659, 1200},  // e5
    {587, 2400},  // d5
    {587, 1200},  // d5
    {523, 2400},  // c5
};
const note_t TheSimpsons[] = {
    {1047, 3750}, // c6
    {1319, 3750}, // e6
    {1480, 3750}, // f#6
    {1760, 1875}, // a6
    {1568, 3750}, // g6
    {1319, 3750}, // e6
    {1047, 3750}, // c6
    {880, 1875},  // a5
    {740, 1875},  // f#5
    {740, 1875},  // f#5
    {740, 1875},  // f#5
    {784, 7500},  // g5
    {0, 1875},    // p5
    {0, 1875},    // p5
    {740, 1875},  // f#5
    {740, 1875},  // f#5
    {740, 1875},  // f#5
    {784, 1875},  // g5
    {932, 3750},  // a#5
    {1047, 1875}, // c6
    {1047, 1875}, // c6
    {1047, 1875}, // c6
    {1047, 3750}, // c6
};
const note_t SWEnd[] = {
    {523, 5333},   // c5
    {698, 10666},  // f5
    {784, 5333},   // g5
    {831, 1333},   // g#5
    {932, 1333},   // a#5
    {831, 10666},  // g#5
    {523, 5333},   // c5
    {523, 2666},   // c5
    {698, 5333},   // f5
    {784, 2666},   // g5
    {831, 2666},   // g#5
    {523, 2666},   // c5
    {831, 1333},   // g#5
    {523, 1333},   // c5
    {1047, 1333},  // c6
    {932, 10666},  // a#5
    {523, 5333},   // c5
    {698, 5333},   // f5
    {784, 2666},   // g5
    {831, 2666},   // g#5
    {698, 1333},   // f5
    {1047, 2666},  // c6
    {831, 1333},   // g#5
    {1397, 10666}, // f6
    {698, 5333},   // f5
    {831, 1333},   // g#5
    {784, 1333},   // g5
    {698, 1333},   // f5
    {1047, 5333},  // c6
    {1047, 1333},  // c6
    {831, 1333},   // g#5
    {698, 1333},   // f5
    {523, 5333},   // c5
    {523, 1333},   // c5
    {523, 1333},   // c5
    {523, 1333},   // c5
    {698, 5333},   // f5
    {698, 1333},   // f5
    {698, 1333},   // f5
    {698, 1333},   // f5
    {698, 5333},   // f5
};
const note_t Superman[] = {
    {587, 1500},   // d5
    {587, 1500},   // d5
    {587, 1500},   // d5
    {784, 1500},   // g5
    {0, 750},      // p6
    {784, 1500},   // g5
    {1175, 6000},  // d6
    {0, 1500},     // p6
    {1175, 1500},  // d6
    {1319, 1500},  // e6
    {1175, 1500},  // d6
    {1047, 1500},  // c6
    {1175, 12000}, // d6
    {0, 1500},     // p6
    {587, 1500},   // d5
    {587, 1500},   // d5
    {587, 1500},   // d5
    {784, 1500},   // g5
    {0, 750},      // p6
    {784, 1500},   // g5
    {1175, 6000},  // d6
    {1175, 1500},  // d6
    {1175, 1500},  // d6
    {1319, 1500},  // e6
    {1047, 1500},  // c6
    {784, 1500},   // g5
    {1319, 1500},  // e6
    {1175, 6000},  // d6
    {0, 3000},     // p6
    {784, 1500},   // g5
    {784, 1500},   // g5
    {784, 1500},   // g5
    {1480, 6000},  // f#6
    {1175, 3000},  // d6
    {784, 1500},   // g5
    {784, 1500},   // g5
    {784, 1500},   // g5
    {1480, 6000},  // f#6
    {1175, 3000},  // d6
    {784, 1500},   // g5
    {784, 1500},   // g5
    {784, 1500},   // g5
    {1480, 1500},  // f#6
    {1319, 1500},  // e6
    {1480, 1500},  // f#6
    {1568, 6000},  // g6
    {784, 1500},   // g5
    {784, 1500},   // g5
    {784, 1500},   // g5
    {784, 6000},   // g5
};
const note_t werock[] = {
    {784, 1875},  // g5
    {784, 1875},  // g5
    {784, 1875},  // g5
    {622, 3750},  // d#5
    {523, 3750},  // c5
    {698, 3750},  // f5
    {698, 3750},  // f5
    {698, 1875},  // f5
    {880, 1875},  // a5
    {880, 1875},  // a5
    {932, 1875},  // a#5
    {1047, 1875}, // c6
    {932, 1875},  // a#5
    {932, 1875},  // a#5
    {932, 1875},  // a#5
    {698, 3750},  // f5
    {622, 3750},  // d#5
    {784, 3750},  // g5
    {784, 3750},  // g5
    {784, 1875},  // g5
    {698, 1875},  // f5
    {698, 1875},  // f5
    {784, 1875},  // g5
    {698, 1875},  // f5
    {784, 1875},  // g5
    {784, 1875},  // g5
    {784, 1875},  // g5
    {622, 3750},  // d#5
    {523, 3750},  // c5
    {698, 3750},  // f5
    {698, 3750},  // f5
    {698, 1875},  // f5
    {880, 1875},  // a5
    {880, 1875},  // a5
    {932, 1875},  // a#5
    {1047, 1875}, // c6
    {932, 1875},  // a#5
    {932, 1875},  // a#5
    {932, 1875},  // a#5
    {698, 3750},  // f5
    {622, 3750},  // d#5
    {784, 3750},  // g5
    {784, 3750},  // g5
    {784, 1875},  // g5
    {698, 1875},  // f5
    {698, 1875},  // f5
};
uint16_t num_tunes = 24;
const note_t *tunes[] = {&SquadronTheme[0],
                         &AdamsFamily[0],
                         &AirwolfTheme[0],
                         &ATeamTheme[0],
                         &BackToTheFuture[0],
                         &BeatlesAllyouneedislove[0],
                         &BeeGeesStayingAlive[0],
                         &BeethoventhSymphony[0],
                         &BeverlyHillsCopTheme[0],
                         &BlurSong[0],
                         &ETTheme[0],
                         &FlashdanceWhatAFeeling[0],
                         &FormulaTheme[0],
                         &FriendsTheme[0],
                         &GhostbustersTheme[0],
                         &HappyDaysTheme[0],
                         &InspectorGadgetTheme[0],
                         &JamesBondTheme[0],
                         &MissionImpossi[0],
                         &Muppet[0],
                         &TheSimpsons[0],
                         &SWEnd[0],
                         &Superman[0],
                         &werock[0]};
const uint16_t num_notes[] = {39, 50, 50, 45, 60, 52, 50, 48, 23, 33, 41, 27,
                              39, 45, 31, 50, 26, 37, 24, 61, 23, 41, 50, 46};