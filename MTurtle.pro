TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

#win32:{
## SDL
#INCLUDEPATH+=C:/SDL2-2.0.0/include
#LIBS +=-LC:/SDL2-2.0.0/lib/x86 -lSDL2
#LIBS +=-LC:/SDL2-2.0.0/lib/x86 -lSDL2main
#}
#linux:{
## SDL
#INCLUDEPATH+= /SDL2-2.0.0/include
#LIBS +=-L/SDL2-2.0.0/lib/x86 -lSDL2
#LIBS +=-L/SDL2-2.0.0/lib/x86 -lSDL2main
##LIBS +=-L/usr/lib/x86_64-linux-gnu/libSDL-1.2.so.0.11.4
#}

SOURCES += main.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    MTurtle.h
