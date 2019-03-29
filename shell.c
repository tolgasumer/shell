#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void *ExecuteCmd(char *cmd)
{
    printf("\n(debug):thread_id:%d - executing command:%s\n", pthread_self(), cmd);
    system(cmd);

    pthread_exit(NULL);
}

int main()
{
    int mode = 0;
    //prompt the user for mode selection
    while (mode != 1 && mode != 2)
    {
        printf("1. Interactive Mode\n2. Batch Mode\nType in the number of the mode that you want to use:");
        scanf(" %d", &mode);
    }

    //preparing to get input
    char input[500];
    char *inputptr;
    inputptr = input;

    //batch mode specific containers
    FILE *fp;
    char buffer[500];

    if (mode == 1) //interactive mode - ask the user for input
    {
        //printf("\ntolgashell:>");
        fgets(input, sizeof(input), stdin); //get input string
    }
    else if (mode == 2) //batch mode - read one line from the file
    {
        printf("\nEnter the batch file's name (file must be on the same folder as the binary): ");
        char *filename;
        scanf("%s",filename);
        printf("Starting to execute the batch named:%s\n",filename);

        fp = fopen(filename, "r");
        if (fp == NULL)
        {
            printf("Could not open file %s", filename);
            return 1;
        }
        else
        {
            fgets(buffer, 500, fp);
            strcpy(input, buffer);
        }
    }

    // parsing starts here
    while (strcmp(inputptr, "quit") != 0)
    {

        char *copy = strdup(input);
        char *delim = ";|";
        char *param = strtok(input, delim); //first tokenization

        char *parsedOperands[100];
        char *parsedParams[100];
        int countOfOperands = 0;

        //ugly loop to seperate params and operands :(
        int i = 0;
        int j = 0;
        while (param)
        {
            parsedParams[i++] = param;                                 //get the command without the operand (i.e. "ls -l" or "pwd")
            parsedOperands[j++] = copy[param - input + strlen(param)]; //parsing to get the operand only (i.e. ';' or '|')
            param = strtok(NULL, delim);
        }
        i = 0;
        j = 0;

        //count the characters in the parsedOperands array
        //will be useful to know how many commands we need to parse beforehand
        for (int i = 0; i < sizeof(*parsedOperands); i++)
        {
            countOfOperands += strlen(&parsedOperands[i]);
        }
        //printf("\n(debug):countOfOperands: %d\n", countOfOperands);

        int quitflag = 0;
        //checking for the "quit" flag on input parameters
        for (int i = 0; i <= countOfOperands; i++)
        {
            if (strncmp(parsedParams[i], "quit", 4) == 0)
            {
                quitflag = 1;
                printf("(debug):***********will quit after commands are executed***********\n");
            }
        }

        char *pipeCmd[100];  //defining a string to store the pipe commands
        pthread_t thread_id; //defining thread

        //loop to go over the parsed input to find piped commands and others
        //piped commands -> execute sequential (same thread)
        //normal commands -> execute concurrently (different threads)
        for (int i = 0; i <= countOfOperands; i++)
        {
            //printf("sizeOfparsedOperands:%s", sizeof(*parsedOperands));
            if (countOfOperands == 0 && parsedParams[0] != NULL && strlen(parsedParams[0]) > 1) //if input has only one command (no ';' or '|')
            {
                pthread_create(&thread_id, NULL, ExecuteCmd, parsedParams[0]);
                parsedParams[0] = NULL;
            }
            else if (strcmp(&parsedOperands[i], "|") == 0)
            {
                printf("\n(debug):pipe found:");
                printf("%s", parsedParams[i]);
                strcpy(pipeCmd, parsedParams[i]);    //copying the first param of the pipe operation
                strcat(pipeCmd, &parsedOperands[i]); //copy operand ('|')
                //check if the piping continues
                while (strcmp(&parsedOperands[i], "|") == 0)
                {
                    i++;
                    printf(" | %s", parsedParams[i]);
                    strcat(pipeCmd, parsedParams[i]);
                    strcat(pipeCmd, &parsedOperands[i]);
                }
                pthread_create(&thread_id, NULL, ExecuteCmd, pipeCmd);
            }
            else
            {
                if (parsedParams[i] && strncmp(parsedParams[i], "quit", 4) != 0) //execute the command if its not quit
                {
                    if(strlen(parsedParams[i])>1)
                    {
                        pthread_create(&thread_id, NULL, ExecuteCmd, parsedParams[i]);
                    }
                }
            }
        }
        sleep(1); //make sure all the threads finish their work before prompting the user again for new commands

        //make sure the input arrays are cleared
        memset(&parsedParams, 0, sizeof(parsedParams));
        memset(&parsedOperands, 0, sizeof(parsedOperands));

        if (quitflag) //exit if the quitflag is true
        {
            printf("(debug):***********QUITTING***********\n");
            exit(0);
        }


        if (mode == 1) //INTERACTIVE MODE - continue with getting another input from the user
        {
            printf("\ntolgashell:>");
            gets(inputptr); //get input string
        }
        if (mode == 2) //BATCH MODE - read another line from the batch file
        {
            if (fgets(buffer, sizeof(buffer),fp) != NULL)
            {
                strcpy(input, buffer);
            }
            else //exit if the file is ended
            {
                printf("\nBatch executed.");
                fclose(fp);
                exit(0);
            }
        }
    }
}
