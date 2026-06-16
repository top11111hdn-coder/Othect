#include <math.h>
#include <stdio.h>

#define IMAGE_HEIGHT 1050
#define IMAGE_WIDTH 1650

struct point_f { float x_, y_; };
struct color { float r_, g_, b_; };

struct cube_bezier_curve {
    struct color curve_color_;
    struct point_f point_[4];
};

struct point_f get_bezier_point(float t, const struct cube_bezier_curve *curve);
struct color get_color(const struct point_f *point, const struct point_f bezier_curve_points[], 
                        const struct color *bezier_curve_color, int segments);

int main() {
    int SEGMENTS = 200; // Увеличил количество сегментов для более гладкой линии
    struct point_f bezier_curve_point_segments[SEGMENTS];

    // Уникальная S-образная кривая
    struct cube_bezier_curve curve = {
        {0.2f, 1.0f, 0.3f}, // Ярко-зеленый цвет
        {{0.1f, 0.8f}, {0.9f, 0.9f}, {0.1f, 0.1f}, {0.9f, 0.2f}}
    };
    
    for (int i = 0; i < SEGMENTS; ++i) {
        float t = (float)i / (SEGMENTS - 1);
        bezier_curve_point_segments[i] = get_bezier_point(t, &curve);
    }

    printf("P3\n%d %d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    for (int j = 0; j < IMAGE_HEIGHT; ++j) {
        for (int i = 0; i < IMAGE_WIDTH; ++i) {
            struct point_f point = { (float)i / IMAGE_WIDTH, (float)j / IMAGE_HEIGHT };
            struct color pixel_color = get_color(&point, bezier_curve_point_segments, &curve.curve_color_, SEGMENTS);
            
            printf("%d %d %d\n", (int)(pixel_color.r_ * 255.999f), 
                                 (int)(pixel_color.g_ * 255.999f), 
                                 (int)(pixel_color.b_ * 255.999f));
        }
    }
    return 0;
}

struct point_f get_bezier_point(float t, const struct cube_bezier_curve *curve) {
    float a = 1 - t;
    float a2 = a * a; float a3 = a2 * a;
    float t2 = t * t; float t3 = t2 * t;
    struct point_f p;
    p.x_ = a3*curve->point_[0].x_ + 3*a2*t*curve->point_[1].x_ + 3*a*t2*curve->point_[2].x_ + t3*curve->point_[3].x_;
    p.y_ = a3*curve->point_[0].y_ + 3*a2*t*curve->point_[1].y_ + 3*a*t2*curve->point_[2].y_ + t3*curve->point_[3].y_;
    return p;
}

float distance(const struct point_f *u, const struct point_f *v) {
    float dx = u->x_ - v->x_, dy = u->y_ - v->y_;
    return sqrt(dx * dx + dy * dy);
}

struct color get_color(const struct point_f *point, const struct point_f curve_points[], 
                        const struct color *c_color, int segments) {
    float dist = 1.0f;
    for (int i = 0; i < segments; ++i) {
        float d = distance(point, &curve_points[i]);
        if (dist > d) dist = d;
    }
    
    struct color bg = {0.0f, 0.0f, 0.0f}; // Черный фон
    float sigma = 0.08f; // Уменьшил сигму для более тонкой и яркой линии
    float weight = (float)exp(-dist*dist/(2*sigma*sigma));
    
    return (struct color){bg.r_*(1-weight) + c_color->r_*weight, 
                          bg.g_*(1-weight) + c_color->g_*weight, 
                          bg.b_*(1-weight) + c_color->b_*weight};
}