.PHONY: clean
clean:
	rm -f cscope.out
	make cscope.out

findpath = ~/llvm/llvm-2.8/

cscope.out:
	find $(findpath) -name *.c > filenames
	find $(findpath) -name *.h >> filenames
	find $(findpath) -name *.cpp >> filenames

	ctags -L filenames
	cscope -i filenames

