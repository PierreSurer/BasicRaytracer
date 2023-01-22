#include "Material.hpp"
#include "ParseObj.hpp"
#include "Scene.hpp"
#include "Raytracer.hpp"

#include <chrono>
#include <GLFW/glfw3.h>
#include <thread>
#include <atomic>

std::atomic<bool> stop_signal = false;

void render_loop(Image* img) {
    GLFWwindow* window;


    /* Initialize the library */
    if (!glfwInit() || stop_signal)
        return;

    window = glfwCreateWindow(1, 1, "Raytracer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);


    std::pair<int, int> dimensions = {img->width(), img->height()};
    /* Set the framebuffer resize callback */
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        auto dims = *(std::pair<int, int>*)glfwGetWindowUserPointer(window);

        GLfloat zoom = std::min(width / (float)dims.first, height / (float)dims.second);
        GLint offx = (int)((width - zoom * dims.first) / 2.0f);
        GLint offy = (int)((height - zoom * dims.second) / 4.0f);

        glViewport(offx, -offy, width + offx, height - offy);
        glRasterPos2f(-1,1);
        glPixelZoom(zoom, -zoom);
    });

    glfwSetWindowUserPointer(window, &dimensions);

    glfwSetWindowSize(window, 400, (int)(400 * (float)img->height()/(float)img->width())); //calls resize callback
    while (!glfwWindowShouldClose(window) && !stop_signal) {
        glDrawPixels(img->width(), img->height(), GL_RGBA, GL_UNSIGNED_BYTE, img->getPixels().data());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}


int main(int argc, char *argv[])
{
    std::cout << "Introduction to Computer Graphics - Raytracer" << std::endl << std::endl;
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " in-file" << std::endl;
        std::cout << "    -o [out-file.png]" << std::endl;
        std::cout << "    -d [width] [height]" << std::endl;
        std::cout << "    -w [display window (y/n)]" << std::endl;
        return 1;
    }

    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double, std::milli> elapsed_time;
    Scene scene;
    Raytracer raytracer;

    std::cout << "Parsing... ";
    start = std::chrono::system_clock::now();

    int word = 2;
    int width = 400;
    int height = 400;
    std::string ofname;

    try { //parsing parameters
        while(word < argc) {
            if (strcmp(argv[word], "-o") == 0) {
                if(word + 1 < argc) {
                    ofname = argv[word + 1];
                    word += 2;
                } else {
                    throw std::runtime_error("Missing arguments for " + std::string(argv[word]));
                }
            } 
            else if (strcmp(argv[word], "-d") == 0) {
                if(word + 2 < argc) {
                    width = std::stoi(argv[word + 1]);
                    height = std::stoi(argv[word + 2]);
                    word += 3;
                } else {
                    throw std::runtime_error("Missing arguments for " + std::string(argv[word]));
                }
            }
            else if (strcmp(argv[word], "-w") == 0) {
                if(word + 1 < argc) {
                    stop_signal = strcmp(argv[word + 1], "n") == 0;
                    word += 2;
                } else {
                    throw std::runtime_error("Missing arguments for " + std::string(argv[word]));
                }
            } 
            else {
                throw std::runtime_error("Unknown argument " + std::string(argv[word]));
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout << std::endl<< e.what() << std::endl;
        std::cerr << "Usage: " << argv[0] << " in-file" << std::endl;
        std::cout << "    -o [out-file.png]" << std::endl;
        std::cout << "    -d [width] [height]" << std::endl;
        std::cout << "    -w [display window (y/n)]" << std::endl;
        return 1;
            
    }

    if(ofname.length() == 0) {
        ofname = argv[1];
        if (ofname.size()>=5 && ofname.substr(ofname.size()-5)==".yaml") {
            ofname = ofname.substr(0,ofname.size()-5);
        }
        ofname += ".png";
    }

    if (!scene.readScene(argv[1])) {
        std::cerr << "Error: reading scene from " << argv[1] << " failed - no output generated."<< std::endl;
        return 1;
    }

    Image img(width, height);

    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
    std::cout << (elapsed_time.count() / 1000.0) << "s" << std::endl;

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
