#pragma once

#ifndef VITA_WIDTH
#define VITA_WIDTH 960
#endif

#ifndef VITA_HEIGHT
#define VITA_HEIGHT 544
#endif

#define VITA_WIDTH_HALF (VITA_WIDTH / 2)
#define VITA_HEIGHT_HALF (VITA_HEIGHT / 2)

template <typename T>
struct Rect
{
    T displacement_x; // screen center is (0, 0), right is positive
    T displacement_y; // up is positive
    T width;
    T height;
    T top;
    T left;
    T bottom;
    T right;

    void InitWithDisplacementAndSize(T x, T y, T w, T h)
    {
        displacement_x = x;
        displacement_y = y;
        width = w;
        height = h;

        top = VITA_HEIGHT_HALF - h / 2 - y;
        bottom = top + h;
        left = VITA_WIDTH_HALF - w / 2 - x;
        right = left + w;
    }

    void InitWithPosition(T t, T l, T b, T r)
    {
        top = t;
        left = l;
        bottom = b;
        right = r;

        width = right - left;
        height = bottom - top;
        displacement_x = VITA_WIDTH_HALF - width / 2 - left;
        displacement_y = VITA_HEIGHT_HALF - height / 2 - top;
    }

    bool Contains(T x, T y)
    {
        return x >= left && x <= right && y >= top && y <= bottom;
    }
};
