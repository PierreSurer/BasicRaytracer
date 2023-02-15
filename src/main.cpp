#include "Material.hpp"
#include "Scene.hpp"
#include "Raytracer.hpp"

int main(int argc, char *argv[])
{
    std::cout << "Introduction to Computer Graphics - Raytracer" << std::endl << std::endl;
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " in-file [out-file.png]" << std::endl;
        return 1;
    }

    Scene scene;
    Raytracer raytracer;
    Image img(400, 400);

    if (!scene.readScene(argv[1])) {
        std::cerr << "Error: reading scene from " << argv[1] << " failed - no output generated."<< std::endl;
        return 1;
    }
    std::string ofname;
    if (argc >= 3) {
        ofname = argv[2];
    } else {
        ofname = argv[1];
        if (ofname.size()>=5 && ofname.substr(ofname.size()-5)==".yaml") {
            ofname = ofname.substr(0,ofname.size()-5);
        }
        ofname += ".png";
    }

    raytracer.render(scene, img);
    img.write_png(ofname);

    return 0;
}
