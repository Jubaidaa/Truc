EXE=main.exe
ARCH=random_cpp.tgz

.PHONY: clean

compile:
	@g++ *.cc -o $(EXE)

test: compile
	@./$(EXE)

clean:
	rm -f $(ARCH)
	rm -f *.o $(EXE)

archive: clean
	tar -cvf $(ARCH) *
