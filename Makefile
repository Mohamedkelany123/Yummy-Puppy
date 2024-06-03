all:
	$(MAKE) -f recursive_makefile all

clean:
	$(MAKE) -f recursive_makefile clean

closure_go: 
	bash closure_go.sh

