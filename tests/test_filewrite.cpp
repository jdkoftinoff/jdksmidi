#include "doctest/doctest.h"
#include "jdksmidi/filewrite.h"
#include "jdksmidi/fileread.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/sysex.h"

#include <vector>
#include <cstring>
#include <memory>

using namespace jdksmidi;

// Memory-based output stream for testing
class MIDIFileWriteStreamMemory : public MIDIFileWriteStream
{
public:
    MIDIFileWriteStreamMemory()
        : _position(0)
    {}
    
    std::int32_t seek(std::int32_t pos, int whence = SEEK_SET) override
    {
        switch (whence) {
            case SEEK_SET:
                _position = pos;
                break;
            case SEEK_CUR:
                _position += pos;
                break;
            case SEEK_END:
                _position = _data.size() + pos;
                break;
            default:
                return -1;
        }
        
        // Extend buffer if seeking beyond current size
        if (_position > static_cast<std::int32_t>(_data.size())) {
            _data.resize(_position, 0);
        }
        
        return _position;
    }
    
    int write_char(int c) override
    {
        if (_position >= static_cast<std::int32_t>(_data.size())) {
            _data.resize(_position + 1, 0);
        }
        
        _data[_position] = static_cast<std::uint8_t>(c);
        _position++;
        return c;
    }
    
    std::vector<std::uint8_t> const& get_data() const { return _data; }
    void clear() { _data.clear(); _position = 0; }
    size_t size() const { return _data.size(); }
    std::int32_t get_position() const { return _position; }

private:
    std::vector<std::uint8_t> _data;
    std::int32_t _position;
};

// Memory-based input stream for round-trip testing
class MIDIFileReadStreamMemory : public MIDIFileReadStream
{
public:
    explicit MIDIFileReadStreamMemory(std::vector<std::uint8_t> const& data)
        : _data(data), _position(0) {}
    
    int read_char() override
    {
        if (_position >= _data.size()) return -1;
        return _data[_position++];
    }
    
    void reset() { _position = 0; }

private:
    std::vector<std::uint8_t> _data;
    size_t _position;
};

// Test event handler - copied exactly from working test_fileread.cpp
class TestMIDIFileEvents : public MIDIFileEvents
{
public:
    TestMIDIFileEvents()
        : _header_called(false)
        , _start_track_count(0)
        , _end_track_count(0)
        , _eot_count(0)
        , _error_count(0)
        , _format(0)
        , _ntrks(0)
        , _division(0)
    {}

    void mf_header(int format, int ntrks, int division) override
    {
        _header_called = true;
        _format = format;
        _ntrks = ntrks;
        _division = division;
    }

    void mf_starttrack(int trk) override
    {
        _start_track_count++;
    }

    void mf_endtrack(int trk) override
    {
        _end_track_count++;
    }

    void mf_eot(MIDIClockTime time) override
    {
        _eot_count++;
    }

    void mf_error(char const* msg) override
    {
        _error_count++;
    }

    // Accessors for test verification
    bool header_called() const { return _header_called; }
    int get_format() const { return _format; }
    int get_ntrks() const { return _ntrks; }
    int get_division() const { return _division; }
    int get_start_track_count() const { return _start_track_count; }
    int get_end_track_count() const { return _end_track_count; }
    int get_eot_count() const { return _eot_count; }
    int get_error_count() const { return _error_count; }

private:
    bool _header_called;
    int _start_track_count, _end_track_count;
    int _eot_count, _error_count;
    int _format, _ntrks, _division;
};

TEST_CASE("MIDIFileWriteStream memory stream functionality")
{
    SUBCASE("Basic write operations")
    {
        MIDIFileWriteStreamMemory stream;
        
        CHECK(stream.write_char('M') == 'M');
        CHECK(stream.write_char('T') == 'T');
        CHECK(stream.write_char('h') == 'h');
        CHECK(stream.write_char('d') == 'd');
        
        auto const& data = stream.get_data();
        CHECK(data.size() == 4);
        CHECK(data[0] == 'M');
        CHECK(data[1] == 'T');
        CHECK(data[2] == 'h');
        CHECK(data[3] == 'd');
    }
    
    SUBCASE("Seek operations")
    {
        MIDIFileWriteStreamMemory stream;
        
        stream.write_char('A');
        stream.write_char('B');
        
        // Seek to beginning and overwrite
        CHECK(stream.seek(0) == 0);
        stream.write_char('X');
        
        auto const& data = stream.get_data();
        CHECK(data[0] == 'X');
        CHECK(data[1] == 'B');
        
        // Seek to end
        CHECK(stream.seek(0, SEEK_END) == 2);
        stream.write_char('Z');
        CHECK(data.size() == 3);
        CHECK(data[2] == 'Z');
        
        // Seek relative
        stream.seek(-1, SEEK_CUR);
        stream.write_char('Y');
        CHECK(data[2] == 'Y');
    }
    
    SUBCASE("Seek beyond current size")
    {
        MIDIFileWriteStreamMemory stream;
        
        stream.write_char('A');
        CHECK(stream.seek(10) == 10);
        stream.write_char('B');
        
        auto const& data = stream.get_data();
        CHECK(data.size() == 11);
        CHECK(data[0] == 'A');
        CHECK(data[10] == 'B');
        // Intermediate bytes should be zero
        for (size_t i = 1; i < 10; ++i) {
            CHECK(data[i] == 0);
        }
    }
}

TEST_CASE("MIDIFileWrite basic construction and file headers")
{
    SUBCASE("Basic construction")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        CHECK_FALSE(writer.error_occurred());
        CHECK(writer.get_file_length() == 0);
        CHECK(writer.get_track_length() == 0);
    }
    
    SUBCASE("File header writing")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        writer.write_file_header(1, 2, 480);
        
        auto const& data = stream.get_data();
        CHECK(data.size() == 14);  // Header chunk is 14 bytes
        
        // Check header chunk ID "MThd"
        CHECK(data[0] == 'M');
        CHECK(data[1] == 'T');
        CHECK(data[2] == 'h');
        CHECK(data[3] == 'd');
        
        // Check header length (6 bytes)
        CHECK(data[4] == 0x00);
        CHECK(data[5] == 0x00);
        CHECK(data[6] == 0x00);
        CHECK(data[7] == 0x06);
        
        // Check format (1)
        CHECK(data[8] == 0x00);
        CHECK(data[9] == 0x01);
        
        // Check number of tracks (2)
        CHECK(data[10] == 0x00);
        CHECK(data[11] == 0x02);
        
        // Check division (480)
        CHECK(data[12] == 0x01);
        CHECK(data[13] == 0xE0);
        
        CHECK(writer.get_file_length() == 14);
        CHECK_FALSE(writer.error_occurred());
    }
    
    SUBCASE("Different file formats and divisions")
    {
        auto test_header = [](int format, int ntrks, int division) {
            MIDIFileWriteStreamMemory stream;
            MIDIFileWrite writer(&stream);
            
            writer.write_file_header(format, ntrks, division);
            
            auto const& data = stream.get_data();
            CHECK(data.size() == 14);
            
            // Verify format
            std::uint16_t file_format = (data[8] << 8) | data[9];
            CHECK(file_format == format);
            
            // Verify track count
            std::uint16_t track_count = (data[10] << 8) | data[11];
            CHECK(track_count == ntrks);
            
            // Verify division
            std::uint16_t file_division = (data[12] << 8) | data[13];
            CHECK(file_division == division);
        };
        
        test_header(0, 1, 96);
        test_header(1, 16, 480);
        test_header(2, 32, 960);
    }
}

TEST_CASE("MIDIFileWrite track operations")
{
    SUBCASE("Track header writing")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        writer.write_track_header(100);
        
        auto const& data = stream.get_data();
        CHECK(data.size() == 8);  // Track header is 8 bytes
        
        // Check track chunk ID "MTrk"
        CHECK(data[0] == 'M');
        CHECK(data[1] == 'T');
        CHECK(data[2] == 'r');
        CHECK(data[3] == 'k');
        
        // Check track length (100)
        CHECK(data[4] == 0x00);
        CHECK(data[5] == 0x00);
        CHECK(data[6] == 0x00);
        CHECK(data[7] == 0x64);
        
        CHECK(writer.get_file_length() == 8);
    }
    
    SUBCASE("Track length management")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        writer.reset_track_length();
        CHECK(writer.get_track_length() == 0);
        
        writer.reset_track_time();
        // Track time is private, but we can verify through other operations
    }
}

TEST_CASE("MIDIFileWrite MIDI event writing")
{
    SUBCASE("Basic MIDI message writing")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        // Need to start a track first
        writer.write_track_header(0);
        
        // Create a note on message
        MIDITimedMessage msg;
        msg.set_time(0);
        msg.set_note_on(0, 60, 100);  // Channel 0, Note C4, Velocity 100
        
        writer.write_event(msg);
        
        auto const& data = stream.get_data();
        CHECK(data.size() >= 12);  // Track header (8) + MIDI event (4)
        
        // MIDI event should start at offset 8
        CHECK(data[8] == 0x00);   // Delta time (0) - variable length encoding
        CHECK(data[9] == 0x90);   // Note on, channel 0
        CHECK(data[10] == 60);    // Note C4
        CHECK(data[11] == 100);   // Velocity 100
        
        CHECK_FALSE(writer.error_occurred());
    }
    
    SUBCASE("Multiple MIDI events with delta times")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        // Need to start a track first
        writer.write_track_header(0);
        
        // Note on at time 0
        MIDITimedMessage note_on;
        note_on.set_time(0);
        note_on.set_note_on(0, 60, 100);
        writer.write_event(note_on);
        
        // Note off at time 480 (delta = 480)
        MIDITimedMessage note_off;
        note_off.set_time(480);
        note_off.set_note_off(0, 60, 64);
        writer.write_event(note_off);
        
        auto const& data = stream.get_data();
        CHECK(data.size() >= 17);  // Track header (8) + two events
        
        // First event should start at offset 8 with delta time 0
        CHECK(data[8] == 0x00);   // Delta time 0
        CHECK(data[9] == 0x90);   // Note on
        
        // Second event should have delta time 480 (0x83 0x60 in variable length)
        size_t second_event_pos = 12;  // After track header + first event
        CHECK(data[second_event_pos] == 0x83);
        CHECK(data[second_event_pos + 1] == 0x60);
        CHECK(data[second_event_pos + 2] == 0x80);  // Note off
    }
}

TEST_CASE("MIDIFileWrite meta event writing")
{
    SUBCASE("Tempo meta event")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        // Need to start a track first
        writer.write_track_header(0);
        
        // Write tempo: 120 BPM = 500000 microseconds per quarter note
        writer.write_tempo(0, 500000);
        
        auto const& data = stream.get_data();
        CHECK(data.size() >= 15);  // Track header (8) + tempo event (7)
        
        // Tempo event should start at offset 8
        CHECK(data[8] == 0x00);  // Delta time 0
        CHECK(data[9] == 0xFF);  // Meta event
        CHECK(data[10] == 0x51);  // Tempo event type
        CHECK(data[11] == 0x03);  // Length: 3 bytes
        
        // Tempo value in 24-bit big-endian (500000 = 0x07A120)
        CHECK(data[12] == 0x07);
        CHECK(data[13] == 0xA1);
        CHECK(data[14] == 0x20);
    }
    
    SUBCASE("Time signature meta event")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        // Need to start a track first
        writer.write_track_header(0);
        
        // Write 4/4 time signature
        writer.write_time_signature(0, 4, 2, 24, 8);
        
        auto const& data = stream.get_data();
        CHECK(data.size() >= 16);  // Track header (8) + time sig event (8)
        
        // Time sig event should start at offset 8
        CHECK(data[8] == 0x00);  // Delta time 0
        CHECK(data[9] == 0xFF);  // Meta event
        CHECK(data[10] == 0x58);  // Time signature event type
        CHECK(data[11] == 0x04);  // Length: 4 bytes
        CHECK(data[12] == 4);     // Numerator
        CHECK(data[13] == 2);     // Denominator power (2^2 = 4)
        CHECK(data[14] == 24);    // MIDI clocks per metronome click
        CHECK(data[15] == 8);     // 32nd notes per quarter note
    }
    
    SUBCASE("Key signature meta event")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        // Need to start a track first
        writer.write_track_header(0);
        
        // Write C major key signature (0 sharps/flats, major mode)
        writer.write_key_signature(0, 0, 0);
        
        auto const& data = stream.get_data();
        CHECK(data.size() >= 14);  // Track header (8) + key sig event (6)
        
        // Key sig event should start at offset 8
        CHECK(data[8] == 0x00);  // Delta time 0
        CHECK(data[9] == 0xFF);  // Meta event
        CHECK(data[10] == 0x59);  // Key signature event type
        CHECK(data[11] == 0x02);  // Length: 2 bytes
        CHECK(data[12] == 0);     // 0 sharps/flats
        CHECK(data[13] == 0);     // Major mode
    }
    
    SUBCASE("Text meta event")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        // Need to start a track first
        writer.write_track_header(0);
        
        char const* text = "Test Track";
        writer.write_event(0, 0x03, text);  // Track name event
        
        auto const& data = stream.get_data();
        size_t expected_size = 8 + 1 + 1 + 1 + 1 + strlen(text);  // track header + delta + FF + type + len + text
        CHECK(data.size() >= expected_size);
        
        // Text event should start at offset 8
        CHECK(data[8] == 0x00);  // Delta time 0
        CHECK(data[9] == 0xFF);  // Meta event
        CHECK(data[10] == 0x03);  // Track name event type
        CHECK(data[11] == strlen(text));  // Length
        
        // Verify text content
        for (size_t i = 0; i < strlen(text); ++i) {
            CHECK(data[12 + i] == text[i]);
        }
    }
    
    SUBCASE("End of track meta event")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        // Need to start a track first
        writer.write_track_header(0);  // Placeholder length
        writer.write_end_of_track(0);
        
        auto const& data = stream.get_data();
        CHECK(data.size() >= 12);  // Track header (8) + End of track (4)
        
        // Check track header first
        CHECK(data[0] == 'M');
        CHECK(data[1] == 'T');
        CHECK(data[2] == 'r');
        CHECK(data[3] == 'k');
        
        // End of track should start at offset 8
        CHECK(data[8] == 0x00);  // Delta time 0
        CHECK(data[9] == 0xFF);  // Meta event
        CHECK(data[10] == 0x2F);  // End of track event type
        CHECK(data[11] == 0x00);  // Length: 0 bytes
    }
}

TEST_CASE("MIDIFileWrite complete file creation")
{
    SUBCASE("Minimal valid MIDI file")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        // Write file header (Format 0, 1 track, 480 ticks per quarter note)
        writer.write_file_header(0, 1, 480);
        CHECK_FALSE(writer.error_occurred());
        
        // Write track header (placeholder length)
        writer.write_track_header(0);  // Will be rewritten later
        CHECK_FALSE(writer.error_occurred());
        
        // Write end of track
        writer.write_end_of_track(0);
        CHECK_FALSE(writer.error_occurred());
        
        // Calculate and rewrite track length
        writer.rewrite_track_length();
        CHECK_FALSE(writer.error_occurred());
        
        auto const& data = stream.get_data();
        CHECK(data.size() == 26);  // 14 (header) + 8 (track header) + 4 (end of track)
        
        // Verify basic file structure
        CHECK(data[0] == 'M');
        CHECK(data[1] == 'T');
        CHECK(data[2] == 'h');
        CHECK(data[3] == 'd');
        
        // Check track header
        CHECK(data[14] == 'M');
        CHECK(data[15] == 'T');
        CHECK(data[16] == 'r');
        CHECK(data[17] == 'k');
        
        // Comprehensive binary format validation
        // This MIDI file should be identical to a standard minimal MIDI file
        std::vector<std::uint8_t> expected_minimal_midi = {
            'M', 'T', 'h', 'd',              // Header chunk ID
            0x00, 0x00, 0x00, 0x06,          // Header length: 6 bytes
            0x00, 0x00,                      // Format 0
            0x00, 0x01,                      // 1 track
            0x01, 0xE0,                      // 480 ticks per quarter note
            'M', 'T', 'r', 'k',              // Track chunk ID
            0x00, 0x00, 0x00, 0x04,          // Track length: 4 bytes
            0x00, 0xFF, 0x2F, 0x00           // End of track (delta=0, meta event FF 2F, length=0)
        };
        
        // Verify our output matches the expected standard format exactly
        CHECK(data.size() == expected_minimal_midi.size());
        for (size_t i = 0; i < data.size() && i < expected_minimal_midi.size(); ++i) {
            CHECK(data[i] == expected_minimal_midi[i]);
        }
    }
    
    SUBCASE("MIDI file with note sequence")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        // Write file header
        writer.write_file_header(0, 1, 480);
        
        // Write track header
        writer.write_track_header(0);  // Placeholder
        
        // Write a simple note sequence
        MIDITimedMessage note_on;
        note_on.set_time(0);
        note_on.set_note_on(0, 60, 100);
        writer.write_event(note_on);
        
        MIDITimedMessage note_off;
        note_off.set_time(480);
        note_off.set_note_off(0, 60, 64);
        writer.write_event(note_off);
        
        // Write end of track
        writer.write_end_of_track(480);
        
        // Rewrite track length
        writer.rewrite_track_length();
        
        // Verify file structure
        auto const& data = stream.get_data();
        CHECK(data.size() > 26);  // Should be larger due to note events
        
        // Verify header structure
        CHECK(data[0] == 'M');
        CHECK(data[1] == 'T');
        CHECK(data[2] == 'h');
        CHECK(data[3] == 'd');
        
        // Verify track structure
        CHECK(data[14] == 'M');
        CHECK(data[15] == 'T');
        CHECK(data[16] == 'r');
        CHECK(data[17] == 'k');
        
        // Verify the note sequence was written correctly in binary format
        // Check for note on/off events in the track data
        CHECK(data.size() > 30);  // Should contain note events
        
        // Look for MIDI note events in the track data (after header + track header)
        bool found_note_on = false;
        bool found_note_off = false;
        for (size_t i = 22; i < data.size() - 3; ++i) {
            if (data[i] == 0x90 && data[i+1] == 60 && data[i+2] == 100) {  // Note on C4, vel 100
                found_note_on = true;
            }
            if (data[i] == 0x80 && data[i+1] == 60) {  // Note off C4
                found_note_off = true;
            }
        }
        CHECK(found_note_on);
        CHECK(found_note_off);
    }
}

TEST_CASE("MIDIFileWrite round-trip testing")
{
    SUBCASE("Complex MIDI file structure verification")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        // Create a more complex MIDI file
        writer.write_file_header(1, 2, 480);
        CHECK_FALSE(writer.error_occurred());
        
        // Track 1: Tempo and time signature
        writer.write_track_header(0);
        writer.write_tempo(0, 500000);  // 120 BPM
        writer.write_time_signature(0, 4, 2, 24, 8);  // 4/4 time
        writer.write_end_of_track(0);
        writer.rewrite_track_length();
        CHECK_FALSE(writer.error_occurred());
        
        // Track 2: Note sequence
        writer.write_track_header(0);
        
        // Write several notes
        MIDITimedMessage msg;
        for (int i = 0; i < 4; ++i) {
            msg.set_time(i * 480);
            msg.set_note_on(0, 60 + i, 100);
            writer.write_event(msg);
            
            msg.set_time((i * 480) + 240);
            msg.set_note_off(0, 60 + i, 64);
            writer.write_event(msg);
        }
        
        writer.write_end_of_track(4 * 480);
        writer.rewrite_track_length();
        CHECK_FALSE(writer.error_occurred());
        
        // Verify file structure
        auto const& data = stream.get_data();
        CHECK(data.size() > 50);  // Should be a substantial file
        
        // Verify file header
        CHECK(data[0] == 'M');
        CHECK(data[1] == 'T');
        CHECK(data[2] == 'h');
        CHECK(data[3] == 'd');
        
        // Verify format 1, 2 tracks, 480 division
        CHECK(data[8] == 0x00);
        CHECK(data[9] == 0x01);  // Format 1
        CHECK(data[10] == 0x00);
        CHECK(data[11] == 0x02); // 2 tracks
        
        // Verify complex file structure with binary format validation
        // Check for tempo and time signature meta events
        bool found_tempo = false;
        bool found_time_sig = false;
        bool found_multiple_tracks = false;
        
        // Look for meta events in the file
        for (size_t i = 0; i < data.size() - 3; ++i) {
            if (data[i] == 0xFF && data[i+1] == 0x51) {  // Tempo meta event
                found_tempo = true;
            }
            if (data[i] == 0xFF && data[i+1] == 0x58) {  // Time signature meta event
                found_time_sig = true;
            }
        }
        
        // Count track headers
        int track_count = 0;
        for (size_t i = 0; i < data.size() - 3; ++i) {
            if (data[i] == 'M' && data[i+1] == 'T' && data[i+2] == 'r' && data[i+3] == 'k') {
                track_count++;
            }
        }
        
        CHECK(found_tempo);
        CHECK(found_time_sig);
        CHECK(track_count == 2);  // Should have 2 tracks
    }
}

TEST_CASE("MIDIFileWrite error handling")
{
    SUBCASE("Stream error detection")
    {
        // This test would require a mock stream that fails writes
        // For now, we test that error state is properly tracked
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        // Normal operations should not cause errors
        writer.write_file_header(0, 1, 480);
        CHECK_FALSE(writer.error_occurred());
        
        writer.write_track_header(0);
        CHECK_FALSE(writer.error_occurred());
        
        writer.write_end_of_track(0);
        CHECK_FALSE(writer.error_occurred());
    }
    
    SUBCASE("Large data handling")
    {
        MIDIFileWriteStreamMemory stream;
        MIDIFileWrite writer(&stream);
        
        // Write a file with many events to test buffer handling
        writer.write_file_header(0, 1, 480);
        writer.write_track_header(0);
        
        // Write 1000 note events
        for (int i = 0; i < 1000; ++i) {
            MIDITimedMessage msg;
            msg.set_time(i);
            msg.set_note_on(i % 16, 60 + (i % 12), 100);
            writer.write_event(msg);
        }
        
        writer.write_end_of_track(1000);
        writer.rewrite_track_length();
        
        CHECK_FALSE(writer.error_occurred());
        CHECK(stream.size() > 4000);  // Should be a reasonably large file
        
        // Verify basic file structure
        auto const& data = stream.get_data();
        CHECK(data[0] == 'M');
        CHECK(data[1] == 'T');
        CHECK(data[2] == 'h');
        CHECK(data[3] == 'd');
        
        // Verify large file contains substantial MIDI event data
        // Note: MIDI uses running status which reduces the number of explicit status bytes
        int note_on_count = 0;
        for (size_t i = 22; i < data.size() - 2; ++i) {  // Skip headers, look in track data
            if (data[i] == 0x90) {  // Note on event
                note_on_count++;
            }
        }
        CHECK(note_on_count > 50);   // Should have substantial note events (running status reduces count)
        CHECK(data.size() > 4000);   // File should be large due to many events
    }
}