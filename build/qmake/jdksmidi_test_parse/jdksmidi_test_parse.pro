! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_test_parse

SOURCES += $$TOPDIR/examples/jdksmidi_test_parse.cpp

HEADERS += $$TOPDIR/include/*/*.h

