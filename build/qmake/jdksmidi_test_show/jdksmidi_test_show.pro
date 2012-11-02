! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_test_show

SOURCES += $$TOPDIR/examples/jdksmidi_test_show.cpp

HEADERS += $$TOPDIR/include/*/*.h

