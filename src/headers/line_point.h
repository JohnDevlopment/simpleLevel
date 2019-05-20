#ifndef LINE_POINT_H_INCLUDED
#define LINE_POINT_H_INCLUDED

#include <iostream>
#include <custom/pointvector>

struct Line {
	Point<int> start;  // starting point
	size_t     length; // length of line
	char       orient; // 0 = horizontal, 1 = vertical
};

inline Line CreateLine(int _x, int _y, size_t _len, int _orient) {
	Line retval;
	
	retval.start  = retval.start.create(_x, _y);
	retval.length = _len;
	retval.orient = _orient;
	
return retval;
}

#endif /* LINE_POINT_H_INCLUDED */
