#include "Image.hpp"

#include <lodepng/lodepng.h>
#include <glm/glm.hpp>

#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <stdexcept>

void Image::set_size(unsigned int width, unsigned int height)
{
    if(width == 0 || height == 0)
        throw std::runtime_error("Invalid image size");
    _width = width;
    _height = height;
    _pixel.resize(size());
    std::fill(_pixel.begin(), _pixel.end(), Color(0.0));
}


void Image::write_png(const std::string& filename) const
{
    std::vector<unsigned char> image;
    image.resize(size() * 4);
    auto imageIterator = image.begin();
    for (auto it = _pixel.begin(); it  != _pixel.end(); ++it) {
        const Color& pixel = *it;
        *imageIterator = (unsigned char)(pixel.r * 255.0);
        imageIterator++;
        *imageIterator = (unsigned char)(pixel.g * 255.0);
        imageIterator++;
        *imageIterator = (unsigned char)(pixel.b * 255.0);
        imageIterator++;
        *imageIterator = 255;
        imageIterator++;
    }
    LodePNG::encode(filename, image, _width, _height);
}


void Image::read_png(const std::string& filename)
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
    set_size(w, h);

    // now convert the image data
    auto imageIterator = image.begin();
    for (auto it = _pixel.begin(); it  != _pixel.end(); ++it) {
        Color& pixel = *it;
        pixel.r = (*imageIterator) / 255.0;
        imageIterator++;
        pixel.g = (*imageIterator) / 255.0;
        imageIterator++;
        pixel.b = (*imageIterator) / 255.0;
        imageIterator++;
        // Let's just ignore the alpha channel
        imageIterator++; 
    }
}
