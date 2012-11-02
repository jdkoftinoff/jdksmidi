CONFIG-=app_bundle

TOP = ../../..

! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_test_multitrack

SOURCES += $$TOP/examples/jdksmidi_test_multitrack.cpp

HEADERS += $$TOP/include/*/*.h

