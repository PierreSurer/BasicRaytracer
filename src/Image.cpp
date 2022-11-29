#include "Image.hpp"
#include "lodepng.h"
#include <fstream>

/*
* Create a picture. Answer false if failed.
*/
bool Image::setSize(int width, int height)
{
    _width = width;
    _height = height;
    if (_pixel) delete[] _pixel;
    _pixel = size() > 0 ? new Color[size()] : 0;
    return _pixel != 0;
}


void Image::writePng(const char* filename) const
{
    std::vector<unsigned char> image;
    image.resize(_width * _height * 4);
    std::vector<unsigned char>::iterator imageIterator = image.begin();
    Color *currentPixel = _pixel;
    while (imageIterator != image.end()) {
        *imageIterator = (unsigned char)(currentPixel->r * 255.0);
        imageIterator++;
        *imageIterator = (unsigned char)(currentPixel->g * 255.0);
        imageIterator++;
        *imageIterator = (unsigned char)(currentPixel->b * 255.0);
        imageIterator++;
        *imageIterator = 255;
        imageIterator++;
        currentPixel++;
    }
    LodePNG::encode(filename, image, _width, _height);
}


void Image::readPng(const char* filename)
{
    std::vector<unsigned char> buffer, image;
    //load the image file with given filename
    LodePNG::loadFile(buffer, filename);

    //decode the png
    LodePNG::Decoder decoder;
    decoder.decode(image, buffer.empty() ? 0 : &buffer[0], (unsigned)buffer.size());
    std::cout << decoder.getChannels() << std::endl;
    std::cout << decoder.getBpp() << std::endl;

    if (decoder.getChannels()<3 || decoder.getBpp()<24) {
        std::cerr << "Error: only color (RGBA), 8 bit per channel png images are supported." << std::endl;
        std::cerr << "Either convert your image or change the sourcecode." << std::endl;
        exit(1);
    }
    int w = decoder.getWidth();
    int h = decoder.getHeight();
    setSize(w, h);

    // now convert the image data
    std::vector<unsigned char>::iterator imageIterator = image.begin();
    Color *currentPixel = _pixel;
    while (imageIterator != image.end()) {
        currentPixel->r = (*imageIterator)/255.0;
        imageIterator++;
        currentPixel->g = (*imageIterator)/255.0;
        imageIterator++;
        currentPixel->b = (*imageIterator)/255.0;
        imageIterator++;
        // Let's just ignore the alpha channel
        imageIterator++; 
        currentPixel++;
    }	
}
