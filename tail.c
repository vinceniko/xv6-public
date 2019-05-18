#include "types.h"
#include "stat.h"
#include "user.h"

#define NULL 0

char buff[512];

// circularArr operates like a circular buffer
// elements are added using the current index position and it wraps around once the size has been reached
// functions that act upon CircularArr take care of wrapping around the indices
struct CircularArr
{
    void **arr; // dynamic array to hold n strings as determined by the limit arg passed in the cli
    int size; //  equivalent to limit here 
    int index; // refers to next free spot
};

// returns element to the right of index in the circular array
int right(int index, int size)
{
    return (index + 1) % size;
}

// returns element to the right of index in the circular array
int left(int index, int size)
{
    return (index - 1 + size) % size;
}

// retuns the index of the last inserted elem
int getLastIndex(struct CircularArr const *circArr)
{
    return left(circArr->index, circArr->size);
}

// returns the oldest element which is referred to by the current index
void *getOldest(struct CircularArr const *circArr)
{
    return circArr->arr[circArr->index];
}

// adds an element, replacing an element after the first loop around
void addElem(struct CircularArr *circArr, void *const line)
{
    if (circArr->arr[circArr->index] != NULL) // init values in first loop over
        free(circArr->arr[circArr->index]); // free the dynamic strings allocated that will be replaced
    circArr->arr[circArr->index] = line; // replace
    circArr->index = right(circArr->index, circArr->size);
}

// appends src to dest by allocating a new array and returning it
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
    int lastI = getLastIndex(lines);
    char *lastLine = lines->arr[lastI];
    char *newLine = malloc(strlen(lastLine) + strlen(line) + 1); // + 1 for null
    newLine[0] = '\0'; // initial null terminator for allocated but empty string, used as a reference point for first strcat call
    strcat(newLine, lastLine);
    strcat(newLine, line);

    free(line);
    free(lastLine);

    lines->arr[lastI] = newLine;
}

// prints the elements in the correct order from first inserted to last inserted
void printInOrder(struct CircularArr *const circArr)
{
    int i, start;
    i = start = circArr->index; // oldest element

    do {
        if (circArr->arr[i] != NULL) // occurs when the file has less lines than limit
            printf(1, "%s", circArr->arr[i]);
        else
            i = circArr->size-1; // set to end so next statement sets i to 0, will iterate from 0 till index-1 and terminate
        i = right(i, circArr->size); // loops around
    } while (i != start); // iterates from index (which is oldest element unless null) to element before index (which is newest elem)
}

// sticks in Null values into every space in the circular arr. used to later free memory correctly in addElem
void init(struct CircularArr *circArr)
{
    int i;
    for (i = 0; i < circArr->size; i++)
        circArr->arr[i] = NULL;
}

// creates a line from n chars in buff
char *createLine(char *buff, int n)
{
    char *line = malloc(sizeof(char *) * n + 1);  // n + 1 for null terminator
    memmove(line, buff, n);
    line[n] = '\0'; // add the null terminator at the end of the line

    return line;
}

// tail prints n (limit) lines from the file specified by fd
void tail(int fd, int limit)
{
    int n, i, start, numBytes;
    struct CircularArr lines = {
        malloc(sizeof(char **) * limit),
        limit,
        0,
    }; // allocate an arr of size limit which stores lines
    init(&lines); // default values in arr to free memory later

    while ((n = read(fd, buff, sizeof(buff))) > 0)
    {
        for (i = 0, start = 0, numBytes = 1; i < n; i++, numBytes++) // beginning of arr, numBytes gets initialized to 1 since the first element is 1 byte
        {
            if (buff[i] == '\n') // if the char is a newline
            {
                char *line = createLine(&buff[start], numBytes);

                addElem(&lines, line);
            }
            else if (i == n - 1) // end of buffer will be reached before reading in another new line
            {
                int len = n - start + 1;
                char *line = createLine(&buff[start], len);

                appendLast(&lines, line); // have to append to the last inserted entry since no new line was found
            }

            if (buff[i] == '\n' || i == n - 1) // either of the two conditions above
            {
                start = i + 1; // move ahead one index to after newline
                numBytes = 0;  // reset numBytes (numBytes will immediately become 1 on next iteration)
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
