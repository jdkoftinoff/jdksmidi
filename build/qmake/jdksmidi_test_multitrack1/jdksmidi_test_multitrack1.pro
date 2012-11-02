! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_test_multitrack1

SOURCES += $$TOPDIR/examples/jdksmidi_test_multitrack1.cpp

HEADERS += $$TOPDIR/include/*/*.h

