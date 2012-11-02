CONFIG-=app_bundle

TOP = ../../..

! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_test_drv

SOURCES += $$TOP/examples/jdksmidi_test_drv.cpp

HEADERS += $$TOP/include/*/*.h

