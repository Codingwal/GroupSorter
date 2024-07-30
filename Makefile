CXX = C:\\msys64\\ucrt64\\bin\\g++.exe
CXXFLAGS = -std=c++20 -Og -g3 -ggdb -fdiagnostics-color=always

srcDir = src/
buildDir = build/

objects = build/main.o build/groupSorter.o build/personParser.o build/hutParser.o build/errorHandler.o

.PHONY: all
all: $(objects)
	$(CXX) $^ -o main.exe

# Define a pattern rule that compiles every .c file into a .o file
$(objects): $(buildDir)%.o : $(srcDir)%.cpp
		$(CXX) -c $(CXXFLAGS) $< -o $@

# Delete executable
.PHONY: clean
clean:
	del main.exe