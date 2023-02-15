#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

#include <glm/glm.hpp>

using Color = glm::dvec3;

class Image
{
public:
    Image(unsigned int width, unsigned int height)
        : _pixel(), _width(0), _height(0)
    {
        set_size(width, height);    //creates array
    }

    Image(const std::string& imageFilename)
    {
        read_png(imageFilename);
    }

    // Normal accessors
    inline void put_pixel(unsigned int x, unsigned int y, Color c);
    inline Color get_pixel(unsigned int x, unsigned int y) const;

    // Handier accessors
    // Usage: color = img(x,y);
    //        img(x,y) = color;
    inline const Color& operator()(unsigned int x, unsigned int y) const;
    inline Color& operator()(unsigned int x, unsigned int y);

    // Normalized accessors, interval is (0...1, 0...1)
    inline Color color_at(float x, float y) const;

    // Normalized accessors for bumpmapping. Uses green component.
    inline glm::dvec2 derivative_at(float x, float y) const;

    // Image parameters
    inline unsigned int width() const  { return _width; }
    inline unsigned int height() const { return _height; }
    inline unsigned int size() const   { return _width * _height; }

    // File stuff
    void write_png(const std::string& filename) const;
    void read_png(const std::string& filename);

protected:

    inline unsigned int index(int x, int y) const            //integer index
    { return (y * _width + x); }

    inline unsigned int windex(int x, int y) const           //wrapped integer index
    { return index(x % _width, y % _height); }

    inline int findex(float x, float y) const                //float index
    { return index(int(x * (_width-1)), int(y * (_height-1))); }

    void set_size(unsigned int width, unsigned int height);

protected:
    std::vector<Color> _pixel;
    unsigned int _width;
    unsigned int _height;
};


//Inline functions

inline void Image::put_pixel(unsigned int x, unsigned int y, Color c)
{
    _pixel.at(index(x, y)) = c;
}

inline Color Image::get_pixel(unsigned int x, unsigned int y) const
{
    return _pixel.at(index(x, y));
}

inline const Color& Image::operator()(unsigned int x, unsigned int y) const
{
    return _pixel.at(index(x, y));
}

inline Color& Image::operator()(unsigned int x, unsigned int y)
{
    return _pixel.at(index(x, y));
}

inline Color Image::color_at(float x, float y) const
{
    return _pixel.at(findex(x, y));
}

inline glm::dvec2 Image::derivative_at(float x, float y) const
{
    int ix = (int)(x * (_width - 1));
    int iy = (int)(y * (_height - 1));
    return glm::dvec2(
        _pixel.at(windex(ix,iy+1)).g - _pixel.at(index(ix,iy)).g,
        _pixel.at(windex(ix+1,iy)).g - _pixel.at(index(ix,iy)).g
    );
}
