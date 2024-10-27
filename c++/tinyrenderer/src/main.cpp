#include <algorithm>
#include <array>
#include <iostream>
#include "geometry.h"
#include "tgaimage.h"
#include "tools.h"
#include "model.h"
#include <math.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);

Vec3f light_dir(0, 0, -1); // define light_dir
Vec3f View_dir(0, 0, -1);  // define View_dir

int main(int argc, char **argv)
{
    TGAImage image(100, 100, TGAImage::RGB);
    // image.set(52, 41, white);
    Tool::line(10, 10, 80, 80, image, white);
    Tool::line2(10, 20, 80, 90, image, red);
    Tool::line3(10, 30, 80, 100, image, red);
    Tool::line4(10, 40, 80, 110, image, white);
    Tool::line5(10, 50, 80, 120, image, white);
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("../dist/output.tga");

    // import the model
    int      width  = 800;
    int      height = 800;
    TGAImage image1(width, height, TGAImage::RGB);
    Model    model("../asset/african_head.obj");

    // draw the wireframe of the model
    for (int i = 0; i < model.nfaces(); i++)
    {
        std::vector<int>     faces = model.face(i); // e.g 1193/1240/1193
        std::array<Vec2i, 3> screen_vertices;
        std::array<Vec3f, 3> vertices;
        for (int j = 0; j < 3; j++)
        {
            Vec3f v            = model.vert(faces[j]);
            screen_vertices[j] = {static_cast<int>((v.x + 1.) * width / 2.), static_cast<int>((v.y + 1.) * height / 2)};
            vertices[j]        = v;
        }
        Vec3f normal     = (vertices[2] - vertices[0]).cross(vertices[1] - vertices[0]);
        Vec3f normal_dir = normal.normalize();

        float intensity = light_dir.dot(normal_dir);
        intensity       = std::max(intensity, 0.f);

        // back face culling
        float back_face_indicator = View_dir.dot(normal_dir);
        if (back_face_indicator >= 0)
        {
            const TGAColor color = TGAColor(255 * intensity, 255 * intensity, 255 * intensity, 255);
            Tool::triangle_v3(screen_vertices[0], screen_vertices[1], screen_vertices[2], image1, color, false);
        }
    }
    // save the image
    image1.flip_vertically();
    image1.write_tga_file("../dist/model.tga");
    std::cout
        << "Success" << std::endl;
    return 0;
}