CPPFLAGS+=-std=c++0x -I../../
LDFLAGS+=
CXX=g++ -g
SRCS=main.cpp
EXE=a
TEST_SRCS=test.cpp
TEST_EXE=test

all: run

compile: $(SRCS)
	$(CXX) $(CPPFLAGS) $(SRCS) -o $(EXE) $(LDFLAGS)

compile_test: $(TEST_SRCS)
	$(CXX) $(CPPFLAGS) $(TEST_SRCS) -o $(TEST_EXE) $(LDFLAGS)

clean:
	rm $(EXE) *.o *.so

run: compile
	./$(EXE)

run_test: compile_test
	./$(TEST_EXE)
