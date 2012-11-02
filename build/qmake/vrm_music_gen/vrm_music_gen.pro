CONFIG-=app_bundle

TOP = ../../..

! include( ../common.pri ) {
  error( need common.pri file )
}


TARGET=vrm_music_gen

SOURCES += $$TOP/examples/vrm_music_gen.cpp

HEADERS += $$TOP/include/*/*.h

