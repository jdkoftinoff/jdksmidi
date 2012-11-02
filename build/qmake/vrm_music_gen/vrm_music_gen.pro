! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=vrm_music_gen

SOURCES += $$TOPDIR/examples/vrm_music_gen.cpp

HEADERS += $$TOPDIR/include/*/*.h

