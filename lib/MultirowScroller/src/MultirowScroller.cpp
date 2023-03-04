#include "MultirowScroller.hpp"

MultirowScroller::MultirowScroller(char *buf, size_t bufSz, int rows, int cols) : buf(buf), bufSz(bufSz), rows(rows), cols(cols)
{
    clear();
}
void MultirowScroller::clear()
{
    for (int i = 0; i < bufSz; i++)
    {
        buf[i] = ' ';
    }
}
void MultirowScroller::getRow(int row, char *bufDest)
{
    int pos = getOffsetPos(currPos, -((rows - 1 - row) * cols));
    // printf("curr pos: %d, pos: %d\n", currPos-1, pos);
    for (int i = 0; i < cols; i++)
    {
        pos = getOffsetPos(pos, -1);
        bufDest[cols - 1 - i] = buf[pos];
    }
}
void MultirowScroller::push(char c)
{
    buf[currPos] = c;
    currPos = getOffsetPos(currPos, 1);
}
int MultirowScroller::getOffsetPos(int pos, int offs)
{
    return (((pos + offs) % bufSz) + bufSz) % bufSz;
}