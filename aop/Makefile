ARCH=storage_java.tgz

.PHONY: clean

compile:
	javac *.java

test: compile
	java Main

clean:
	rm -f $(ARCH)
	rm -f *.class

archive: clean
	tar -cvf $(ARCH) *
