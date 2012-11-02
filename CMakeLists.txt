cmake_minimum_required (VERSION 2.6) 
project (JDKSMIDI)

include_directories( ${JDKSMIDI_SOURCE_DIR}/include )

add_library (jdksmidi src/jdksmidi_advancedsequencer.cpp src/jdksmidi_driver.cpp src/jdksmidi_driverdump.cpp src/jdksmidi_edittrack.cpp src/jdksmidi_file.cpp src/jdksmidi_fileread.cpp src/jdksmidi_filereadmultitrack.cpp src/jdksmidi_fileshow.cpp src/jdksmidi_filewrite.cpp src/jdksmidi_filewritemultitrack.cpp src/jdksmidi_keysig.cpp src/jdksmidi_manager.cpp src/jdksmidi_matrix.cpp src/jdksmidi_midi.cpp src/jdksmidi_msg.cpp src/jdksmidi_multitrack.cpp src/jdksmidi_parser.cpp src/jdksmidi_process.cpp src/jdksmidi_queue.cpp src/jdksmidi_sequencer.cpp src/jdksmidi_showcontrol.cpp src/jdksmidi_showcontrolhandler.cpp src/jdksmidi_smpte.cpp src/jdksmidi_sysex.cpp src/jdksmidi_tempo.cpp src/jdksmidi_tick.cpp src/jdksmidi_track.cpp src/jdksmidi_utils.cpp)

link_directories( ${JDKSMIDI_BINARY_DIR} )

add_executable(create_midifile examples/create_midifile.cpp)
target_link_libraries(create_midifile jdksmidi)

add_executable(jdksmidi_rewrite_midifile examples/jdksmidi_rewrite_midifile.cpp)
target_link_libraries(jdksmidi_rewrite_midifile jdksmidi)

add_executable(jdksmidi_test_drv examples/jdksmidi_test_drv.cpp)
target_link_libraries(jdksmidi_test_drv jdksmidi)

add_executable(jdksmidi_test_multitrack examples/jdksmidi_test_multitrack.cpp)
target_link_libraries(jdksmidi_test_multitrack jdksmidi)

add_executable(jdksmidi_test_multitrack1 examples/jdksmidi_test_multitrack1.cpp)
target_link_libraries(jdksmidi_test_multitrack1 jdksmidi)

add_executable(jdksmidi_test_parse examples/jdksmidi_test_parse.cpp)
target_link_libraries(jdksmidi_test_parse jdksmidi)

add_executable(jdksmidi_test_sequencer examples/jdksmidi_test_sequencer.cpp)
target_link_libraries(jdksmidi_test_sequencer jdksmidi)

add_executable(jdksmidi_test_show examples/jdksmidi_test_show.cpp)
target_link_libraries(jdksmidi_test_show jdksmidi)

add_executable(rewrite_midifile examples/rewrite_midifile.cpp)
target_link_libraries(rewrite_midifile jdksmidi)

add_executable(vrm_music_gen examples/vrm_music_gen.cpp)
target_link_libraries(vrm_music_gen jdksmidi)


