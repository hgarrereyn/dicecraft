
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>

class PerlinNoise {

    static float clamp(float x, float a, float b) {
        return x > b ? b : (x < a ? a : x);
    }

    static glm::vec2 randomGradient(int ix, int iy) {
        const unsigned w = 8 * sizeof(unsigned);
        const unsigned s = w / 2; // rotation width
        unsigned a = ix, b = iy;
        a *= 3284157443; b ^= a << s | a >> (w-s);
        b *= 1911520717; a ^= b << s | b >> (w-s);
        a *= 2048419325;
        float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
        return glm::vec2(glm::sin(random), glm::cos(random));
    }

    static float interpolate(float a0, float a1, float w) {
        return (a1 - a0) * clamp(w, 0, 1) + a0;
    }

    static float dotGridGradient(int ix, int iy, float x, float y) {
        // Get gradient from integer coordinates
        glm::vec2 gradient = randomGradient(ix, iy);

        // Compute the distance vector
        float dx = x - (float)ix;
        float dy = y - (float)iy;

        // Compute the dot-product
        return (dx * gradient.x + dy * gradient.y);
    }

public:
    static float perlin(float x, float y) {
        // Determine grid cell coordinates
        x += 0x2000;
        y += 0x2000;
        int x0 = (int)x;
        int x1 = x0 + 1;
        int y0 = (int)y;
        int y1 = y0 + 1;

        // Determine interpolation weights
        // Could also use higher order polynomial/s-curve here
        float sx = x - (float)x0;
        float sy = y - (float)y0;

        // Interpolate between grid point gradients
        float n0, n1, ix0, ix1, value;

        n0 = dotGridGradient(x0, y0, x, y);
        n1 = dotGridGradient(x1, y0, x, y);
        ix0 = interpolate(n0, n1, sx);

        n0 = dotGridGradient(x0, y1, x, y);
        n1 = dotGridGradient(x1, y1, x, y);
        ix1 = interpolate(n0, n1, sx);

        value = interpolate(ix0, ix1, sy);
        return value;
    }
};
