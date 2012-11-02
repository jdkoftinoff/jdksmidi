! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_test_multitrack

SOURCES += $$TOPDIR/examples/jdksmidi_test_multitrack.cpp

HEADERS += $$TOPDIR/include/*/*.h

