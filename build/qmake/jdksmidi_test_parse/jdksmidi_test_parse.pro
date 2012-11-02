CONFIG-=app_bundle

TOP = ../../..

! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_test_parse

SOURCES += $$TOP/examples/jdksmidi_test_parse.cpp

HEADERS += $$TOP/include/*/*.h

