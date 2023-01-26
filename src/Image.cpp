#include "Image.hpp"
#include "lodepng.h"
#include <algorithm>
#include <fstream>

/*
* Create a picture. Answer false if failed.
*/
void Image::setSize(int width, int height)
{
    _width = width;
    _height = height;
    _pixel.resize(size() * 4);
    std::fill(_pixel.begin(), _pixel.end(), 0);
}


void Image::writePng(const char* filename)
{
    LodePNG::encode(filename, _pixel, _width, _height);
}


void Image::readPng(const char* filename)
{
    std::vector<unsigned char> buffer, image;
    //load the image file with given filename
    LodePNG::loadFile(buffer, filename);

    //decode the png
    LodePNG::Decoder decoder;
    decoder.decode(image, buffer.empty() ? 0 : &buffer[0], (unsigned)buffer.size());
    // std::cout << decoder.getChannels() << std::endl;
    // std::cout << decoder.getBpp() << std::endl;

    if (decoder.getChannels()<3 || decoder.getBpp()<24) {
        std::cerr << "Error: only color (RGBA), 8 bit per channel png images are supported." << std::endl;
        std::cerr << "Either convert your image or change the sourcecode." << std::endl;
        exit(1);
    }
    int w = decoder.getWidth();
    int h = decoder.getHeight();
    setSize(w, h);

    std::copy(image.begin(), image.end(), _pixel.begin());
}

Color Image::sample(glm::dvec2 uv) const {
    unsigned int x = std::clamp<double>(uv.x * _width, 0.0, _width - 1);
    unsigned int y = std::clamp<double>(uv.y * _height, 0.0, _height - 1);
    y = _height - y - 1;

    return glm::dvec4(
        getPixel(x, y, 0) / 255.0,
        getPixel(x, y, 1) / 255.0,
        getPixel(x, y, 2) / 255.0,
        getPixel(x, y, 3) / 255.0
    );
}
