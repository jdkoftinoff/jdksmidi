CONFIG-=app_bundle

TOP = ../../..

! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=jdksmidi_rewrite_midifile

SOURCES += $$TOP/examples/jdksmidi_rewrite_midifile.cpp

HEADERS += $$TOP/include/*/*.h

