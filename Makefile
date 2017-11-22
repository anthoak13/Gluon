CHROMA=$(SOFTDIR)/chroma-momsrc-single
CONFIG=$(CHROMA)/bin/chroma-config
CXX=mpicxx
CXXFLAGS=$(shell $(CONFIG) --cxxflags) -I.
LDFLAGS=$(shell $(CONFIG) --ldflags)
LIBS=$(shell $(CONFIG) --libs)


HDRS=inline_Ob.h

OBJS=chroma.o \
     inline_Ob.o

chroma: $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJS) $(LDFLAGS) $(LIBS)

%.o: %.cc $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< 

clean:
	rm -rf chroma $(OBJS) *~

stripOb: stripOb.cc $(HDRS)
	$(CXX) -o $@ $(CXXFLAGS) -Wno-cpp $< $(LDFLAGS) $(LIBS)

processOb: processOb.cc
	$(CXX) -o $@ $(CXXFLAGS) -Wno-cpp $<
