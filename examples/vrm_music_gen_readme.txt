

  VRM Music Generator  based on  libJDKSmidi C++ MIDI Library

  version 1.23  from February 2011
  
  Copyright (C) 2010 V.R.Madgazin
  www.vmgames.com
  vrm@vmgames.com
  

1. Launching.


  "vrm_music_gen"

  or with equivalent default arguments

  "vrm_music_gen -se 3 -in 25 -n0 0 -n1 14 -tr 48 -di 1 -ch 1 -md 43 -sd 1 -nd 0.5 -de 1.5 -pr 4"


2. Arguments.


  "-key key_value", key_value "I" = Integer number, "F" = Float number


  "-se I" random seed value (default 3); any of 32 bit integer

  "-in I" midi melodic instrument number (25), see section 4; 0...127

  "-n0 I" min index of notes array (0), see section 3; 0 for C-dur, 5 for A-moll; 0...70

  "-n1 I" max index of notes array (14); add N*7 to min index for N octaves diapason; 0...70

  "-tr I" notes transposition (48); 0...127
          abs. note number is calculated on formula ( notes_table[index] + transposition )

  "-di I" switch for discretization of all time intervals in note duration unit (1); 0...1

  "-ch I" channel number (1 for melodic instruments); 1...16
          channel number 10 for percussion instruments, which correspond to abs. note number
  
  "-md F" total music duration in seconds (43); 0.001...3600...

  "-sd F" temporal section of music in seconds (1); 0.001...3600...

  "-nd F" note duration in seconds (0.5); 0.001...10...

  "-de F" average notes number per note duration (1.5); 0.1...10...

  "-pr I" last note prolongation time in note duration time (4); 0...4...


3. Notes array.


const int MAX_INDEX = 70;
int notes_table[MAX_INDEX+1] = // notes number array: all "white" notes in 10 octaves
{  0, 2, 4, 5, 7, 9,11,  12,14,16,17,19,21,23,  24,26,28,29,31,33,35,  36,38,40,41,43,45,47, ... };
// 0  1  2  3  4  5  6    7  8  9 10 11 12 13   14 15 16 17 18 19 20   21 22 23 24 25 26 27  index
// C  D  E  F  G  A  B    C  D  E  F  G  A  B    C  D  E  F  G  A  B    C  D  E  F  G  A  B  notes


4. General Midi melodic instruments numbers and names.


    0  Acoustic Grand Piano
    1  Bright Acoustic Piano
    2  Electric Grand Piano
    3  Honky-tonk Piano
    4  Electric Piano 1
    5  Electric Piano 2
    6  Harpsichord
    7  Clavinet
    8  Celesta
    9  Glockenspiel
   10  Music Box
   11  Vibraphone
   12  Marimba
   13  Xylophone
   14  Tubular Bells
   15  Dulcimer
   16  Drawbar Organ
   17  Percussive Organ
   18  Rock Organ
   19  Church Organ
   20  Reed Organ
   21  Accordion
   22  Harmonica
   23  Tango Accordion
   24  Acoustic Guitar (nylon)
   25  Acoustic Guitar (steel)
   26  Electric Guitar (jazz)
   27  Electric Guitar (clean)
   28  Electric Guitar (muted)
   29  Overdriven Guitar
   30  Distortion Guitar
   31  Guitar Harmonics
   32  Acoustic Bass
   33  Electric Bass (finger)
   34  Electric Bass (pick)
   35  Fretless Bass
   36  Slap Bass 1
   37  Slap Bass 2
   38  Synth Bass 1
   39  Synth Bass 2
   40  Violin
   41  Viola
   42  Cello
   43  Contrabass
   44  Tremolo Strings
   45  Pizzicato Strings
   46  Orchestral Harp
   47  Timpani
   48  String Ensemble 1
   49  String Ensemble 2
   50  Synth Strings 1
   51  Synth Strings 2
   52  Choir Aahs
   53  Voice Oohs
   54  Synth Voice
   55  Orchestra Hit
   56  Trumpet
   57  Trombone
   58  Tuba
   59  Muted Trumpet
   60  French Horn
   61  Brass Section
   62  Synth Brass 1
   63  Synth Brass 2
   64  Soprano Sax
   65  Alto Sax
   66  Tenor Sax
   67  Baritone Sax
   68  Oboe
   69  English Horn
   70  Bassoon
   71  Clarinet
   72  Piccolo
   73  Flute
   74  Recorder
   75  Pan Flute
   76  Bottle Blow
   77  Shakuhachi
   78  Whistle
   79  Ocarina
   80  Lead 1 (square)
   81  Lead 2 (sawtooth)
   82  Lead 3 (calliope)
   83  Lead 4 (chiff)
   84  Lead 5 (charang)
   85  Lead 6 (voice)
   86  Lead 7 (fifths)
   87  Lead 8 (bass + lead)
   88  Pad 1 (new age)
   89  Pad 2 (warm)
   90  Pad 3 (polysynth)
   91  Pad 4 (choir)
   92  Pad 5 (bowed)
   93  Pad 6 (metallic)
   94  Pad 7 (halo)
   95  Pad 8 (sweep)
   96  FX 1 (rain)
   97  FX 2 (soundtrack)
   98  FX 3 (crystal)
   99  FX 4 (atmosphere)
  100  FX 5 (brightness)
  101  FX 6 (goblins)
  102  FX 7 (echoes)
  103  FX 8 (sci-fi)
  104  Sitar
  105  Banjo
  106  Shamisen
  107  Koto
  108  Kalimba
  109  Bagpipe
  110  Fiddle
  111  Shanai
  112  Tinkle Bell
  113  Agogo
  114  Steel Drums
  115  Woodblock
  116  Taiko Drum
  117  Melodic Tom
  118  Synth Drum
  119  Reverse Cymbal
  120  Guitar Fret Noise
  121  Breath Noise
  122  Seashore
  123  Bird Tweet
  124  Telephone Ring
  125  Helicopter
  126  Applause
  127  Gunshot


5. General Midi percussion instruments numbers and names.


   35  Acoustic Bass Drum
   36  Bass Drum 1       
   37  Side Kick         
   38  Acoustic Snare    
   39  Hand Clap         
   40  Electric Snare    
   41  Low Floor Tom     
   42  Closed High-Hat   
   43  High Floor Tom    
   44  Pedal High Hat    
   45  Low Tom           
   46  Open High Hat     
   47  Low-Mid Tom       
   48  High-Mid Tom      
   49  Crash Cymbal 1    
   50  High Tom          
   51  Ride Cymbal 1     
   52  Chinese Cymbal    
   53  Ride Bell         
   54  Tambourine        
   55  Splash Cymbal     
   56  Cowbell           
   57  Crash Cymbal 2    
   58  Vibraslap
   59  Ride Cymbal 2   
   60  High Bongo      
   61  Low Bongo       
   62  Mute High Conga 
   63  Open High Conga 
   64  Low Conga       
   65  High Timbale    
   66  Low Timbale     
   67  High Agogo      
   68  Low Agogo       
   69  Cabasa          
   70  Maracas         
   71  Short Whistle   
   72  Long Whistle    
   73  Short Guiro     
   74  Long Guiro      
   75  Claves          
   76  High Wood Block 
   77  Low Wood Block  
   78  Mute Cuica      
   79  Open Cuica      
   80  Mute Triangle   
   81  Open Triangle   


6. License.


  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program;
  if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


7. Project pages.


   http://www.vmgames.com

   http://www.vmgames.com/music/

   http://github.com/vadimrm/jdksmidi

   http://github.com/jdkoftinoff/jdksmidi


   Vadim R. Madgazin  vrm@vmgames.com

