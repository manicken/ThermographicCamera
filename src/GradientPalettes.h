//#include "HTMLColors.h"

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

//cannot be initialized in stupid c++
struct GradientPalette {
    char *name;
    u_int16_t itemCount;
    struct GradientPaletteItem *items;
};

const struct GradientPaletteItem IronBow[] = {
    {0, {0x00,0x00,0x00}}, // black
    {10.0f, 0x20008C}, // dark blue
    {35.0f, 0xCC0077}, //magenta red
    {70.0f, 0xFFA500}, // orange
    {85.0f, 0xFFD700}, // gold
    {100.0f,0xFFFFFF}  // white
};

const struct GradientPaletteItem RainBow[] = {
    {0, 0x000000},
    {100/6, 0x0000FF},
    {200/6, 0x00FFFF},
    {300/6, 0x00FF00},
    {400/6, 0xFFFF00},
    {500/6, 0xFF0000},
    {100, 0xFFFFFF}
};

/*
struct GradientPaletteItem** GetFromIndex(uint8_t index)
{
    if (index == 0)
        return IronBow;
}
*/

const struct GradientPalette IronBow = {
    "Iron Bow", 6, {
        {0, 0x000000}, // black
        {10.0f, 0x20008C} // dark blue
    }
};



void acessorTest()
{
    //Palettes[i].
}

