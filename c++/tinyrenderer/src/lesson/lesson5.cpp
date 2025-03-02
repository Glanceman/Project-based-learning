#include <iostream>
#include <vector>
#include "../geometry.h"
#include "../model.h"
#include "../tgaimage.h"
#include "../tools.h"
#include <expected>



const TGAColor white  = TGAColor(255, 255, 255, 255);
const TGAColor red    = TGAColor(255, 0, 0, 255);
const TGAColor green  = TGAColor(0, 255, 0, 255);
const TGAColor blue   = TGAColor(0, 0, 255, 255);
const TGAColor yellow = TGAColor(255, 255, 0, 255);

Model    *model  = NULL;
const int width  = 100;
const int height = 100;
const int depth  = 255;
Vec3f light_dir(0, 0, -1); // define light_dir
Vec3f view_dir(0, 0, -1);  // define View_dir


Mat<float,4,4> viewport(int x, int y, int w, int h) {
    Mat<float,4,4> m = Mat<float,4,4>::identity();
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
    return m;
}

/// @brief lookat matrix
/// @param eye camera postion
/// @param center target center 
/// @param up
/// @return Mat<float,4,4>
Mat<float,4,4> lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye-center).normalized();
    Vec3f x = up.cross(z).normalized();
    Vec3f y = z.cross(x).normalized();
    Mat<float,4,4> basisInverse = Mat<float,4,4>::identity();
    Mat<float,4,4> translateInverse   = Mat<float,4,4>::identity();
    for (int i=0; i<3; i++) {
        basisInverse[0][i] = x[i];
        basisInverse[1][i] = y[i];
        basisInverse[2][i] = z[i];
        translateInverse[i][3] = -eye[i];
    }
    /*
    res= x1 x2 x3 -x1c
         y1 y2 y3 -y1c
         z1 z2 z3 -z1c
         0  0  0   1
    */
    return basisInverse*translateInverse;
}


Mat<float,4,4> perspectiveProjectionMatrix(float top=1, float bot=-1, float left=-1, float right=1, float near=5, float far=100) {
    Mat<float,4,4> m = Mat<float,4,4>::zero();
    m[0][0] = 2*near/(right-left);
    m[0][2] = (right+left)/(right-left);
    
    m[1][1] = 2*near/(top-bot);
    m[1][2] = (top+bot)/(top-bot);
    
    m[2][2] = -(far+near)/(far-near);
    m[2][3] = -2*far*near/(far-near);
    
    m[3][2] = -1;
    m[3][3] = 0;
    return m;
}

Mat<float,4,4> translationMatrix(Vec3f v) {
        Mat<float,4,4> m = Mat<float,4,4>::identity();
        m[0][3] = v.x;
        m[1][3] = v.y;
        m[2][3] = v.z;
        return m;
}


Vec3f WorldToCameraSpace(Vec3f CameraPos, Vec3f WorldPos){
    return WorldPos - CameraPos;
}



int main(int argc, char **argv)
{

    model = new Model("../asset/cube_z.obj");

    int      width  = 800;
    int      height = 500;
    float aspect = width/(float)height;
    TGAImage scene(width, height, TGAImage::RGB);
    std::vector<float> zBuffer(width * height, std::numeric_limits<float>::lowest());
    Vec3f camera_pos(0, 0, 1);
    //Mat<float,4,4> VP = viewport(width/4, width/4, width/2, height/2);

    Mat<float,4,4> projectionMatrix = perspectiveProjectionMatrix(0.5, -0.5, -aspect/2, aspect/2, 0.35, 1000);
    Mat<float,4,4> modelMatrix =  lookat(camera_pos, camera_pos+Vec3f(0,0,-1), Vec3f(0,1,0)); 
    Mat<float,4,4> z = projectionMatrix*modelMatrix;
    // draw the wireframe of the model
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> vertex_indexes = model->face_vert(i); // e.g f 1193(vert)/1240(texture)/1193 1180(vert)/1227(texture)/1180 1179(vert)/1226(texture)/1179
        std::vector<int> uv_indexes     = model->face_uv(i);   // e.g f 1193(vert)/1240(texture)/1193 1180(vert)/1227(texture)/1180 1179(vert)/1226(texture)/1179

        std::array<Vec3f, 3> screen_vertices;
        std::array<Vec3f, 3> vertices;

        for (int j = 0; j < 3; j++)
        {
            Vec3f v            = model->vert(vertex_indexes[j]);
            // vec3 to vec4 
            Vec<float,4> v4 = {v.x, v.y, v.z, 1};
            // projection
            Vec4f projVec= z*v4;
            // convert to ndc 
            Vec3f projNDCVec3 = {projVec[0]/(projVec[3]+0.0001f), projVec[1]/(projVec[3]+0.0001f), projVec[2]/(projVec[3]+0.0001f)};
            //scale up to screen width and height
            screen_vertices[j] = Tool::WorldToScreen(projNDCVec3, scene);
            vertices[j]        = v;
        }
        Vec3f normal     = (vertices[2] - vertices[0]).cross(vertices[1] - vertices[0]);
        Vec3f normal_dir = normal.normalized();

        float intensity = light_dir.dot(normal_dir);
        intensity       = std::max(intensity, 0.f);

        const TGAColor color = TGAColor(255 , 255 , 255 , 255);
        
        Tool::line5(screen_vertices[0].x, screen_vertices[0].y, screen_vertices[1].x, screen_vertices[1].y, scene, TGAColor(255, 50, 50, 255));
        Tool::line5(screen_vertices[1].x, screen_vertices[1].y, screen_vertices[2].x, screen_vertices[2].y, scene, TGAColor(255, 50, 50, 255));
        Tool::line5(screen_vertices[2].x, screen_vertices[2].y, screen_vertices[0].x, screen_vertices[0].y, scene, TGAColor(255, 50, 50, 255));

    }


    scene.flip_vertically();
    scene.write_tga_file("../dist/perspective2.tga");
    std::cout
        << "Success" << std::endl;

    return 0;
}