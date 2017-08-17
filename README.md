Intertask Concurrency and Communication Project:

This is a UNIX multithreaded and multi-processes C program which solves the producer consumer synchronization problem. 

Threads: 
The program uses shared memory to communicate between the threads. It also uses binary and counting semaphores in order to ensure there are no synchronization problems. 

Processes: 
The program uses message queues to communicate between processes. There was no need for using binary and counting semaphores since synchronization is handled by the mqueue library.

