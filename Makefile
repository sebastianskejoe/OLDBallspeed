P=Ballspeed
OBJECTS=main.o hough.o window.o frame.o match.o output.o
CFLAGS = -g -Wall -O0
LDLIBS = -lopencv_core -lopencv_highgui -lopencv_calib3d -lopencv_nonfree -lopencv_imgproc -lopencv_features2d -lGL -lglut -lm
 
$(P): $(OBJECTS)
	  $(LINK.o) $^ $(LDLIBS) -o $@

clean:
	  rm $(OBJECTS)
