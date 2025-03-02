#include "geometry.h"
#include "tgaimage.h"
#include <array>
#include <iostream>
#include <utility>
#include <algorithm>
namespace Tool
{

    inline Vec3f WorldToScreen(Vec3f v, TGAImage &screen)
    {
        int width  = screen.get_width();
        int height = screen.get_height();
        return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
    }

    inline Vec2f lerp(float r, Vec2f p0, Vec2f p1)
    {
        return p0 + (p1 - p0).dot(r);
    }

    inline Vec2i lerp(float r, Vec2i p0, Vec2i p1)
    {
        return p0.dot(1 - r) + (p1).dot(r);
    }

    /// @brief version 1 bad if we choose large stepping, small will be example
    /// @param x0
    /// @param y0
    /// @param x1
    /// @param y1
    /// @param image
    /// @param color
    inline void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
    {
        for (float t = 0.; t < 1.; t += .01)
        {
            int x = x0 + (x1 - x0) * t;
            int y = y0 + (y1 - y0) * t;
            image.set(x, y, color);
        }
    }

    /// @brief verison 2 auto stepping but only consider 1 direction
    /// @param x0
    /// @param y0
    /// @param x1
    /// @param y1
    /// @param image
    /// @param color
    inline void line2(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
    {
        for (int x = x0; x <= x1; x++)
        {
            float t = (x - x0) / (float)(x1 - x0);
            int   y = y0 * (1. - t) + y1 * t;
            image.set(x, y, color);
        }
    }

    /// @brief line3 prevent divide 0 by swap x and y
    /// @param x0
    /// @param y0
    /// @param x1
    /// @param y1
    /// @param image
    /// @param color
    inline void line3(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
    {
        bool steep = false;
        if (std::abs(x0 - x1) < std::abs(y0 - y1))
        { // if the line is steep >45, we transpose the image
            std::swap(x0, y0);
            std::swap(x1, y1);
            steep = true;
        }
        if (x0 > x1)
        { // make it left−to−right
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        for (int x = x0; x <= x1; x++)
        {
            float t = (x - x0) / (float)(x1 - x0);
            int   y = y0 * (1. - t) + y1 * t;
            if (steep)
            {
                image.set(y, x, color); // if transposed, de−transpose
            }
            else
            {
                image.set(x, y, color);
            }
        }
    }

    /// @brief fix version 3 error
    /// @param x0
    /// @param y0
    /// @param x1
    /// @param y1
    /// @param image
    /// @param color
    inline void line4(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
    {
        bool steep = false;
        if (std::abs(x0 - x1) < std::abs(y0 - y1))
        {
            std::swap(x0, y0);
            std::swap(x1, y1);
            steep = true;
        }
        if (x0 > x1)
        {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        int   dx       = x1 - x0;
        int   dy       = y1 - y0;
        float slopeVal = std::abs(dy / float(dx)); //(0 - 1)
        float error    = 0;
        int   y        = y0;

        for (int x = x0; x <= x1; x++)
        {
            if (steep)
            {
                image.set(y, x, color);
            }
            else
            {
                image.set(x, y, color);
            }
            error += slopeVal;
            if (error > .5)
            {
                y += (y1 > y0 ? 1 : -1);
                error -= 1.;
            }
        }
    }

    /// @brief remove division and float
    /// @param x0
    /// @param y0
    /// @param x1
    /// @param y1
    /// @param image
    /// @param color
    inline void line5(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
    {
        bool steep = false;
        if (std::abs(x0 - x1) < std::abs(y0 - y1))
        {
            std::swap(x0, y0);
            std::swap(x1, y1);
            steep = true;
        }
        if (x0 > x1)
        {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        int dx       = x1 - x0;
        int dy       = y1 - y0;
        int slopeVal = std::abs(dy) * 2; //!!!!!
        int error    = 0;
        int y        = y0;

        for (int x = x0; x <= x1; x++)
        {
            if (steep)
            {
                image.set(y, x, color);
            }
            else
            {
                image.set(x, y, color);
            }
            error += slopeVal;
            if (error > dx)
            {
                y += (y1 > y0 ? 1 : -1);
                error -= dx * 2;
            }
        }
    }

    inline void line5(Vec2i p, Vec2i p1, TGAImage &image, TGAColor color)
    {
        line5(p.x, p.y, p1.x, p1.y, image, color);
    }

    inline void triangle_v1(const Vec2i &v0, const Vec2i &v1, const Vec2i &v2, TGAImage &image, const TGAColor &color)
    {
        line5(v0.x, v0.y, v1.x, v1.y, image, color);
        line5(v1.x, v1.y, v2.x, v2.y, image, color);
        line5(v2.x, v2.y, v0.x, v0.y, image, color);
    }

    // scanline
    inline void triangle_v2(Vec2i v0, Vec2i v1, Vec2i v2, TGAImage &image, const TGAColor &color, bool outline = true)
    {
        // sort by y v0<v1<v2
        if (v0.y > v1.y) std::swap(v0, v1);
        if (v1.y > v2.y) std::swap(v1, v2);
        if (v0.y > v2.y) std::swap(v0, v2);

        // notice not y/x -> x/y
        float slopeA = static_cast<float>(v2.x - v0.x) / static_cast<float>(v2.y - v0.y);
        float slopeB = static_cast<float>(v1.x - v0.x) / static_cast<float>(v1.y - v0.y);
        float slopeC = static_cast<float>(v2.x - v1.x) / static_cast<float>(v2.y - v1.y);

        float diff  = slopeB;
        float curX1 = v0.x;
        float curX2 = v0.x;

        for (int i = v0.y; i <= v2.y; i++)
        {
            if (i >= v1.y)
            {
                // recalculate slope value
                diff = slopeC;
            }
            line5(curX1, i, curX2, i, image, color);
            curX1 = curX1 + slopeA;
            curX2 += diff;
        }

        if (outline)
        {
            line5(v0.x, v0.y, v1.x, v1.y, image, TGAColor(255, 255, 255, 255));
            line5(v1.x, v1.y, v2.x, v2.y, image, TGAColor(255, 255, 255, 255));
            line5(v2.x, v2.y, v0.x, v0.y, image, TGAColor(255, 255, 255, 255));
        }
    }

    // convert Cartesian coordinates into barycentric
    inline Vec3f barycentric(const std::array<Vec2i, 3> &points, const Vec2i &p)
    {
        float area1 = 0.5f * (points[1] - points[0]).cross((p - points[0])); // tri area in clockwise with cross product
        float area2 = 0.5f * (points[2] - points[1]).cross((p - points[1]));
        float area3 = 0.5f * (p - points[0]).cross((points[2] - points[0]));

        float triArea = 0.5f * (points[1] - points[0]).cross((points[2] - points[0]));

        return Vec3f(area2 / triArea, area3 / triArea, area1 / triArea);
    }

    inline Vec3f barycentric(const std::array<Vec2f, 3> &points, const Vec2f &p)
    {
        float area1 = 0.5 * (points[1] - points[0]).cross((p - points[0])); // tri area in clockwise with cross product
        float area2 = 0.5 * (points[2] - points[1]).cross((p - points[1]));
        float area3 = 0.5 * (p - points[0]).cross((points[2] - points[0]));

        float triArea = 0.5 * (points[1] - points[0]).cross((points[2] - points[0]));

        return Vec3f(area2 / triArea, area3 / triArea, area1 / triArea);
    }

    // barycentric coordinates https://www.youtube.com/watch?v=HYAgJN3x4GA&t=29s
    inline void triangle_v4(std::array<Vec3f, 3> &screen_coords, std::array<Vec2f, 3> &uvs, TGAImage &screen, TGAImage &diffuse_map, float zBuffer[], const TGAColor &c, float intensity, bool outline = true)
    {
        Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
        Vec2f clamp(screen.get_width() - 1, screen.get_height() - 1);

        for (int i = 0; i < 3; i++)
        {
            bboxmin.x = std::max(0.f, std::min(screen_coords[i].x, bboxmin.x));
            bboxmin.y = std::max(0.f, std::min(screen_coords[i].y, bboxmin.y));
            bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, screen_coords[i].x));
            bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, screen_coords[i].y));
        }

        Vec3f p(bboxmin.x, bboxmin.y, 0);
        int   img_width  = screen.get_width();
        int   img_height = screen.get_height();

        Vec2f tx_p;

        std::array<Vec2f, 3> screen_pts;
        for (int i = 0; i < screen_pts.size(); i++)
        {
            screen_pts[i] = Vec2f(screen_coords[i].x, screen_coords[i].y);
        }

        for (p.y = bboxmin.y; p.y <= bboxmax.y; p.y++)
        {
            for (p.x = bboxmin.x; p.x <= bboxmax.x; p.x++)
            {
                Vec3f res = barycentric(screen_pts, Vec2f(p.x, p.y));
                if (res.x < 0 || res.y < 0 || res.z < 0) continue;

                // get the z value of the point from
                p.z  = 0;
                tx_p = Vec2f(0, 0);
                for (int i = 0; i < 3; i++) // barycentric(x+y+z=1)  (p=p1x+p2y+p3z)
                {
                    // depth of p
                    p.z += screen_coords[i].z * res[i];

                    // texture coordinate of p
                    tx_p.u += uvs[i].u * res[i];
                    tx_p.v += uvs[i].v * res[i];
                }

                // std::cout << tx_p.u << " " << tx_p.v << std::endl;
                TGAColor col = diffuse_map.get(tx_p.u * diffuse_map.get_width(), (1 - tx_p.v) * diffuse_map.get_height());
                col.set_brightness(intensity);
                // TGAColor col = TGAColor(res.x * 255, res.y * 255, res.z * 255, 255);

                int zIndex = p.y * img_width + p.x;
                if (zBuffer[zIndex] < p.z)
                {
                    screen.set(p.x, p.y, col);
                    zBuffer[zIndex] = p.z;
                }
            }
        }

        if (outline)
        {
            line5(screen_coords[0].x, screen_coords[0].y, screen_coords[1].x, screen_coords[1].y, screen, TGAColor(255, 50, 50, 255));
            line5(screen_coords[1].x, screen_coords[1].y, screen_coords[2].x, screen_coords[2].y, screen, TGAColor(255, 50, 50, 255));
            line5(screen_coords[2].x, screen_coords[2].y, screen_coords[0].x, screen_coords[0].y, screen, TGAColor(255, 50, 50, 255));
        }
    }

    inline Mat<float,4,4> perspectiveProjectionMatrix(float top=1, float bot=-1, float left=-1, float right=1, float near=5, float far=100) {
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

    inline Mat<float,4,4> translationMatrix(Vec3f v) {
        Mat<float,4,4> m = Mat<float,4,4>::identity();
        m[0][3] = v.x;
        m[1][3] = v.y;
        m[2][3] = v.z;
        return m;
    }

    /// @brief lookat matrix
    /// @param eye camera postion
    /// @param center target center 
    /// @param up
    /// @return Mat<float,4,4>
    inline Mat<float,4,4> lookat(Vec3f eye, Vec3f center, Vec3f up) {
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
    

} // namespace Tool