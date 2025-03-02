#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include "geometry.h"
#include "tgaimage.h"
#include "tools.h"
#include "model.h"
#include <math.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);

Vec3f light_dir(0, 0, -1); // define light_dir
Vec3f view_dir(0, 0, -1);  // define View_dir

Vec3f camera_pos(0, 0, 0);



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
    int                width  = 800;
    int                height = 800;
    float aspect = width/(float)height;
    TGAImage           image1(width, height, TGAImage::RGB);
    std::vector<float> zBuffer(width * height, std::numeric_limits<float>::lowest());
    Model              model("../asset/african_head.obj");
    // Model    model("../asset/plane.obj");
    TGAImage diffuse_map;
    diffuse_map.read_tga_file("../asset/african_head_diffuse.tga");

    Mat<float,4,4> projectionMatrix = Tool::perspectiveProjectionMatrix(0.5, -0.5, -aspect/2, aspect/2, 1, 1000);
    Mat<float,4,4> modelMatrix =  Tool::lookat(camera_pos, camera_pos+view_dir, Vec3f(0,1,0)); 
    Mat<float,4,4> translationMatrix = Tool::translationMatrix(Vec3f(0,0,-6));
    Mat<float,4,4> z = projectionMatrix*translationMatrix;
    // draw the wireframe of the model
    for (int i = 0; i < model.nfaces(); i++)
    {
        std::vector<int> vertex_indexes = model.face_vert(i); // e.g f 1193(vert)/1240(texture)/1193 1180(vert)/1227(texture)/1180 1179(vert)/1226(texture)/1179
        std::vector<int> uv_indexes     = model.face_uv(i);   // e.g f 1193(vert)/1240(texture)/1193 1180(vert)/1227(texture)/1180 1179(vert)/1226(texture)/1179

        std::array<Vec3f, 3> screen_vertices;
        std::array<Vec2f, 3> uvs;
        std::array<Vec3f, 3> vertices;

        for (int j = 0; j < 3; j++)
        {
            Vec3f v            = model.vert(vertex_indexes[j]);
            // vec3 to vec4
            Vec<float,4> v4 = {v.x, v.y, v.z, 1};
            Vec4f projVec= z*v4;
            Vec3f projNDCVec3 = {projVec[0]/(projVec[3]+0.0001f), projVec[1]/(projVec[3]+0.0001f), -1*projVec[2]/(projVec[3]+0.0001f)};
            auto res = std::signbit(projVec[2])==std::signbit(projNDCVec3[2])?"true":"false";
            std::cout<<i <<";"<<v4[2] <<";"<<projVec[2]<<";" << projNDCVec3[2]<<";" << res << std::endl;
            screen_vertices[j] = Tool::WorldToScreen(projNDCVec3, image1);
            vertices[j]        = {projNDCVec3.x, projNDCVec3.y, projNDCVec3.z};

            uvs[j] = model.uv(uv_indexes[j]);
        }
        
        Vec3f normal     = (vertices[2] - vertices[0]).cross(vertices[1] - vertices[0]);
        Vec3f normal_dir = normal.normalized();

        float intensity = std::max(0.f,light_dir.dot(normal_dir));

        // back face culling
        float back_face_indicator = view_dir.dot(normal_dir);
        if (back_face_indicator>=0)
        {
            const TGAColor color = TGAColor(255 * intensity, 255 * intensity, 255 * intensity, 255);
            Tool::triangle_v4(screen_vertices, uvs, image1, diffuse_map, zBuffer.data(), color, intensity, false);
        }
    }
    // save the image
    image1.flip_vertically();
    image1.write_tga_file("../dist/model_perspective.tga");
    std::cout
        << "Success (Main)" << std::endl;
    return 0;
}
