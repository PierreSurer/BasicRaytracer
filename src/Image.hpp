#pragma once

#include <vector>
#include <iostream>
#include <glm.hpp>

typedef glm::dvec3 Color;

class Image
{
public:
    Image(int width=0, int height=0)
        : _pixel(), _width(0), _height(0)
    {
        setSize(width, height);    //creates array
    }

    Image(const char *imageFilename): Image()
    {
        readPng(imageFilename);
    }

    // Normal accessors
    inline void putPixel(unsigned int x, unsigned int y, unsigned int channel, const unsigned char& c);
    inline unsigned char getPixel(unsigned int x, unsigned int y, unsigned int channel) const;

    // Normalized accessors, interval is (0...1, 0...1)
    //inline const Color& colorAt(float x, float y) const; //TODO : convert to unsigned char if needed

    // Normalized accessors for bumpmapping. Uses green component.
    //inline void derivativeAt(float x, float y, float *dx, float *dy) const; //TODO : convert to unsigned char if needed

    // Image parameters
    inline unsigned int width() const  { return _width; }
    inline unsigned int height() const { return _height; }
    inline unsigned int size() const   { return _width * _height; }

    // File stuff
    void writePng(const char* filename);
    void readPng(const char* filename);

    inline std::vector<unsigned char> getPixels() const { return _pixel; };

protected:

    inline unsigned int index(int x, int y) const            //integer index
    { return 4 * (y * _width + x); }

    inline unsigned int windex(int x, int y) const           //wrapped integer index
    { return index(x % _width, y % _height); }

    inline int findex(float x, float y) const       //float index
    { return index(int(x * (_width-1)), int(y * (_height-1))); }

    // Create a picture. Return false if failed.
    void setSize(int width, int height);

protected:
    std::vector<unsigned char> _pixel;
    unsigned int _width;
    unsigned int _height;
};


//Inline functions

inline void Image::putPixel(unsigned int x, unsigned int y, unsigned int channel, const unsigned char& c)
{
    _pixel.at(index(x, y) + channel) = c;
}

inline unsigned char Image::getPixel(unsigned int x, unsigned int y, unsigned int channel) const
{
    return _pixel.at(index(x, y) + channel);
}

// inline const Color& Image::colorAt(float x, float y) const
// {
//     return _pixel[findex(x, y)];
// }

// inline void Image::derivativeAt(float x, float y, float *dx, float *dy) const
// {
//     int ix = (int)(x * (_width - 1));
//     int iy = (int)(y * (_height - 1));
//     *dx = _pixel[windex(ix,iy+1)].g - _pixel[index(ix,iy)].g;
//     *dy = _pixel[windex(ix+1,iy)].g - _pixel[index(ix,iy)].g;
// }
