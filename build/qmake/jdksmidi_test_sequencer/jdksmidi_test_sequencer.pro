! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_test_sequencer

SOURCES += $$TOPDIR/examples/jdksmidi_test_sequencer.cpp

HEADERS += $$TOPDIR/include/*/*.h

