#include "Material.hpp"
#include "ParseObj.hpp"
#include "Scene.hpp"
#include "Raytracer.hpp"

int main(int argc, char *argv[])
{
    std::cout << "Introduction to Computer Graphics - Raytracer" << std::endl << std::endl;
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " in-file [out-file.png]" << std::endl;
        return 1;
    }

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    Raytracer raytracer(scene);

    if (!scene->readScene(argv[1]) || !raytracer.readParameters(argv[1])) {
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

    Image img(1600, 1600);
    std::cout << "Tracing..." << std::endl;
    raytracer.render(img);
    std::cout << "Writing image to " << ofname << "..." << std::endl;
    img.writePng(ofname.c_str());
    std::cout << "Done." << std::endl;

    return 0;
}
