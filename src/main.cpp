#include "Material.hpp"
#include "ParseObj.hpp"
#include "Scene.hpp"
#include "Raytracer.hpp"

#include <chrono>
#include <GLFW/glfw3.h>
#include <thread>
#include <atomic>

const int IMAGE_WIDTH = 400, IMAGE_HEIGHT = 400;
std::atomic<bool> stop_signal = false;

void render_loop(Image* img) {
    GLFWwindow* window;


    /* Initialize the library */
    if (!glfwInit())
        return;

    window = glfwCreateWindow(1, 1, "Raytracer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    /* Set the framebuffer resize callback */
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        GLfloat zoom = std::min(width / (float)IMAGE_WIDTH, height / (float)IMAGE_HEIGHT);
        GLint offx = (int)((width - zoom * IMAGE_WIDTH) / 2.0f);
        GLint offy = (int)((height - zoom * IMAGE_HEIGHT) / 4.0f);

        glViewport(offx, -offy, width + offx, height - offy);
        glRasterPos2f(-1,1);
        glPixelZoom(zoom, -zoom);
    });

    glfwSetWindowSize(window, 400, (int)(400 * (float)IMAGE_HEIGHT/(float)IMAGE_WIDTH)); //calls resize callback
    while (!glfwWindowShouldClose(window) && !stop_signal) {
        glDrawPixels(IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, img->getPixels().data());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}


int main(int argc, char *argv[])
{
    std::cout << "Introduction to Computer Graphics - Raytracer" << std::endl << std::endl;
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " in-file [out-file.png]" << std::endl;
        return 1;
    }

    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double, std::milli> elapsed_time;
    Scene scene;
    TraceParameters params;
    Raytracer raytracer;

    std::cout << "Parsing... ";
    start = std::chrono::system_clock::now();
    if (!scene.readScene(argv[1])) {
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

    std::cout << "Tracing... ";
    start = std::chrono::system_clock::now();
    raytracer.render(scene, img);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
    std::cout << (elapsed_time.count() / 1000.0) << "s" << std::endl;

    std::cout << "Writing image to " << ofname << "... ";
    start = std::chrono::system_clock::now();
    img.writePng(ofname.c_str());
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
    std::cout << (elapsed_time.count() / 1000.0) << "s" << std::endl;

    std::cout << "Done." << std::endl;
    stop_signal = true;
    render_thread.join();

    return 0;
}
