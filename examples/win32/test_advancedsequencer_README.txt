test_advancedsequencer is a console based MIDI file player: it demonstrates the features of the class AdvancedSequencer.
When the program starts, it invites you to type 'help' to get a list of all available commands: you can load midifiles, play them, mute and solo tracks, change the tempo and many other things...

For compiling test_advancedsequencer.cpp and jdksmidi_test_drvwin32.cpp files you must:
- Set as search directory (for header files):
        jdksmidi\include
- Link with libraries:
       jdksmidi (in this directory)
       winmm (windows multimedia library: it should be already in your search paths).

At the start, the 

NC