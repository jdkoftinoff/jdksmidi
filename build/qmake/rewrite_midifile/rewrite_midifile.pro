CONFIG-=app_bundle

TOP = ../../..

! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=rewrite_midifile

SOURCES += $$TOP/examples/rewrite_midifile.cpp

HEADERS += $$TOP/include/*/*.h

