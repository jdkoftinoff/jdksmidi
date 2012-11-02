! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=rewrite_midifile

SOURCES += $$TOPDIR/examples/rewrite_midifile.cpp

HEADERS += $$TOPDIR/include/*/*.h

