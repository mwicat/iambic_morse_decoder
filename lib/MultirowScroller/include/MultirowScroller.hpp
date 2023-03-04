#ifndef MULTIROW_SCROLLER_HPP
#define MULTIROW_SCROLLER_HPP

#include <stdlib.h>

class MultirowScroller
{
public:
    MultirowScroller(char *buf, size_t bufSz, int rows, int cols);
    void clear();
    void getRow(int row, char *bufDest);
    void push(char c);
    int getOffsetPos(int pos, int offs);

private:
    int rows;
    int cols;
    char *buf;
    int bufSz;
    int currPos = 0;
};

#endif