#VPATH= src:test
CFLAGS= -Wall -Wextra -pedantic -g
CXX=g++
TEST_LIBS=-lgtest -lgtest_main
LIBS=
FRAMEWORKS =
OBJECT_FILES =
OBJECT_SPEC_FILES =
APP = Assignment
# spec/views/*.o src/controllers/*.o src/views/*.o


test: all model_specs
	rm -f ./tests
	$(CXX) -g $(OBJECT_FILES) $(OBJECT_SPEC_FILES) $(TEST_LIBS) $(FRAMEWORKS) $(LIBS) -o tests
	./tests

model_specs:

#valgrind: test
#	valgrind --leak-check=yes ./tests

#models: $(OBJECT_FILES)


client: client.o

server: server.o

all: client server

clean:
	rm -rf main.o $(OBJECT_FILES) $(OBJECT_SPEC_FILES) $(APP) tests *.o client server

.cpp.o:
	$(CXX) -c $(CFLAGS) $< -o $@

