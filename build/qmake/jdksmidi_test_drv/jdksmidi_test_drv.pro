! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_test_drv

SOURCES += $$TOPDIR/examples/jdksmidi_test_drv.cpp

HEADERS += $$TOPDIR/include/*/*.h

