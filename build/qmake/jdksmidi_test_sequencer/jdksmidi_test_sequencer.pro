CONFIG-=app_bundle

TOP = ../../..

! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_test_sequencer

SOURCES += $$TOP/examples/jdksmidi_test_sequencer.cpp

HEADERS += $$TOP/include/*/*.h

