# Socket_Client-Server

Server generates random numbers. 
Count of random numbers is set by client.

Code compiled using "GCC -pthread" for multiThreads server file. 


# Execution:

Using command line 

    "./Server_oneThread <port-number>" 			--> to operate single thread server.
    OR "./Server_multiThreads <port-number>" 	--> to operate multiple threads server.
    
    "./Client localhost <port-number> <count of random number to generate>"  -->
    Rerun this command on as many terminals as wanted while operating multithreads server.
