# MIDI Implementation

## Channel Voice Messages

|MIDI Message|HEX Code|Description|Compatibility|
|:-|:-|:-|:-:|
|NOTE ON|9nH kk vv|MIDI channel n(0-15) note ON #kk(0-127), velocity vv(1-127).<br>vv=0 means NOTE OFF|MIDI|
|NOTE OFF|8nH kk xx|MIDI channel n(0-15) note OFF #kk(0-127), xx is ignored.|MIDI|
|PITCH BEND|EnH bl bh|Pitch bend as specified by bh\|bl (14 bits).<br>Maximum swing is +/- 2 tone (power-up).<br>Can be changed using « Pitch bend sensitivity ».<br>Center position is 00H 40H.|GM|
|PROGRAM<br>CHANGE|CnH pp|Program (patch) change.<br>Specific action on channel 10 (n=9) : select drumset.|GM/GS|
|CHANNEL<br>AFTERTOUCH|DnH vv|vv pressure value|MIDI|
|CTRL 00|BnH 00H cc|Bank select MSB|GS|
|CTRL 05|BnH 01H cc|Modulation wheel|MIDI|
|CTRL 05|BnH 02H cc|Breath controller|MIDI|
|CTRL 05|BnH 04H cc|Foot controller|MIDI|
|CTRL 06|BnH 06H cc|Data entry : provides data to RPN|MIDI|
|CTRL 07|BnH 07H cc|Volume|MIDI|
|CTRL 10|BnH 0AH cc|Pan|MIDI|
|CTRL 11|BnH 0BH cc|Expression|MIDI/GM|
|CTRL 32|BnH 20H cc|Bank select LSB|GS|
|CTRL 64|BnH 40H cx|Sustain (damper) pedal|MIDI|
|CTRL 91|BnH 5BH vv|Reverb send level vv=00H to 7FH|GS|
|CTRL 93|BnH 5DH vv|Chorus send level vv=00H to 7FH|GS|
|CTRL 96|BnH 60H xx|Increase RPN/NRPN value|MIDI|
|CTRL 97|BnH 61H xx|Decrease RPN/NRPN value|MIDI|
|CTRL 98|BnH 62H xx|NRPN LSB|MIDI|
|CTRL 99|BnH 63H xx|NRPN MSB|MIDI|
|CTRL 100|BnH 64H vv|RPN LSB|MIDI|
|CTRL 101|BnH 65H vv|RPN MSB|MIDI|
|CTRL 120|BnH 78H xx|All sounds off (abrupt stop of sound on channel n)|MIDI|
|CTRL 121|BnH 79H xx|Reset all controllers|MIDI|
|CTRL 123|BnH 7BH xx|All notes off|MIDI|
|CTRL 124|BnH 7CH xx|Omni off.<br>The same processing will be carried out as when « All Sounds Off » is received.|MIDI|
|CTRL 125|BnH 7DH xx|Omni on.<br>The same processing will be carried out as when « All Sounds Off » is received.|MIDI|
|CTRL 126|BnH 7EH xx|Mono on.<br>The same processing will be carried out as when « All Notes Off » is received.|MIDI|
|CTRL 127|BnH 7FH xx|Poly on.<br>The same processing will be carried out as when « All Notes Off » is received.|MIDI|
|RPN 0000H|BnH 65H 00H 64H 00H 06H vv|Pitch bend sensitivity in semitones|MIDI/GM|
|RPN 0002H|BnH 65H 00H 64H 02H 06H vv|Coarse tuning in half-tones (vv=00H -64, vv=40H 0, vv=7FH +64)|MIDI|

## System Exclusive Messages

|Manufacturer ID|HEX Code|Description|Compatibility|
|:-|:-|:-|:-:|
|Universal Non-Real Time|F0H 7EH 7FH 09H 01H F7H|General MIDI Mode On|GM|
|Universal Non-Real Time|F0H 7EH 7FH 09H 02H F7H|General MIDI Mode Off|GM|
|Roland Corporation|F0H 41H xx 42H 12H 40H 00H 7FH 00H xx F7H|Roland GS Mode On|GS|
|Roland Corporation|F0H 41H xx 42H 12H 40H 00H 7FH 7FH xx F7H|Roland GS Mode Off|GS|