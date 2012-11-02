TOP = ../../..

QT -= core gui
CONFIG += link_prl debug
win32:QT += core
win32:CONFIG+=console

TEMPLATE = app

DEFINES += 

INCLUDEPATH += $$TOP/include 

DEPENDPATH += $$INCLUDEPATH

unix:LIBS += -L../jdksmidi -ljdksmidi

win32 {
  CONFIG(release):LIBS += -L../lib/release -ljdksmidi
  CONFIG(debug):LIBS += -L../lib/debug -ljdksmidi
  LIBS += pcap
}

win32:LIBS+=
macx:LIBS+=

macx {
 MAC_SDK  = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk
 if( !exists( $$MAC_SDK) ) {
  error("The selected Mac OSX SDK does not exist at $$MAC_SDK!")
 }
 macx:QMAKE_MAC_SDK = $$MAC_SDK
}


