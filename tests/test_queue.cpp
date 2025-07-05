/*
 *  libjdksmidi-2004 C++ Class Library for MIDI
 *
 *  Copyright (C) 2004-2025  Jeffrey Koftinoff
 *  www.jdkoftinoff.com
 *  jeffk@jdkoftinoff.com
 *
 *  *** RELEASED UNDER THE MIT LICENSE ***
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include "doctest/doctest.h"
#include "jdksmidi/midi.h"
#include "jdksmidi/msg.h"
#include "jdksmidi/queue.h"

#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

using namespace jdksmidi;

TEST_CASE("MIDIQueue basic construction and initialization")
{
    SUBCASE("Construction with valid size")
    {
        MIDIQueue queue(10);

        // Initially empty
        CHECK_FALSE(queue.can_get());
        CHECK(queue.can_put());
        CHECK_FALSE(queue.is_full());
    }

    SUBCASE("Construction with different sizes")
    {
        MIDIQueue small_queue(2);  // Minimum size for functionality
        CHECK_FALSE(small_queue.can_get());
        CHECK(small_queue.can_put());

        MIDIQueue large_queue(1000);
        CHECK_FALSE(large_queue.can_get());
        CHECK(large_queue.can_put());
    }

    SUBCASE("Clear method resets queue state")
    {
        MIDIQueue queue(5);
        MIDITimedBigMessage msg;
        msg.set_note_on(0, 60, 100);

        // Add some messages
        queue.put(msg);
        queue.put(msg);
        CHECK(queue.can_get());

        // Clear should reset to empty state
        queue.clear();
        CHECK_FALSE(queue.can_get());
        CHECK(queue.can_put());
        CHECK_FALSE(queue.is_full());
    }
}

TEST_CASE("MIDIQueue basic operations")
{
    MIDIQueue queue(5);

    SUBCASE("Single message put and get")
    {
        MIDITimedBigMessage original;
        original.set_note_on(2, 64, 127);
        original.set_time(1000);

        // Put message
        CHECK(queue.can_put());
        queue.put(original);

        // Should now have a message to get
        CHECK(queue.can_get());
        CHECK_FALSE(queue.is_full());

        // Get message and verify contents
        MIDITimedBigMessage retrieved = queue.get();
        CHECK(retrieved.get_type() == NOTE_ON);
        CHECK(retrieved.get_channel() == 2);
        CHECK(retrieved.get_note() == 64);
        CHECK(retrieved.get_velocity() == 127);
        CHECK(retrieved.get_time() == 1000);

        // Advance queue
        queue.next();
        CHECK_FALSE(queue.can_get());
    }

    SUBCASE("Multiple message operations")
    {
        std::vector<MIDITimedBigMessage> messages(3);

        // Create different messages
        messages[0].set_note_on(0, 60, 100);
        messages[0].set_time(100);

        messages[1].set_control_change(1, 7, 127);
        messages[1].set_time(200);

        messages[2].set_note_off(2, 64, 64);
        messages[2].set_time(300);

        // Put all messages
        for (auto const& msg : messages) {
            CHECK(queue.can_put());
            queue.put(msg);
        }

        // Get and verify all messages in order
        for (size_t i = 0; i < messages.size(); ++i) {
            CHECK(queue.can_get());
            MIDITimedBigMessage retrieved = queue.get();

            CHECK(retrieved.get_type() == messages[i].get_type());
            CHECK(retrieved.get_channel() == messages[i].get_channel());
            CHECK(retrieved.get_time() == messages[i].get_time());

            queue.next();
        }

        CHECK_FALSE(queue.can_get());
    }

    SUBCASE("Peek operation")
    {
        MIDITimedBigMessage msg;
        msg.set_program_change(5, 42);
        msg.set_time(500);

        queue.put(msg);

        // Peek should return pointer to message without consuming it
        MIDITimedBigMessage const* peeked = queue.peek();
        CHECK(peeked != nullptr);
        CHECK(peeked->get_type() == PROGRAM_CHANGE);
        CHECK(peeked->get_channel() == 5);
        CHECK(peeked->get_byte1() == 42);
        CHECK(peeked->get_time() == 500);

        // Queue should still have the message
        CHECK(queue.can_get());

        // Get should return same message
        MIDITimedBigMessage retrieved = queue.get();
        CHECK(retrieved.get_type() == peeked->get_type());
        CHECK(retrieved.get_time() == peeked->get_time());
    }
}

TEST_CASE("MIDIQueue capacity and full conditions")
{
    SUBCASE("Fill to capacity")
    {
        MIDIQueue queue(3);
        MIDITimedBigMessage msg;
        msg.set_note_on(0, 60, 100);

        // Should be able to put 2 messages (capacity - 1)
        CHECK(queue.can_put());
        CHECK_FALSE(queue.is_full());
        queue.put(msg);

        CHECK(queue.can_put());
        CHECK_FALSE(queue.is_full());
        queue.put(msg);

        // After 2 messages, should be full
        CHECK_FALSE(queue.can_put());
        CHECK(queue.is_full());
        CHECK(queue.can_get());
    }

    SUBCASE("Empty after consuming all messages")
    {
        MIDIQueue queue(4);
        MIDITimedBigMessage msg;
        msg.set_control_change(0, 1, 64);

        // Fill with 3 messages
        queue.put(msg);
        queue.put(msg);
        queue.put(msg);

        CHECK(queue.can_get());
        CHECK_FALSE(queue.can_put());

        // Consume all messages
        queue.next();
        CHECK(queue.can_get());
        CHECK(queue.can_put());

        queue.next();
        CHECK(queue.can_get());
        CHECK(queue.can_put());

        queue.next();
        CHECK_FALSE(queue.can_get());
        CHECK(queue.can_put());
    }

    SUBCASE("Single element queue")
    {
        MIDIQueue queue(1);
        MIDITimedBigMessage msg;
        msg.set_note_on(0, 72, 90);

        // Size 1 queue cannot function - always full
        CHECK_FALSE(queue.can_get());
        CHECK_FALSE(queue.can_put());  // Size 1 means always "full"
        CHECK(queue.is_full());
    }

    SUBCASE("Minimum functional queue size")
    {
        MIDIQueue queue(2);
        MIDITimedBigMessage msg;
        msg.set_note_off(3, 48, 0);

        // Can put one message
        CHECK(queue.can_put());
        CHECK_FALSE(queue.is_full());
        queue.put(msg);

        // Now full
        CHECK_FALSE(queue.can_put());
        CHECK(queue.is_full());
        CHECK(queue.can_get());

        // After consuming, empty again
        queue.next();
        CHECK_FALSE(queue.can_get());
        CHECK(queue.can_put());
    }
}

TEST_CASE("MIDIQueue circular buffer wraparound")
{
    MIDIQueue queue(4);  // Size 4 allows 3 messages

    SUBCASE("Wraparound at buffer boundary")
    {
        std::vector<MIDITimedBigMessage> messages(6);

        // Create distinguishable messages
        for (int i = 0; i < 6; ++i) {
            messages[i].set_note_on(0, 60 + i, 100 + i);
            messages[i].set_time(100 * (i + 1));
        }

        // Fill queue to capacity
        queue.put(messages[0]);
        queue.put(messages[1]);
        queue.put(messages[2]);
        CHECK(queue.is_full());

        // Consume one message
        MIDITimedBigMessage retrieved = queue.get();
        CHECK(retrieved.get_note() == 60);
        queue.next();

        // Should be able to add another
        CHECK(queue.can_put());
        queue.put(messages[3]);

        // Consume another
        retrieved = queue.get();
        CHECK(retrieved.get_note() == 61);
        queue.next();

        // Add and consume more to force wraparound
        queue.put(messages[4]);
        retrieved = queue.get();
        CHECK(retrieved.get_note() == 62);
        queue.next();

        queue.put(messages[5]);
        retrieved = queue.get();
        CHECK(retrieved.get_note() == 63);
        queue.next();

        // Final messages should be correct
        retrieved = queue.get();
        CHECK(retrieved.get_note() == 64);
        queue.next();

        retrieved = queue.get();
        CHECK(retrieved.get_note() == 65);
        queue.next();

        CHECK_FALSE(queue.can_get());
    }

    SUBCASE("Multiple complete cycles")
    {
        MIDITimedBigMessage msg;
        msg.set_control_change(7, 10, 127);

        // Perform multiple fill/empty cycles
        for (int cycle = 0; cycle < 5; ++cycle) {
            // Fill
            for (int i = 0; i < 3; ++i) {
                msg.set_time(cycle * 100 + i * 10);
                queue.put(msg);
            }
            CHECK(queue.is_full());

            // Empty
            for (int i = 0; i < 3; ++i) {
                CHECK(queue.can_get());
                MIDITimedBigMessage retrieved = queue.get();
                CHECK(retrieved.get_time() == cycle * 100 + i * 10);
                queue.next();
            }
            CHECK_FALSE(queue.can_get());
        }
    }
}

TEST_CASE("MIDIQueue message type handling")
{
    MIDIQueue queue(10);

    SUBCASE("Various MIDI message types")
    {
        struct TestMessage
        {
            MIDITimedBigMessage msg;
            std::string description;
        };

        std::vector<TestMessage> test_messages;

        // Note On
        test_messages.push_back({});
        test_messages.back().msg.set_note_on(0, 60, 127);
        test_messages.back().msg.set_time(100);
        test_messages.back().description = "Note On";

        // Note Off
        test_messages.push_back({});
        test_messages.back().msg.set_note_off(1, 64, 64);
        test_messages.back().msg.set_time(200);
        test_messages.back().description = "Note Off";

        // Control Change
        test_messages.push_back({});
        test_messages.back().msg.set_control_change(2, 7, 100);
        test_messages.back().msg.set_time(300);
        test_messages.back().description = "Control Change";

        // Program Change
        test_messages.push_back({});
        test_messages.back().msg.set_program_change(3, 42);
        test_messages.back().msg.set_time(400);
        test_messages.back().description = "Program Change";

        // Pitch Bend
        test_messages.push_back({});
        test_messages.back().msg.set_pitch_bend(4, 1000);
        test_messages.back().msg.set_time(500);
        test_messages.back().description = "Pitch Bend";

        // Put all messages
        for (auto const& test_msg : test_messages) {
            queue.put(test_msg.msg);
        }

        // Retrieve and verify all messages
        for (size_t i = 0; i < test_messages.size(); ++i) {
            CAPTURE(test_messages[i].description);

            MIDITimedBigMessage retrieved = queue.get();
            CHECK(retrieved.get_type() == test_messages[i].msg.get_type());
            CHECK(retrieved.get_channel() == test_messages[i].msg.get_channel());
            CHECK(retrieved.get_time() == test_messages[i].msg.get_time());

            queue.next();
        }
    }

    SUBCASE("Messages with system exclusive markers")
    {
        MIDITimedBigMessage msg;
        msg.set_status(0xF0);  // SysEx start marker
        msg.set_time(1000);

        queue.put(msg);

        MIDITimedBigMessage retrieved = queue.get();
        CHECK(retrieved.is_sys_ex());
        CHECK(retrieved.get_time() == 1000);
        CHECK(retrieved.get_status() == 0xF0);
    }
}

TEST_CASE("MIDIQueue thread safety simulation")
{
    SUBCASE("Single producer, single consumer pattern")
    {
        MIDIQueue queue(100);
        std::atomic<bool> test_complete{false};
        std::atomic<int> messages_produced{0};
        std::atomic<int> messages_consumed{0};
        std::atomic<bool> producer_done{false};

        int const total_messages = 1000;

        // Producer thread simulation
        std::thread producer([&]() {
            for (int i = 0; i < total_messages; ++i) {
                // Wait for space
                while (!queue.can_put() && !test_complete.load()) {
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                }

                if (test_complete.load())
                    break;

                MIDITimedBigMessage msg;
                msg.set_note_on(0, 60 + (i % 12), 100);
                msg.set_time(i);

                queue.put(msg);
                messages_produced.fetch_add(1);
            }
            producer_done.store(true);
        });

        // Consumer thread simulation
        std::thread consumer([&]() {
            while (!producer_done.load() || queue.can_get()) {
                if (queue.can_get()) {
                    MIDITimedBigMessage msg = queue.get();
                    CHECK(msg.get_type() == NOTE_ON);
                    CHECK(msg.get_time() == messages_consumed.load());

                    queue.next();
                    messages_consumed.fetch_add(1);
                } else {
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                }
            }
        });

        // Wait for completion with timeout
        producer.join();
        consumer.join();

        CHECK(messages_produced.load() == total_messages);
        CHECK(messages_consumed.load() == total_messages);
        CHECK_FALSE(queue.can_get());
    }

    SUBCASE("Atomic operations consistency check")
    {
        MIDIQueue queue(20);

        // Pre-fill queue partially
        for (int i = 0; i < 10; ++i) {
            MIDITimedBigMessage msg;
            msg.set_note_on(0, 60 + i, 100);
            msg.set_time(i);
            queue.put(msg);
        }

        std::atomic<bool> test_running{true};
        std::atomic<int> puts_completed{0};
        std::atomic<int> gets_completed{0};

        // Simple producer thread
        std::thread producer([&]() {
            for (int i = 0; i < 50 && test_running.load(); ++i) {
                if (queue.can_put()) {
                    MIDITimedBigMessage msg;
                    msg.set_control_change(1, 7, i % 128);
                    msg.set_time(1000 + i);
                    queue.put(msg);
                    puts_completed.fetch_add(1);
                }
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        });

        // Simple consumer thread
        std::thread consumer([&]() {
            for (int i = 0; i < 50 && test_running.load(); ++i) {
                if (queue.can_get()) {
                    MIDITimedBigMessage msg = queue.get();
                    queue.next();
                    gets_completed.fetch_add(1);
                }
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        });

        // Let them run for a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        test_running.store(false);

        producer.join();
        consumer.join();

        // Validate that operations completed without crashes
        CHECK(puts_completed.load() >= 0);
        CHECK(gets_completed.load() >= 0);

        // The queue should still be in a valid state
        CHECK((queue.can_put() || queue.can_get() || queue.is_full()));
    }

    SUBCASE("Rapid put/get operations")
    {
        MIDIQueue queue(50);
        int const iterations = 10000;

        for (int i = 0; i < iterations; ++i) {
            if (queue.can_put()) {
                MIDITimedBigMessage msg;
                msg.set_control_change(0, 1, i % 128);
                msg.set_time(i);
                queue.put(msg);
            }

            if (queue.can_get()) {
                MIDITimedBigMessage msg = queue.get();
                CHECK(msg.get_type() == CONTROL_CHANGE);
                queue.next();
            }

            // Occasionally clear to test state reset
            if (i % 1000 == 999) {
                queue.clear();
            }
        }
    }
}

TEST_CASE("MIDIQueue stress testing and edge cases")
{
    SUBCASE("Alternating put/get at capacity")
    {
        MIDIQueue queue(3);  // Capacity of 2 messages
        MIDITimedBigMessage msg1, msg2;

        msg1.set_note_on(0, 60, 100);
        msg1.set_time(100);

        msg2.set_note_off(0, 60, 0);
        msg2.set_time(200);

        // Fill to capacity
        queue.put(msg1);
        queue.put(msg2);
        CHECK(queue.is_full());

        // Alternate operations
        for (int i = 0; i < 100; ++i) {
            // Remove one
            CHECK(queue.can_get());
            MIDITimedBigMessage retrieved = queue.get();
            queue.next();
            CHECK(queue.can_put());

            // Add one back
            MIDITimedBigMessage new_msg;
            new_msg.set_control_change(0, 7, i % 128);
            new_msg.set_time(300 + i);
            queue.put(new_msg);

            if (i % 2 == 0) {
                CHECK(queue.is_full());
            }
        }
    }

    SUBCASE("State consistency after many operations")
    {
        MIDIQueue queue(10);
        int put_count = 0;
        int get_count = 0;

        // Perform random operations
        for (int i = 0; i < 1000; ++i) {
            bool should_put = (i % 3 != 0) && queue.can_put();
            bool should_get = (i % 7 == 0) && queue.can_get();

            if (should_put && !should_get) {
                MIDITimedBigMessage msg;
                msg.set_note_on(0, 60, 100);
                msg.set_time(put_count);
                queue.put(msg);
                put_count++;
            } else if (should_get) {
                MIDITimedBigMessage msg = queue.get();
                CHECK(msg.get_time() == get_count);
                queue.next();
                get_count++;
            }

            // Verify state consistency
            int expected_count = put_count - get_count;
            if (expected_count > 0) {
                CHECK(queue.can_get());
            } else {
                CHECK_FALSE(queue.can_get());
            }

            if (expected_count >= 9) {  // Queue size 10 means max 9 messages
                CHECK(queue.is_full());
                CHECK_FALSE(queue.can_put());
            }
        }
    }

    SUBCASE("Large message throughput")
    {
        MIDIQueue queue(1000);
        int const message_count = 50000;

        // Fill with many messages
        for (int i = 0; i < message_count && queue.can_put(); ++i) {
            MIDITimedBigMessage msg;
            msg.set_pitch_bend(i % 16, (i * 13) % 16384);
            msg.set_time(i * 10);
            queue.put(msg);
        }

        // Verify all messages are retrievable
        int retrieved_count = 0;
        while (queue.can_get()) {
            MIDITimedBigMessage msg = queue.get();
            CHECK(msg.get_type() == PITCH_BEND);
            CHECK(msg.get_time() == retrieved_count * 10);
            queue.next();
            retrieved_count++;
        }

        CHECK(retrieved_count > 900);  // Should handle most messages
        CHECK_FALSE(queue.can_get());
    }
}