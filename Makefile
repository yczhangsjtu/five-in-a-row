ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
  FLAGS=-L"/System/Library/Frameworks/OpenGL.framework/Libraries" -framework GLUT -framework OpenGL -lglut -lGL -lGLU
else
  FLAGS=-lglut -lGL -lGLU
endif

Five: Five.cpp Five.h
	g++ -o $@ $< ${FLAGS}
