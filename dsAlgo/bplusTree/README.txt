
B+ Tree
-- ----

One of the important data structures in DBMS concepts.

Implementation:
	A generic B+ Tree whose size (number of keys in a node) can be adjusted.
	Randomly generates `n' entries (<number, info> fields) less than a SENTINEL
	 value, inserts them into a list and creates a B+ Index on the `info' field.

Commands:

a. `cd' to the directory.
b. Use `make' to build the executable.
c. Run the executable as:
		$ ./bplus.out <no. of elements>  # $ ./bplus.out 10
d. Use dotty or kgraphviewer to view the .dot output that gets generated.

Cheers:)VIKRAM
Thanks for reading till here...
