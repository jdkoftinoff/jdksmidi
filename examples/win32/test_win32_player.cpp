/*

  AdvancedSequencer class example for libJDKSmidi C++ MIDI Library
  (Win32 GUI App using MS Windows API)

  Copyright (C) 2013 N.Cassetta
  ncassetta@tiscali.it

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

*/
//
// Copyright (C) 2013 N. Cassetta
// ncassetta@tiscali.it
//


/* A GUI based midifile player for Windows. It uses an AdvancedSequencer class, a SMPTE and a
 * MIDISequencerGUIEventNotifierWin32 updating the GUI.
 */

#include "test_win32_player.h"


// Declare jdks objects
static UINT NotifierMessage = 0;                    // the Windows message id to communicate between notifier and GUI
AdvancedSequencer *sequencer;                       // the sequencer
SMPTE smpte;                                        // milliseconds to smpte converter


// Declare handles to the window controls (In a real application you may wanr to subclass your
// window class and make them local)

HWND hMainWin;                      // our main window
HWND hFileName;                     // filename box
HWND hTime;                         // time box
HWND hTempo;                        // tempo box
HWND hMeas;                         // meas/beat box
HWND hSmpte;                        // smpte box
HWND hMarker;                       // marker box
HWND hTrackNames[16];               // array of boxes for track names
HWND hTrackChans[16];               // array of boxes for track channels
HWND hTrackPrgrs[16];               // array of boxes for track programs
HWND hTrackVols[16];                // array of boxes for track volumes


//
// WinMain
//

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    MSG messages;            // Here messages to the application are saved
    WNDCLASSEX wincl;        // Data structure for the windowclass
    char szClassName[ ] = "Test AdvancedSequencer in WIN32";    // Class name

    // The Window structure
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;        // This function is called by windows
    wincl.style = CS_DBLCLKS;                   // Catch double-clicks
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
                                                // Use default icon and mouse-pointer
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                  // No menu
    wincl.cbClsExtra = 0;                       // No extra bytes after the window class
    wincl.cbWndExtra = 0;                       // Structure or the window instance
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
                                                // Use Windows's default colour as the background of the window

    // Register the window class, and if it fails quit the program
    if (!RegisterClassEx (&wincl))
        return 0;

    // The class is registered, let's create the window
    hMainWin = CreateWindowEx (
           0,                           // Extended possibilites for variation
           szClassName,                 // Classname
           "test WIN32 MIDI player (C) by N. Cassetta",
                                        // Title Text
           WS_OVERLAPPEDWINDOW,         // Default window
           CW_USEDEFAULT,               // Windows decides the position
           CW_USEDEFAULT,               // where the window ends up on the screen
           800,                         // The window width
           600,                         // and height in pixels
           HWND_DESKTOP,                // The window is a child-window to desktop
           NULL,                        // No menu
           hThisInstance,               // Program Instance handler
           NULL                         // No Window Creation data
           );

    // Now create the jdksmidi objects: the GUI notifier and the sequencer (to send messages to the window
    // the notifier needs its handle and the message id)
    MIDISequencerGUIEventNotifierWin32 notifier (
        hMainWin                        // The window handle to which send messages
        );
    NotifierMessage = notifier.GetMsgId();
    sequencer = new AdvancedSequencer( &notifier );

    // Make the window visible on the screen
    ShowWindow (hMainWin, nCmdShow);

    // Run the message loop. It will run until GetMessage() returns 0
    while (GetMessage (&messages, NULL, 0, 0))
    {
        // Translate virtual-key messages into character messages
        TranslateMessage(&messages);
        // Send message to WindowProcedure
        DispatchMessage(&messages);
    }

    // The program return-value is 0 - The value that PostQuitMessage() gave
    return messages.wParam;
}


//  This function is called by the Windows function DispatchMessage()
//  hwnd is the window handle
//  message is the message id
//  other parameters are used for identifying the widget that sent the message
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == NotifierMessage)         // If the message is originated from the sequencer notifier ...
    {
        ProcessNotifierMessage( (MIDISequencerGUIEvent)lParam );
                                            // ... process it with our dedicated function to upgrade the GUI
        return 0;
    }

    switch (message)                        // handle other messages (originated from Windows)
    {

        // The WM_CREATE message is sent by Windows at the creation of the main window.
        // Here it creates all its children widgets: this is only windows API stuff and you
        // probably may want to do it with a dedicated UI toolkit.
        // However here we create some buttons and static (i.e. text boxes) controls
         case WM_CREATE:

            // "Load" button
            CreateWindowW(L"button",        // Preregistered class (type) of winwow: in this case a button
                          L"Load",          // Button label
                WS_VISIBLE | WS_CHILD ,     // Attributes
                10, 10, 50, 25,             // x, y, w, h (in the parent window)
                hwnd,                       // Handle to the parent
                (HMENU) 1,                  // Parameter used for identifying the control
                NULL, NULL);                // Other unused parameters

            // "Rew" button
            CreateWindowW(L"button", L"Rew",// Same
                WS_VISIBLE | WS_CHILD ,
                100, 10, 50, 25,
                hwnd, (HMENU) 2, NULL, NULL);

            // "Play" button
            CreateWindowW(L"button", L"Play",
                WS_VISIBLE | WS_CHILD ,
                155, 10, 50, 25,
                hwnd, (HMENU) 3, NULL, NULL);

            // "Stop" button
            CreateWindowW(L"button", L"Stop",
                WS_VISIBLE | WS_CHILD ,
                210, 10, 50, 25,
                hwnd, (HMENU) 4, NULL, NULL);

            // Back step button
            CreateWindowW(L"button", L"<<",
                WS_VISIBLE | WS_CHILD ,
                265, 10, 50, 25,
                hwnd, (HMENU) 5, NULL, NULL);

            // Forward step button
            CreateWindowW(L"button", L">>",
                WS_VISIBLE | WS_CHILD ,
                320, 10, 50, 25,
                hwnd, (HMENU) 6, NULL, NULL);

            // Filename text box
            hFileName = CreateWindowW(L"static", L"UNLOADED",
                WS_VISIBLE | WS_CHILD | SS_LEFTNOWORDWRAP,
                400, 10, 380, 25,
                hwnd, (HMENU) 7, NULL, NULL);

            // Timesig text box
            hTime = CreateWindowW(L"static", NULL,
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                100, 45, 50, 25,
                hwnd, (HMENU) 8, NULL, NULL);

            // Tempo text box
            hTempo = CreateWindowW(L"static", NULL,
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                155, 45, 50, 25,
                hwnd, (HMENU) 8, NULL, NULL);

            // Meas - beat text box
            hMeas = CreateWindowW(L"static", NULL,
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                210, 45, 50, 25,
                hwnd, (HMENU) 7, NULL, NULL);

            // SMPTE text box
            hSmpte = CreateWindowW(L"static", NULL,
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                265, 45, 105, 25,
                hwnd, (HMENU) 7, NULL, NULL);

            // Marker text box
            hMarker = CreateWindowW(L"static", NULL,
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                400, 45, 380, 25,
                hwnd, (HMENU) 7, NULL, NULL);

            // For every track ...
            for (int i = 0; i < 16; i++) {
                wchar_t s[5];
/* NOTE BY NC: this gives an error when compiled with MINGW and the -std=c++0x flag, because this enable
   by default also the -ansi flag and this doesn't define swprintf : to workaround this you may:
   1) use the -std=gnu++0x instead
   2) add the compiler option -U__STRICT_ANSI__
*/
                swprintf(s, L"%d", i+1);

                // Track number text box
                CreateWindowW(L"static", s,
                    WS_VISIBLE | WS_CHILD | SS_LEFT,
                    10, 80+30*i, 30, 25,
                    hwnd, NULL, NULL, NULL);

                // Track name text box
                hTrackNames[i] = CreateWindowW(L"static", NULL,
                    WS_VISIBLE | WS_CHILD | SS_LEFT,
                    45, 80+30*i, 180, 25,
                    hwnd, NULL, NULL, NULL);

                // Track channel text box
                hTrackChans[i] = CreateWindowW(L"static", NULL,
                    WS_VISIBLE | WS_CHILD | SS_LEFT,
                    230, 80+30*i, 70, 25,
                    hwnd, NULL, NULL, NULL);

                // Track program (patch) text box
                hTrackPrgrs[i] = CreateWindowW(L"static", NULL,
                    WS_VISIBLE | WS_CHILD | SS_LEFT,
                    305, 80+30*i, 180, 25,
                    hwnd, NULL, NULL, NULL);

                // Track volume text box
                hTrackVols[i] = CreateWindowW(L"static", NULL,
                    WS_VISIBLE | WS_CHILD | SS_LEFT,
                    490, 80+30*i, 70, 25,
                    hwnd, NULL, NULL, NULL);
            }
            break;

        // The WM_COMMAND message is sent by buttons: so we identify the button that sent the message
        // (LOWORD(wParam) is the MENU param in the corresponding CreateWindowW) and call the appropriate
        // AdvancedSequencer method
        case WM_COMMAND:

            if (LOWORD(wParam) == 1) {      // Load
                LoadFile();
            }

            if (LOWORD(wParam) == 2) {      // Rew
                sequencer->GoToZero();
            }

            if (LOWORD(wParam) == 3) {      // Play
                // start playing the file
                sequencer->Play();
                // start a Windows timer for updating the smpte box: the timer
                // sends a WM_TIMER message to the main window every 1/20 second
                SetTimer(hwnd,              // window to which send WM_TIMER messages
                         1,                 // timer id
                         50,                // timer interval (50 msecs)
                         NULL);             // unused
            }

            if (LOWORD(wParam) == 4) {      // Stop
                sequencer->Stop();          // stop playback
                KillTimer(hMainWin, 1);     // stop the timer (1 is the timer id)
            }

            if (LOWORD(wParam) == 5) {      // Step backward
                sequencer->GoToMeasure(sequencer->GetMeasure() - 1);
            }

            if (LOWORD(wParam) == 6) {      // Step forward
                sequencer->GoToMeasure(sequencer->GetMeasure() + 1);
            }
            break;

        // The WM_TIMER message is sent by the timer for updating the SMPTE box
        case WM_TIMER:

            if (LOWORD(wParam) == 1) {      // timer id
                // update the Smpte box with the current SMPTE string
                SetWindowText( hSmpte, GetSmpteString());
            }
            break;

        // The WM_DESTROY message is sent by Windows when the program ends
        case WM_DESTROY:
            PostQuitMessage (0);       // send a WM_QUIT to the message queue
            break;

        default:                      // for messages that we don't deal with
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

// This function loads a file into the sequencer
VOID LoadFile() {
    OPENFILENAME ofn;                       // A struct with appropriate values for the open file control
    char szFileName[MAX_PATH] = "";         // This will hold the filename

    // Sets the correct values for the OPENFILENAME struct
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Midi Files (*.mid)\0*.mid\0\0";  // this is the filename filter
    ofn.lpstrFile = szFileName;                         // the string which will hold the filename
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "mid";

    if(GetOpenFileName(&ofn)) {             // Shows the file choosing control and waits until it is closed
                                            // returning the filename in szFileName
        if (!sequencer->Load(szFileName)) { // Error: the sequencer could not load the file
            MessageBox(
                NULL,
                (LPCSTR)"File loading failed",
                NULL,
                MB_ICONERROR | MB_OK
            );
        }
        else {                              // File loading OK
/* NOTE: currently the notifier is totally fooled by format 0 MIDIFiles (all events in track 0) and reports
 * incorrect informations on track names, volumes, etc. This patch makes a 17-tracks multitrack from the
 * original 1-track, modifying the midifile: be careful if you use this in contests in which you may re-save
 * the file. TODO: adding an option for saving in different MIDI formats in filewrite.h
 */
            if (sequencer->GetMultiTrack()->GetNumTracksWithEvents() == 1)   // all events in one track: format 0
            {
                // redistributes channel events in separate tracks
                sequencer->GetMultiTrack()->AssignEventsToTracks(
                        sequencer->GetMultiTrack()->GetTrack(0) );

                // IMPORTANT: when you edit events you always must call this to update the AdvancedSequencer
                sequencer->SetChanged();
            }

            // update the filename textbox
            SetWindowText( hFileName, szFileName );

            // update the timesig, tempo ... controls
            SetControls();
        }
    }
}

// This function upsdates all the sequencer related textboxes: it is called when a file is loaded and when
// the GUI receives from the notifier a GROUP_ALL (reset) message (for example rewind or step action)
VOID SetControls() {
    char s[300];

    // Update the timesig, tempo, meas-beat, smpte, marker boxes
    sprintf (s, "%d/%d", sequencer->GetTimeSigNumerator(), sequencer->GetTimeSigDenominator());
    SetWindowText( hTime, s );
    sprintf (s, "%3.2f", sequencer->GetTempoWithoutScale());
    SetWindowText ( hTempo, s );
    sprintf (s, "%d:%d", sequencer->GetMeasure()+1, sequencer->GetBeat()+1);
    SetWindowText ( hMeas, s );
    SetWindowText ( hSmpte, GetSmpteString());
    if ( strlen(sequencer->GetCurrentMarker()) == 0) {
        SetWindowText (hMarker, "---");
    }
    else {
        SetWindowText (hMarker, sequencer->GetCurrentMarker());
    }

    // for every track, update the name, channel, program, volume boxes
    for (int i = 0; i < 16; i++) {

        if ( strlen(sequencer->GetTrackName(i+1)) ) {
            SetWindowText (hTrackNames[i], sequencer->GetTrackName (i+1));
        }
        else {
            sprintf(s, "(track %d)", i+1);
            SetWindowText (hTrackNames[i], s);
        }

        if (sequencer->FindFirstChannelOnTrack(i+1) == -1) {    // there aren't channel events or the track is empty
            SetWindowText (hTrackChans[i], "---" );
        }
        else {
            sprintf(s, "ch: %d", sequencer->FindFirstChannelOnTrack(i+1));
            SetWindowText(hTrackChans[i], s);
        }

        if (sequencer->GetTrackProgram(i+1) == -1) {
            SetWindowText(hTrackPrgrs[i], "---");
        }
        else {
            if (sequencer->FindFirstChannelOnTrack(i+1) == 10) {
                SetWindowText(hTrackPrgrs[i], GMDrumKits[sequencer->GetTrackProgram(i+1)]);
            }
            else {
                SetWindowText(hTrackPrgrs[i], GMpatches[sequencer->GetTrackProgram(i+1)]);
            }
        }
        sprintf (s, "vol: %d", sequencer->GetTrackVolume(i+1) );
        SetWindowText(hTrackVols[i], s);
    }

}

// This function returns a text string in the SMPTE format h:mm:ss:ff corresponding to
//the current time
const char* GetSmpteString() {
    static char s[100];

    // Get from the sequencer the current time in msecs
    unsigned long msecs = sequencer->GetCurrentTimeInMs();

/* ADDED NEW METHOD SetMilliSeconds() to SMPTE class
    // Convert the msecs in samples (the default frequency of the SMPTE is 48000 but you can change it
    ulong samples = (ulong) (msecs * GetSampleRateFrequency(smpte.GetSampleRate()) / 1000);

    // Feed the smpte with our number of samples
    smpte.SetSampleNumber(samples);
*/

    // Feed the smpte with msecs
    smpte.SetMilliSeconds(msecs);

    // Now it qives us the hours, minutes, secs and frames corresponding to our msecs
    sprintf (s, "%d:%02d:%02d:%02d", smpte.GetHours(), smpte.GetMinutes(),
                smpte.GetSeconds(), smpte.GetFrames());
    return s;
}


// This function is called by WindowProcedure whe it receives a notifier message: it does essentially
// the same things that SetControls does, but updates only the appropriate control based on the param
// ev (the notifier event)
 VOID ProcessNotifierMessage( MIDISequencerGUIEvent ev) {
// the param ev is a GUIEvent object (see the file sequencer.h)
    char s[300];

    // Get the group (general type) of the event
    switch (ev.GetEventGroup()) {

        case MIDISequencerGUIEvent::GROUP_ALL:
        // This is a general GUI reset event: update all rextboxes
            SetControls();
            break;

        case MIDISequencerGUIEvent::GROUP_CONDUCTOR:
        // This is an event regarding conductor track: find the type and update appropriate textbox

            switch (ev.GetEventItem()) {
                case MIDISequencerGUIEvent::GROUP_CONDUCTOR_TEMPO:
                // Tempo (bpm) is changed
                    sprintf (s, "%3.2f", sequencer->GetTempoWithoutScale());
                    SetWindowText ( hTempo, s );
                    break;
                case MIDISequencerGUIEvent::GROUP_CONDUCTOR_TIMESIG:
                // Timesig is changed
                    sprintf (s, "%d/%d", sequencer->GetTimeSigNumerator(), sequencer->GetTimeSigDenominator());
                    SetWindowText( hTime, s );
                    break;
                case MIDISequencerGUIEvent::GROUP_CONDUCTOR_KEYSIG:
                    /* TODO: */
                    //sprintf(s, "%s", KeyNames[seq->GetState()->keysig_sharpflat+ 7 +
                    //                          15 * seq->GetState()->keysig_mode]);
                    break;
                case MIDISequencerGUIEvent::GROUP_CONDUCTOR_MARKER:
                // Marker is changed
                    SetWindowText (hMarker, sequencer->GetCurrentMarker());
                    break;
            }
            break;

        case MIDISequencerGUIEvent::GROUP_TRANSPORT:
        // This is an event regarding transport (start, stop, etc): we monitor only
        // beat events to update the meas - beat box
            if (ev.GetEventItem() == MIDISequencerGUIEvent::GROUP_TRANSPORT_BEAT) {
                sprintf (s, "%d:%d", sequencer->GetMeasure()+1, sequencer->GetBeat()+1);
                SetWindowText ( hMeas, s );
            }
            break;

        case MIDISequencerGUIEvent::GROUP_TRACK: {
        // This is a track event: find the track (GetEventSubGroup) and the type (GetEventItem) and proceed
            int track = ev.GetEventSubGroup();
            if (ev.GetEventItem() == MIDISequencerGUIEvent::GROUP_TRACK_PG) {
                if (track > 0) {
                    if (sequencer->FindFirstChannelOnTrack(track) == 10) {
                        SetWindowText( hTrackPrgrs[track-1], GMDrumKits[sequencer->GetTrackProgram( track )] );
                    }
                    else {
                        SetWindowText ( hTrackPrgrs[track-1], GMpatches[sequencer->GetTrackProgram( track)] );
                    }
                }
            }

            else if (ev.GetEventItem() == MIDISequencerGUIEvent::GROUP_TRACK_VOLUME) {
                sprintf (s, "vol: %d", sequencer->GetTrackVolume( track ) );
                if (track > 0) {
                    SetWindowText ( hTrackVols[track-1], s );
                }
            }
            break;
        }

    }

}
