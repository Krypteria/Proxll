MAKEFLAGS += -s

FLAGS = -s -masm=intel -static
INCLUDE = -I ./include 
SOURCE = $(wildcard ./src/*.cpp)

EXECUTABLE = proxllGen.exe
	
x64:
	echo [*] Compiling proxllGen.exe (x64)
	
	x86_64-w64-mingw32-g++ -w  $(SOURCE) $(INCLUDE) $(FLAGS) -o ./bin/$(EXECUTABLE)
		
	echo [+] ProxllGen.exe compiled at \proxllGen\bin