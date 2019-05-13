#include "types.h"
#include "stat.h"
#include "user.h"

char buff[512];

void tail(int fd, uint limit)
{
    int n, i, start;
    int lines = 0;
    char *buff_2;
    buff_2 = (char*) malloc (100000);
    int buff_2_size = 0;
    int counter = 0;

    while ((n = read(fd, buff, sizeof(buff))) > 0)
    {
        for (i = 0; i < n; i++,) // beginning of arr, numBytes gets initialized to 1 since the first element is 1 byte
        {
            buff_2[buff_2_size] = (char)buff[i];
            buff_2_size++;
            if (buff[i] == '\n') // if the char is a newline
            {
                lines++;
            } 
        }
    }
    if (n < 0)
    {
        printf(1, "tail: read error\n");
        exit();
    }
    start = lines - limit;
    int j = 0;
    for (j = 0; j < buff_2_size; j++)
    {
        if (counter >= start)
        {
            printf(1,"%c",buff_2[j]);
        }
        if (buff_2[i] == '\n')
        {
            counter++;
        }
    }
    free (buff_2);
}

ushort
notValid(char *arr) // checks if string contains a non number
{
    int i;
    for (i = 0; i < strlen(arr); i++)
    {
        if (arr[i] < '0' || arr[i] > '9') // non number
            return arr[i]; // not Valid number
    }
    return 0; // valid
}

int main(int argc, char *argv[])
{
    int fd, i, param = 1, limit = 10; // param initialized to 1 since 0th param (as an index) is the program name, limit default is 10

    if (argc <= 1) // only tail
    {
        tail(0, limit);
        exit();
    }
    else if (*argv[param] == '-') // if second param starts with '-'
    {
        argv[param]++; // increment to remove '-'
        char sym;
        if ((sym = notValid(argv[param]))) {
            printf(1, "tail: non-valid number in nLines flag: '%c'\n", sym);
            exit();
        }
        limit = atoi(argv[param]);
        if (argc <= 2) // no file, only tail and -n
        {
            tail(0, limit);
            exit();
        }
        ++param; // move onto third param (file)
    }

    for (i = param; i < argc; i++) // 1 or more file params to tail
    {
        if ((fd = open(argv[i], 0)) < 0)
        {
            printf(1, "tail: cannot open %s\n", argv[i]);
            exit();
        }
        tail(fd, limit);
        close(fd);
    }
    exit();
}
