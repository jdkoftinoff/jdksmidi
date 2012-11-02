CONFIG-=app_bundle

TOP = ../../..

! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_test_show

SOURCES += $$TOP/examples/jdksmidi_test_show.cpp

HEADERS += $$TOP/include/*/*.h

