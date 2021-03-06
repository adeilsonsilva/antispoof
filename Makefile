# Paths
OPENCV_PATH=/usr/local

# Programs
CC=
CXX=g++

# Flags
ARCH_FLAGS=
CFLAGS= -std=c++14 -Wextra -Wall -pedantic-errors $(ARCH_FLAGS) -O3 -Wno-long-long
LDFLAGS=$(ARCH_FLAGS)
DEFINES=
INCLUDES=-I$(OPENCV_PATH)/include -Iinclude/
LIBRARIES=-L$(OPENCV_PATH)/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect

# Files which require compiling
SOURCE_FILES=\
	src/lib/FaceAntispoof/data.cpp\
    src/lib/FaceAntispoof/validation.cpp\
    src/lib/FaceAntispoof/misc.cpp\
    src/lib/Vendor/lbph.cpp\
    src/lib/Vendor/detectObject.cpp\
    src/lib/Vendor/preprocessFace.cpp\
    src/lib/Kinect/load.cpp

# Source files which contain a int main(..) function
SOURCE_FILES_WITH_MAIN=src/exe/faceAntispoof.cpp

# End Configuration
SOURCE_OBJECTS=$(patsubst %.cpp,%.o,$(SOURCE_FILES))

ALL_OBJECTS=\
	$(SOURCE_OBJECTS) \
	$(patsubst %.cpp,%.o,$(SOURCE_FILES_WITH_MAIN))

DEPENDENCY_FILES=\
	$(patsubst %.o,%.d,$(ALL_OBJECTS))

all: bin/faceAntispoof

%.o: %.cpp Makefile
	@# Make dependecy file
	$(CXX) -MM -MT $@ -MF $(patsubst %.cpp,%.d,$<) $(CFLAGS) $(DEFINES) $(INCLUDES) $<
	@# Compile
	$(CXX) $(CFLAGS) $(DEFINES) -c -o $@ $< $(INCLUDES)

-include $(DEPENDENCY_FILES)

bin/faceAntispoof: $(ALL_OBJECTS)
	$(CXX) $(LDFLAGS)  -o $@ $(ALL_OBJECTS) $(LIBRARIES) `pkg-config --cflags --libs opencv` -lz -pthread -lpng -lX11 -g #-lfreenect2

.PHONY: clean
clean:
	@echo "Cleaning"
	@for pattern in '*~' '*.o' '*.d' ; do \
		find . -name "$$pattern" | xargs rm ; \
	done
