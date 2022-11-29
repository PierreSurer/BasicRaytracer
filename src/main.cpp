#include "Material.hpp"
#include "ParseObj.hpp"
#include "Raytracer.hpp"

int main(int argc, char *argv[])
{
    std::cout << "Introduction to Computer Graphics - Raytracer" << std::endl << std::endl;
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " in-file [out-file.png]" << std::endl;
        return 1;
    }

    Raytracer raytracer;

    if (!raytracer.readScene(argv[1])) {
        std::cerr << "Error: reading scene from " << argv[1] << " failed - no output generated."<< std::endl;
        return 1;
    }

    std::string ofname;
    if (argc>=3) {
        ofname = argv[2];
    } else {
        ofname = argv[1];
        if (ofname.size()>=5 && ofname.substr(ofname.size()-5)==".yaml") {
            ofname = ofname.substr(0,ofname.size()-5);
        }
        ofname += ".png";
    }
    raytracer.renderToFile(ofname);

    return 0;
}
