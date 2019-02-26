#include "types.h"
#include "stat.h"
#include "user.h"

char buff[512];

void head(int fd, uint limit)
{
    int n, i, start, numBytes;
    int lines = 0;

    while ((n = read(fd, buff, sizeof(buff))) > 0)
    {
        for (i = 0, start = 0, numBytes=1; i < n; i++, numBytes++) // beginning of arr, numBytes gets initialized to 1 since the first element is 1 byte
        {
            if (lines >= limit) // checked outside of below conditional in the case that limit is 0
                return;          // terminate because limit passed
            else if (buff[i] == '\n') // if the char is a newline
            {
                lines++;
                if (write(1, &buff[start], numBytes) != numBytes) // write from certain index until numBytes
                {
                    printf(1, "head: write error\n");
                    exit();
                }
                // printf(1, "| Line #%d |", lines);
                start = i+1; // move ahead one index to after newline
                numBytes = 0; // reset numBytes (numBytes will immediately become 1 on next iteration)
            } 
            else if (i == n-1) { // end of arr reached
                if (write(1, &buff[start], n - start) != n - start) // write from certain index until n which is the end of the array
                {
                    printf(1, "head: write error\n");
                    exit();
                }
                // printf(1, "| New Arr #%d |");
            }
        }
    }
    if (n < 0)
    {
        printf(1, "head: read error\n");
        exit();
    }
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

    if (argc <= 1) // only head
    {
        head(0, limit);
        exit();
    }
    else if (*argv[param] == '-') // if second param starts with '-'
    {
        argv[param]++; // increment to remove '-'
        // printf(1, "%s|%d|%d\n", argv[param], *argv[param], strlen(argv[param]));
        char sym;
        if ((sym = notValid(argv[param]))) {
            printf(1, "head: non-valid number in nLines flag: '%c'\n", sym);
            exit();
        }
        limit = atoi(argv[param]);
        if (argc <= 2) // no file, only head and -n
        {
            head(0, limit);
            exit();
        }
        ++param; // move onto third param (file)
    }

    for (i = param; i < argc; i++) // 1 or more file params to head
    {
        if ((fd = open(argv[i], 0)) < 0)
        {
            printf(1, "head: cannot open %s\n", argv[i]);
            exit();
        }
        head(fd, limit);
        close(fd);
    }
    exit();
}
