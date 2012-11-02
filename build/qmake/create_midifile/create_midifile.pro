CONFIG-=app_bundle

TOP = ../../..

! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=create_midifile

SOURCES += $$TOP/examples/create_midifile.cpp

HEADERS += $$TOP/include/*/*.h

