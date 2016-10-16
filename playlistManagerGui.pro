SOURCES += AlbumInfoWidget.cpp
HEADERS += AlbumInfoWidget.h

SOURCES += FileInfoWidget.cpp
HEADERS += FileInfoWidget.h

SOURCES += Filter.cpp
HEADERS += Filter.h

HEADERS += JukeboxPlaylist.h

SOURCES += main.cpp

SOURCES += MainWindow.cpp
HEADERS += MainWindow.h

SOURCES += XmlReader.cpp
HEADERS += XmlReader.h

SOURCES += XmlWriter.cpp
HEADERS += XmlWriter.h

# debug is required for sigsegv backtrace!
CONFIG += qt warn_on release
LIBS += -lexpat

CONFIG(release, debug | release) {
    DEFINES += INLINE
    QMAKE_CXXFLAGS += -O3 -Wno-error=unused-variable
    QMAKE_CXXFLAGS_RELEASE -= -O2 -Wno-error=unused-variable
} else {
    LIBS += -rdynamic
}

QMAKE_CXXFLAGS += -Werror

CONFIG(static) {
    message(IMPORTANT: Remove Qt shared library paths from LIBS!!!)
    QMAKE_CFLAGS += -static -Wno-error=unused-variable
    QMAKE_CXXFLAGS += -static -Wno-error=unused-variable
    LIBS += -lSM -lX11 -lXm -lXt -lXrender -lfontconfig
    LIBS += -lpng -ljpeg -lmng -ltiff -lrt -L/usr/lib/X11 -L/usr/X11R6/lib64
    LIBS += "-L${QTDIR}/lib-static"
}

BuildDirectory = build
DESTDIR     = "$$BuildDirectory"
OBJECTS_DIR = "$$BuildDirectory"
MOC_DIR     = "$$BuildDirectory"
RCC_DIR     = "$$BuildDirectory"
UI_DIR      = "$$BuildDirectory"
