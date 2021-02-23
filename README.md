Name: Peter Massarello
Class: CS4760

# CONCURRENT UNIX PROCESSES AND SHARED MEMORY

GOAL OF PROJECT
------------------------------------------------------------------------------

The goal of this project is to become more familiar with concurrent processing
in Linux using shared memory. We are to use fork() and exec() to make this 
happen.

------------------------------------------------------------------------------

BUGS/CHANGES
------------------------------------------------------------------------------

-Sometimes on the first run the program gets stuck after the first depth, but
if you were to rerun it, the program would run completely fine and display
the correct value.

-I had some issues passing along certain variables to bin_adder.c so I decided 
to pass them along with the exec to bin_adder.c. The extra values I pass are
	-Size of the array
	-Id of child
	-Max number of processes
	-Current exponent value of 'power'

-My program makes the final addition at the very end then displays the total.
This is done outside the exec loop mostly because I was running low on time
to turn this in on time.

-My file which holds the numbers is not a generated file from the program.
I was confused from the if we were supposed to generated it progromatically or 
just by hand. I went with the latter method. The file still follows the 
constraints given of [0, 256) and one number per line.

-Time printed to log file is only as precise as seconds, therefore there is
not a real difference in the log file of time. I also couldn't get the PID
to be passed to the critical section. So the log displays the two numbers 
being added and the time when they were added.

-------------------------------------------------------------------------------

FUNCTIONS USED
-------------------------------------------------------------------------------

IN MASTER.C:

kill_pids:

	- Kills all left over processes.

kill_func:

	- Final function called that frees all shared memory properly.

ctrl_c:

	- Function called when ctrl-c signal is caught.

timer_func:

	- Function called when alarm signal is caught.

init_pid:

	- Initializes all PID's in pid_list to 0.

init_state:

	- Initializes all states to 'idle'.

help_menu:

	- Displays the help menu.

is_power_of_2:

	- Checks to see if count of numbers is a power of 2 and if not, extends
	the count to the next power of 2 and fills in with zeros.

get_depth:

	- Gets depth of binary tree.

get_num_count:

	- Gets full count of numbers from the file.

max_check:

	- Checks to see if given -s option is higher than max num of processes.
	If it is, resets back to 19.

find_empty:

	- Checks the entire PID list and searches for an empty spot, returns
	that index.

process_check:

	- Checks to see if there are any left over, active processes.

remove_pid:

	- Removes the specified PID from the list of PID's.

my_sigchild_handler:

	- Handles the death signal of children. 
	- Taken from stack overflow.

IN BIN_ADDER.C

kill_func:

	- Detaches all pointers from shared memory.

crit_sec:

	- Process performed during critical section.
	- Appends to adder_log and writes the variables being added with the
	time.

-------------------------------------------------------------------------------
