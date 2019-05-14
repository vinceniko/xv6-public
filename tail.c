#include "types.h"
#include "stat.h"
#include "user.h"

#define NULL 0

char buff[512];

struct CircularArr
{
    void **arr;
    uint size;
    uint index; // refers to next free spot
};

// adds an element, sets index to the beginning if size is passed
void addElem(struct CircularArr *lines, void *const line)
{
    // free(lines->arr[lines->index]);
    // lines->arr[lines->index] = NULL;
    lines->arr[lines->index] = line;
    lines->index = (lines->index + 1) % lines->size; // loops around
}

// retuns the index of the last inserted elem
int getLast(struct CircularArr const *lines)
{
    if (lines->index == 0)
        return lines->size - 1;
    return lines->index;
}

char* strcat(char* dest, char* src) 
{
    // make ptr point to the end of destination string
    char *ptr = dest + strlen(dest);

    // Appends characters of src to the dest string
    while (*src != '\0')
        *ptr++ = *src++;

    // null terminate dest string
    *ptr = '\0';

    // dest is returned by standard strcat()
    return dest;
}

// appends a string to the last inserted string
void appendLast(struct CircularArr *lines, char *const line)
{
    int lastI = getLast(lines);
    char *lastLine = lines->arr[lastI];
    char *newLine = malloc(strlen(lastLine) + strlen(line) + 1); // + 1 for null
    newLine[0] = '\0';
    strcat(newLine, lastLine);
    strcat(newLine, line);

    free(line);
    free(lastLine);

    lines->arr[lastI] = newLine;
}

// prints the elements in the correct order from first inserted to last inserted
void printInOrder(struct CircularArr *const lines)
{
    int i, start;
    i = start = lines->index;

    int first = 1;
    while (i != start || first) // first is there so it executes in the beginning, then it iterates until the end and loops to the beginning
    {
        printf(1, "%s", lines->arr[i]);
        i = (i + 1) % lines->size; // loops around
        first = 0;
    }
}

// void init(struct CircularArr *lines)
// {
//     int i;
//     for (i = 0; i < lines->size; i++)
//     {
//         lines->arr[i] = malloc(NULL);
//     }
// }

void tail(int fd, uint limit)
{
    int n, i, start, numBytes;
    struct CircularArr lines = {
        malloc(sizeof(char **) * limit),
        limit,
        0,
    }; // allocate an arr of size limit which stores lines
    // init(&lines);

    while ((n = read(fd, buff, sizeof(buff))) > 0)
    {
        for (i = 0, start = 0, numBytes = 1; i < n; i++, numBytes++) // beginning of arr, numBytes gets initialized to 1 since the first element is 1 byte
        {
            if (buff[i] == '\n') // if the char is a newline
            {
                char *line = malloc(sizeof(char *) * numBytes);
                memmove(line, &buff[start], numBytes);
                start = i + 1; // move ahead one index to after newline
                numBytes = 0;  // reset numBytes (numBytes will immediately become 1 on next iteration)

                addElem(&lines, line);
            }
            else if (i == n - 1) // end of buffer will be reached before reading in another new line
            { // end of arr reached
                char *line = malloc(sizeof(char *) * numBytes);
                memmove(line, &buff[start], n - start);
                start = i + 1; // move ahead one index to after newline
                numBytes = 0;  // reset numBytes (numBytes will immediately become 1 on next iteration)

                appendLast(&lines, line); // have to append to the last inserted entry since no new line was found
            }
        }
    }
    if (n < 0)
    {
        printf(1, "head: read error\n");
        exit();
    }
    printInOrder(&lines);
}

ushort
notValid(char *arr) // checks if string contains a non number
{
    int i;
    for (i = 0; i < strlen(arr); i++)
    {
        if (arr[i] < '0' || arr[i] > '9') // non number
            return arr[i];                // not Valid number
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
        if ((sym = notValid(argv[param])))
        {
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
