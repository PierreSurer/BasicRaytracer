#pragma once

#include <iostream>
#include <glm/glm.hpp>

typedef glm::dvec3 Color;

class Image
{
public:
    Image(int width=0, int height=0)
        : _pixel(nullptr), _width(0), _height(0)
    {
        setSize(width, height);    //creates array
    }

    Image(const char *imageFilename): Image()
    {
        readPng(imageFilename);
    }

    ~Image()
    {
        delete[] _pixel;
    }

    // Normal accessors
    inline void putPixel(int x, int y, Color c);
    inline Color getPixel(int x, int y) const;

    // Handier accessors
    // Usage: color = img(x,y);
    //        img(x,y) = color;
    inline const Color& operator()(int x, int y) const;
    inline Color& operator()(int x, int y);

    // Normalized accessors, interval is (0...1, 0...1)
    inline const Color& colorAt(float x, float y) const;

    // Normalized accessors for bumpmapping. Uses green component.
    inline void derivativeAt(float x, float y, float *dx, float *dy) const;

    // Image parameters
    inline int width() const  { return _width; }
    inline int height() const { return _height; }
    inline int size() const   { return _width * _height; }

    // File stuff
    void writePng(const char* filename) const;
    void readPng(const char* filename);

protected:

    inline int index(int x, int y) const            //integer index
    { return y * _width + x; }

    inline int windex(int x, int y) const           //wrapped integer index
    { return index(x % _width, y % _height); }

    inline int findex(float x, float y) const       //float index
    { return index(int(x * (_width-1)), int(y * (_height-1))); }

    // Create a picture. Return false if failed.
    bool setSize(int width, int height);

protected:
    Color* _pixel;
    int _width;
    int _height;
};


//Inline functions

inline void Image::putPixel(int x, int y, Color c)
{
    (*this)(x, y) = c;
}

inline Color Image::getPixel(int x, int y) const
{
    return (*this)(x, y);
}

inline const Color& Image::operator()(int x, int y) const
{
    return _pixel[index(x, y)];
}

inline Color& Image::operator()(int x, int y)
{
    return _pixel[index(x, y)];
}

inline const Color& Image::colorAt(float x, float y) const
{
    return _pixel[findex(x, y)];
}

inline void Image::derivativeAt(float x, float y, float *dx, float *dy) const
{
    int ix = (int)(x * (_width - 1));
    int iy = (int)(y * (_height - 1));
    *dx = _pixel[windex(ix,iy+1)].g - _pixel[index(ix,iy)].g;
    *dy = _pixel[windex(ix+1,iy)].g - _pixel[index(ix,iy)].g;
}
