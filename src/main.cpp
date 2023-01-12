#include "Material.hpp"
#include "ParseObj.hpp"
#include "Scene.hpp"
#include "Raytracer.hpp"

#include <chrono>
#include <GLFW/glfw3.h>
#include <thread>

const int IMAGE_WIDTH = 1600, IMAGE_HEIGHT = 1600;
const float WINDOW_ZOOM = 0.5f;

void render_loop(Image* img) {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return;

    window = glfwCreateWindow((int)(IMAGE_WIDTH * WINDOW_ZOOM), (int)(IMAGE_HEIGHT * WINDOW_ZOOM), "Raytracer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    glRasterPos2f(-1,1);
    glPixelZoom(WINDOW_ZOOM, -WINDOW_ZOOM );
    while (!glfwWindowShouldClose(window))
    {
        glDrawPixels(IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, img->getPixels().data());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}


int main(int argc, char *argv[])
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double, std::milli> elapsed_time;
    std::cout << "Introduction to Computer Graphics - Raytracer" << std::endl << std::endl;
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " in-file [out-file.png]" << std::endl;
        return 1;
    }

    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    Raytracer raytracer(scene);

    start = std::chrono::system_clock::now();
    std::cout << "Parsing... ";
    if (!scene->readScene(argv[1]) || !raytracer.readParameters(argv[1])) {
        std::cerr << "Error: reading scene from " << argv[1] << " failed - no output generated."<< std::endl;
        return 1;
    }
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
    std::cout << (elapsed_time.count() / 1000.0) << "s" << std::endl;

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

    Image img(IMAGE_WIDTH, IMAGE_HEIGHT);

    std::thread render_thread(render_loop, &img);
    render_thread.detach();

    start = std::chrono::system_clock::now();
    std::cout << "Tracing... ";
    raytracer.render(img);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
    std::cout << (elapsed_time.count() / 1000.0) << "s" << std::endl;

    start = std::chrono::system_clock::now();
    std::cout << "Writing image to " << ofname << "... ";
    img.writePng(ofname.c_str());
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
    std::cout << (elapsed_time.count() / 1000.0) << "s" << std::endl;

    std::cout << "Done." << std::endl;

    return 0;
}
