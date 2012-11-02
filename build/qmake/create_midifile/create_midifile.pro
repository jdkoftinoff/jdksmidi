! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=create_midifile

SOURCES += $$TOPDIR/examples/create_midifile.cpp

HEADERS += $$TOPDIR/include/*/*.h

