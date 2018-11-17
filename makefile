CXX = g++

CXXFLAGS = -O3 -Wall -g -std=c++11

OBJECTS = main.o aes.o

main : $(OBJECTS)
	@echo linking $^ ...
	@echo Builded.
	@$(CXX) $(CXXFLAGS) -o main $^

%.o : %.cpp
	@echo compile $<...
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

# clean
.PHONY : clean

clean :
	-rm $(OBJECTS) main