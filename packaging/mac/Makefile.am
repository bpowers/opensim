if DARWIN

AM_OBJCFLAGS = -fmessage-length=0 -pipe \
               -isysroot /Developer/SDKs/MacOSX10.5.sdk \
               -mmacosx-version-min=10.5

macdir = $(ABS_TOP)/packaging/mac/Model.app/Contents/MacOS
mac_PROGRAMS = Model

Model_SOURCES = mac_driver.m
Model_LDFLAGS = -framework Python -framework Cocoa

endif
