README for test_win32_player.cpp by Nicola Cassetta

The program is a GUI based midifile player for Windows.

To maintain maximum generality I made this example using the Windows API, but you may want to use your favorite UI toolkit instead.
The example shows the use of classes AdvancedSequencer, MIDISequencerGUIEventNotifierWin32 and SMPTE.
First, a window is created, then the GUI notifier and finally the sequencer. When the sequencer starts playing, the notifier sends
messages to the window, which accordingly updates the various text boxes. In addition, a timer is created, which updates the SMPTE
box (this must be done at fast regular intervals, so we couldn't use the notifier; you could also decide not to use a notifier at all
and update all interface in this way, i.e. in a separate task) The WindowsProcedure handles messages (system, notifier and timer).

I built the application using the "Win32 GUI Project" template of CodeBlocks: however, this is the resultimg GCC command line
(of course you must substitute your own paths):

mingw32-g++.exe -Wall -DWIN32 -g -I E:\Github\jdksmidi\include -c E:\Github\jdksmidi\examples\win32\test_win32.cpp -o E:\Github\jdksmidi\examples\win32\test_win32\test_win32.o

for compiling (should give some warning on #pragma directives ignored)

mingw32-g++.exe  -o E:\Github\jdksmidi\examples\win32\test_win32.exe E:\Github\jdksmidi\examples\win32\test_win32.o   -static-libgcc -static-libstdc++  E:\Github\jdksmidi\lib\libjdksmidi.a -lgdi32 -luser32 -lkernel32 -lcomctl32 -lwinmm -lcomdlg32 

for linking.

If in trouble, you must:

-set the compiler paths in order to see the directory jdksemidi\include 
-set the linker to use the libraries \lib\libjdksmidi.a (after you have built it! : should be in the jdksmidi\lib folder) and the Windows
 libraries gdi32, user32, kernel32, comctl32 winmm comdlg32.

If you get an "Undefined reference to WinMain" error from the linker, I'm sorry but I can't help you:
I also got it and fixed things with Codeblocks (but I don't know why!!!). Check all the flag options.

ISSUES: I tested it with various commercial GM and GS MIDIFiles and all Went OK. Actually problems arise only in files which make extensive use of Sysex: when you push the back or forward button while playng, or start playback from the middle, the sequencer performs a search of all preceding PC, CC and Sysex messages to set the appropriate values. It pauses 10 msecs for every Sysex, so, if in your file there are, say, 100 Sysex, when you start playing from the middle there is a 1 second pause before the sequencer starts.   

NC