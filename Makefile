FLAGS = -Wall -Werror -std=c++11

src = $(wildcard *.cpp)
objects = $(patsubst %.cpp, %.o, $(src))


ifeq ($(OS), Windows_NT)
	RM = del
	target = FlvExploere.exe
else
	RM = rm -f
	target = FlvExploere
endif


$(target): $(objects)
	g++  $^ $(FLAGS) -o $@


$(objects):%.o : %.cpp 
	g++ -c $(FLAGS)  $<  -o $@


.PHONY:clean
clean:
	$(RM) $(objects)
	$(RM) $(target)
