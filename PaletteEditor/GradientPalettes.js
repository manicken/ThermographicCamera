var COLOR = {
    WHITE:0xFFFFFF,
    GOLD:0xFFD700,
    ORANGE:0xFFA500,
    MAGENTA_RED:0xCC0077,
    DARKBLUE:0x20008C,
    BLACK:0x000000,
    RED:0xFF0000,
    YELLOW:0xFFFF00,
    GREEN:0x00FF00,
    CYAN:0x00FFFF,
    BLUE:0x0000FF,
    MAGENTA:0xFF00FF
}

class CRGB
{
	constructor(red,green,blue)
    {
    	this.r = parseInt(red);
        this.g = parseInt(green);
        this.b = parseInt(blue);
    }
    /*constructor (crgb)
    {
    	this.r = crgb.r;
        this.g = crgb.g;
        this.b = crgb.b;
    }*/

    static FROM_VALUE(value)
    {
        return new CRGB((value >> 16) & 0xFF,
                        (value >> 8) & 0xFF,
                        value & 0xFF)
    }
    asArray()
    {
        return [this.r,this.g,this.b];
    }
}

var gps = {
    IronBow:[
        {p:0, c:new CRGB(0,0,0)},        // black
        {p:10, c:new CRGB(32,0,140)},    // dark blue
        {p:35, c:new CRGB(204,0,119)},   // magenta red
        {p:70, c:new CRGB(255,165,0)},   // orange
        {p:85, c:new CRGB(255,215,0)},   // gold
        {p:100, c:new CRGB(255,255,255)} // white
    ],

    RainBow:[
        {p:0,     c:new CRGB(0,0,0)},      // white
        {p:100/6, c:new CRGB(0,0,255)},    // blue
        {p:200/6, c:new CRGB(0,255,255)},  // cyan
        {p:300/6, c:new CRGB(0,255,0)},    // green
        {p:400/6, c:new CRGB(255,255,0)},  // yellow
        {p:500/6, c:new CRGB(255,0,0)},    // red
        {p:100,   c:new CRGB(255,255,255)} // white
    ],

    arctic:[
        {p:0, c:new CRGB(15,16,146)},
        {p:11, c:new CRGB(15,34,239)},   // 66/6  = 11
        {p:29, c:new CRGB(54,227,232)}, // 168/6 = 28
        {p:50, c:new CRGB(107,101,97)}, // 300/6 = 50
        {p:72, c:new CRGB(237,101,13)}, // 432/6 = 72
        {p:87, c:new CRGB(235,202,13)}, // 522/6 = 87
        {p:100, c:new CRGB(235,230,194)},
    ],

    blackHot:[
        {p:0, c:new CRGB(235,235,235)},
        {p:100, c:new CRGB(16,16,16)}
    ],

    blueRed:[
        {p:0, c:new CRGB(19,64,206)},
        {p:15, c:new CRGB(32,141,232)},
        {p:34, c:new CRGB(83,225,161)},
        {p:50, c:new CRGB(156,227,87)},
        {p:67, c:new CRGB(230,147,33)},
        {p:85, c:new CRGB(207,64,16)},
        {p:100, c:new CRGB(126,28,15)}

    ],
    coldest:[
        {p:0, c:new CRGB(15,15,239)}, // blue
        {p:13.5, c:new CRGB(15,15,239)}, // blue
        {p:13.5, c:new CRGB(45,45,45)}, // dark-grey
        {p:100, c:new CRGB(255,255,255)}, // white
    ],

    contrast:[
        {p:0, c:new CRGB(16,16,16)}, // black-ish
        {p:99/8, c:new CRGB(217,14,216)}, // magenta-ish
        {p:195/8, c:new CRGB(15,15,149)}, // blue-ish
        {p:290/8, c:new CRGB(12,214,214)}, // cyan-ish
        {p:386/8, c:new CRGB(15,84,15)}, // green-ish
        {p:492/8, c:new CRGB(213,215,6)}, // yellow-ish
        {p:595/8, c:new CRGB(121,15,15)}, // darkred-ish
        {p:695/8, c:new CRGB(203,55,55)}, // lightred-ish
        {p:100, c:new CRGB(255,255,255)} // white
    ],

    doubleRainbow:[
        {p:0, c:new CRGB(18,15,18)}, // black-ish
        {p:75/12, c:new CRGB(155,42,136)}, // magenta-ish
        {p:165/12, c:new CRGB(59,103,238)}, // blue-ish
        {p:250/12, c:new CRGB(27,153,75)}, // green-ish
        {p:318/12, c:new CRGB(194,193,13)}, // yellow-ish
        {p:505/12, c:new CRGB(169,45,45)}, // red-ish
        {p:565/12, c:new CRGB(193,146,238)}, // pink-ish
        {p:596/12, c:new CRGB(203,168,236)}, // pink2-ish
        {p:680/12, c:new CRGB(68,171,202)}, // blue2-ish
        {p:800/12, c:new CRGB(99,189,71)}, // green2-ish
        {p:910/12, c:new CRGB(236,234,13)}, // yellow2-ish
        {p:1065/12, c:new CRGB(225,48,52)}, // red2-ish
        {p:100, c:new CRGB(235,232,235)}, // white
    ],

    glowBow:[
        {p:0, c:new CRGB(16,16,16)},
        {p:25, c:new CRGB(194,34,48)},
        {p:50, c:new CRGB(236,105,13)},
        {p:75, c:new CRGB(236,201,11)},
        {p:100, c:new CRGB(235,235,235)},
    ],

    grayRed:[
        {p:0, c:new CRGB(218,186,175)}, // gray-ish
        {p:100/3, c:new CRGB(61,130,123)}, // bluegreen-ish
        {p:170/3, c:new CRGB(103,93,90)}, // gray-ish
        {p:100, c:new CRGB(227,15,18)}, // red-ish
    ],

    grayscale:[
        {p:0, c:new CRGB(0,0,0)},
        {p:100, c:new CRGB(255,255,255)}
    ],

    hottest:[
        {p:0, c:new CRGB(16,16,16)}, // black-ish
        {p:87, c:new CRGB(206,206,206)}, // white-ish
        {p:87, c:new CRGB(190,14,13)}, // red
        {p:100, c:new CRGB(190,14,13)}, // red
    ],

    lava:[
        {p:0, c:new CRGB(17,19,22)},
        {p:70/8, c:new CRGB(41,76,156)},
        {p:270/8, c:new CRGB(14,137,131)},
        {p:335/8, c:new CRGB(115,62,122)},
        {p:470/8, c:new CRGB(194,34,48)},
        {p:600/8, c:new CRGB(237,97,13)},
        {p:730/8, c:new CRGB(235,208,45)},
        {p:100, c:new CRGB(235,235,235)},
    ],

    medical:[
        {p:0, c:new CRGB(36,36,198)},    // blue-ish
        {p:10, c:new CRGB(36,36,198)},   // blue-ish
        {p:10, c:new CRGB(70,71,238)},   // violet-ish
        {p:20.5, c:new CRGB(70,71,238)}, // violet-ish
        {p:20.5, c:new CRGB(25,172,193)},// cyan-ish
        {p:30, c:new CRGB(25,172,193)},  // cyan-ish
        {p:30, c:new CRGB(14,158,13)},   // green-ish
        {p:40.5, c:new CRGB(14,158,13)}, // green-ish
        {p:40.5, c:new CRGB(15,15,123)}, // darkblue-ish
        {p:50, c:new CRGB(15,15,123)},   // darkblue-ish
        {p:50, c:new CRGB(237,65,197)},  // darkpink-ish
        {p:60.5, c:new CRGB(237,65,197)},// darkpink-ish
        {p:60.5, c:new CRGB(238,28,28)}, // red-ish
        {p:70.5, c:new CRGB(238,28,28)},   // red-ish
        {p:70.5, c:new CRGB(236,152,93)},  // beige-ish
        {p:80.5, c:new CRGB(236,152,93)},  // beige-ish
        {p:80.5, c:new CRGB(230,125,12)},  // orange-ish
        {p:90.5, c:new CRGB(230,125,12)},  // orange-ish
        {p:90.5, c:new CRGB(236,196,37)},  // yellow-ish
        {p:100, c:new CRGB(236,196,37)}, // yellow-ish
    ],

    rainbow:[
        {p:0, c:new CRGB(1,3,74)},
        {p:25, c:new CRGB(3,129,212)},
        {p:38, c:new CRGB(67,171,60)},
        {p:55, c:new CRGB(236,209,7)},
        {p:67, c:new CRGB(255,157,23)},
        {p:78, c:new CRGB(250,27,65)},
        {p:100, c:new CRGB(255,233,208)}
    ],

    wheel1:[
        {p:0, c:new CRGB(238,13,239)},
        {p:20, c:new CRGB(14,236,13)},
        {p:40, c:new CRGB(20,18,235)},
        {p:60, c:new CRGB(236,234,13)},
        {p:80, c:new CRGB(14,236,237)},
        {p:100, c:new CRGB(240,14,16)},
    ],

    wheel2:[
        {p:0, c:new CRGB(17,14,17)}, // black
        {p:10.5, c:new CRGB(14,236,13)}, // green
        {p:21.4, c:new CRGB(238,14,239)}, // magenta
        {p:39.5, c:new CRGB(15,235,237)}, // cyan
        {p:59, c:new CRGB(239,14,18)}, // red
        {p:77.5, c:new CRGB(23,21,232)}, // blue
        {p:100, c:new CRGB(235,233,14)} // yellow
    ],

    wheel3:[
        {p:0, c:new CRGB(17,14,17)}, // black
        {p:100/11, c:new CRGB(128,13,240)}, // violet
        {p:200/11, c:new CRGB(15,235,12)}, // green
        {p:300/11, c:new CRGB(238,14,239)}, // magenta
        {p:398/11, c:new CRGB(13,236,125)}, // green (lighter)
        {p:495/11, c:new CRGB(238,14,125)}, // red
        {p:596/11, c:new CRGB(15,235,237)}, // cyan
        {p:695/11, c:new CRGB(239,14,18)}, // red
        {p:794/11, c:new CRGB(15,122,238)}, // blue
        {p:890/11, c:new CRGB(237,122,15)}, // orange
        {p:990/11, c:new CRGB(15,14,241)}, // blue
        {p:100, c:new CRGB(235,233,14)} // yellow
    ],

    whiteHot:[
        {p:0, c:new CRGB(16,16,16)},
        {p:100, c:new CRGB(235,235,235)}
    ],

    yellow:[
        {p:0, c:new CRGB(62,16,15)},
        {p:20, c:new CRGB(79,65,14)},
        {p:60, c:new CRGB(185,125,14)},
        {p:100, c:new CRGB(212,235,11)}
    ],
};