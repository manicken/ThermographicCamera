
#ifndef GRADIENT_PALETTE_STRUCTS_H
#define GRADIENT_PALETTE_STRUCTS_H

#include <Arduino.h>

struct CRGB {
    union {
		struct {
            union {
                uint8_t padding;
            };
            union {
                uint8_t r;
                uint8_t red;
            };
            union {
                uint8_t g;
                uint8_t green;
            };
            union {
                uint8_t b;
                uint8_t blue;
            };
        };
		uint32_t raw;
	};

    inline uint16_t toRGB565() const {
        return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
    }

    // default values are UNINITIALIZED
    inline CRGB() __attribute__((always_inline)) = default;
    /// allow construction from R, G, B
    inline CRGB( uint8_t ir, uint8_t ig, uint8_t ib)  __attribute__((always_inline))
        : r(ir), g(ig), b(ib)
    {
    }

    /// allow construction from 32-bit (really 24-bit) bit 0xRRGGBB color code
    inline CRGB( uint32_t colorcode)  __attribute__((always_inline))
    : raw(colorcode)//r((colorcode >> 16) & 0xFF), g((colorcode >> 8) & 0xFF), b((colorcode >> 0) & 0xFF)
    {
    }
};

struct GradientPaletteItem {
    float procent;
    const struct CRGB color;
};

struct GradientPaletteDef {
    char *name;
    uint16_t itemCount;
};

#endif