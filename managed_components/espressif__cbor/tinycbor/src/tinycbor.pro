TEMPLATE = lib
CONFIG += static warn_on
CONFIG -= qt
DESTDIR = ../lib

!msvc:QMAKE_CFLAGS += \
    -Werror=incompatible-pointer-types \
    -Werror=implicit-function-declaration \
    -Werror=int-conversion
include(src.pri)
