! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_rewrite_midifile

SOURCES += $$TOPDIR/examples/jdksmidi_rewrite_midifile.cpp

HEADERS += $$TOPDIR/include/*/*.h

