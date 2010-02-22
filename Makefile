default: all

clean:
	$(RM) extensions
	$(RM) extensions.o

all: extensions

extensions: extensions.o
	$(CXX) $(LDFLAGS) $^ -o $@

extensions.o: extensions.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
