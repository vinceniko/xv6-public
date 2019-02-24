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
            if (buff[i] == '\n') // if the char is a newline
            {
                lines++;
                if (write(1, &buff[start], numBytes) != numBytes) // write from certain index until numBytes
                {
                    printf(1, "head: write error\n");
                    exit();
                }
                // printf(1, "| Line #%d |", lines);
                if (lines >= limit)
                    return;
                start = i+1; // move ahead one index to after newline
                numBytes = 0; // reset numBytes (numBytes will immediately become 1 on next iteration)
            } 
            else if (i == n-1) { // end of arr reached
                if (write(1, &buff[start], n - start) != n - start) // write from certain index until n - 1 which is the end of the array
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

int main(int argc, char *argv[])
{
    int fd, i;

    if (argc <= 1)
    {
        head(0, 10);
        exit();
    }

    for (i = 1; i < argc; i++)
    {
        if ((fd = open(argv[i], 0)) < 0)
        {
            printf(1, "head: cannot open %s\n", argv[i]);
            exit();
        }
        head(fd, 10);
        close(fd);
    }
    exit();
}
