PLUGINS =  \
			  modfilter.so \

FILTERNAME = MD5AnimExporter

LFLAGS = -O2 -Wall
CFLAGS = -fPIC -O2 -Wall -DMM3D_EDIT -I../../src -I../../src/libmm3d -I../../src/mm3dcore -I../.. -DPLUGIN

# Mac OS X flags. Uncomment the following line to compile on Mac OS X
#LFLAGS = -O2 -Wall -undefined suppress -bundle -flat_namespace

all: ${PLUGINS}
	cp modfilter.so ~/.mm3d/plugins/1.3/$(FILTERNAME).so
	
%.o: %.cc
	g++ ${CFLAGS} -c $<

%.so: %.o
	g++ ${LFLAGS} -shared -o $*.so $<

clean:
	rm -rf *.o *.so core core.*

