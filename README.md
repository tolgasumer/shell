# shell
CMPE382 Project #1

-Summary
The codes purpose is to imitate the original bash command line. Initially, it will prompt the user to choose a mode when it launches (Interactive or Batch mode). After the user types in the mode they want to use (1 for interactive, 2 for batch mode). Interactive mode will take a line of input from the user using the terminal, whereas the batch mode asks the user to type in a filename (i.e. .txt or .bat) before executing the commands in the file line by line.

-Design overview
Code-wise, the two modes run in the same main loop and uses exactly the same functions, only thing that differs is the way of getting the initial input (user input or file). When the input is given, every line will be parsed to separate the operands (';' or '|') and parameters (i.e. 'ls -l','pwd') from each other, and store them in different arrays. If it reads a pipe operand ('|'), it will always check the operand after it to see if the pipeline continues. When the pipeline ends, the parser will spawn a single thread to execute the commands on the whole pipeline, whereas each command without piping will spawn their own thread and run concurrently.

-Error handling
Lines that have no commands between semi-colons (i.e. ';;;;;;ls'): This case has been prevented from causing any error by simply not parsing any any string that is NULL, '\0' or shorter than 1 character as a command.

The program will continue to execute the commands even if there are invalid commands on the line:

✓ mkdur a;mkdar o;mkdddar f;mkdir y   //first three commands are false, but the last true one will still execute

✓ ;;;;;;;;;ls -l  // there are semicolons at the start char, but “ls -l” will execute with no problem

✓ touch readme.txt;hudaverdi;nano readme.txt  // even if there is an unjustifiable command in the middle, it will still work to create a txt file and let the user edit it with nano afterwards

✓ ps-ax | grep Finder // piping works without problems

✓ quit command works properly and exits after executing all the commands. If a line contains the 'quit' command, it will execute all the commands on the line and quit afterwards.
