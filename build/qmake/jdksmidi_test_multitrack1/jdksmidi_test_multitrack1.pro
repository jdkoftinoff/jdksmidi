CONFIG-=app_bundle

TOP = ../../..

! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_test_multitrack1

SOURCES += $$TOP/examples/jdksmidi_test_multitrack1.cpp

HEADERS += $$TOP/include/*/*.h

