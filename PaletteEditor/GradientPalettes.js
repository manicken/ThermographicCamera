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
function FROM_RGB(r,g,b)
{
    return r*65536 + g*256 + b; 
}
var gps = {
    IronBow:[
        {p:0, c:COLOR.BLACK},
        {p:10, c:COLOR.DARKBLUE},
        {p:35, c:COLOR.MAGENTA_RED},
        {p:70, c:COLOR.ORANGE},
        {p:85, c:COLOR.GOLD},
        {p:100, c:COLOR.WHITE}
    ],

    RainBow:[
        {p:0, c:COLOR.BLACK},
        {p:100/6, c:COLOR.BLUE},
        {p:200/6, c:COLOR.CYAN},
        {p:300/6, c:COLOR.GREEN},
        {p:400/6, c:COLOR.YELLOW},
        {p:500/6, c:COLOR.RED},
        {p:100, c:COLOR.WHITE}
    ],

    arctic:[
        {p:0, c:FROM_RGB(15,16,146)},
        {p:11, c:FROM_RGB(15,34,239)},   // 66/6  = 11
        {p:29, c:FROM_RGB(54,227,232)}, // 168/6 = 28
        {p:50, c:FROM_RGB(107,101,97)}, // 300/6 = 50
        {p:72, c:FROM_RGB(237,101,13)}, // 432/6 = 72
        {p:87, c:FROM_RGB(235,202,13)}, // 522/6 = 87
        {p:100, c:FROM_RGB(235,230,194)},
    ],

    blackHot:[
        {p:0, c:FROM_RGB(235,235,235)},
        {p:100, c:FROM_RGB(16,16,16)}
    ],

    blueRed:[
        {p:0, c:FROM_RGB(19,64,206)},
        {p:15, c:FROM_RGB(32,141,232)},
        {p:34, c:FROM_RGB(83,225,161)},
        {p:50, c:FROM_RGB(156,227,87)},
        {p:67, c:FROM_RGB(230,147,33)},
        {p:85, c:FROM_RGB(207,64,16)},
        {p:100, c:FROM_RGB(126,28,15)}

    ],
    coldest:[
        {p:0, c:FROM_RGB(15,15,239)}, // blue
        {p:13.5, c:FROM_RGB(15,15,239)}, // blue
        {p:13.5, c:FROM_RGB(45,45,45)}, // dark-grey
        {p:100, c:FROM_RGB(255,255,255)}, // white
    ],

    contrast:[
        {p:0, c:FROM_RGB(16,16,16)}, // black-ish
        {p:99/8, c:FROM_RGB(217,14,216)}, // magenta-ish
        {p:195/8, c:FROM_RGB(15,15,149)}, // blue-ish
        {p:290/8, c:FROM_RGB(12,214,214)}, // cyan-ish
        {p:386/8, c:FROM_RGB(15,84,15)}, // green-ish
        {p:492/8, c:FROM_RGB(213,215,6)}, // yellow-ish
        {p:595/8, c:FROM_RGB(121,15,15)}, // darkred-ish
        {p:695/8, c:FROM_RGB(203,55,55)}, // lightred-ish
        {p:100, c:FROM_RGB(255,255,255)} // white
    ],

    doubleRainbow:[
        {p:0, c:FROM_RGB(18,15,18)}, // black-ish
        {p:75/12, c:FROM_RGB(155,42,136)}, // magenta-ish
        {p:165/12, c:FROM_RGB(59,103,238)}, // blue-ish
        {p:250/12, c:FROM_RGB(27,153,75)}, // green-ish
        {p:318/12, c:FROM_RGB(194,193,13)}, // yellow-ish
        {p:505/12, c:FROM_RGB(169,45,45)}, // red-ish
        {p:565/12, c:FROM_RGB(193,146,238)}, // pink-ish
        {p:596/12, c:FROM_RGB(203,168,236)}, // pink2-ish
        {p:680/12, c:FROM_RGB(68,171,202)}, // blue2-ish
        {p:800/12, c:FROM_RGB(99,189,71)}, // green2-ish
        {p:910/12, c:FROM_RGB(236,234,13)}, // yellow2-ish
        {p:1065/12, c:FROM_RGB(225,48,52)}, // red2-ish
        {p:100, c:FROM_RGB(235,232,235)}, // white
    ],

    glowBow:[
        {p:0, c:FROM_RGB(16,16,16)},
        {p:25, c:FROM_RGB(194,34,48)},
        {p:50, c:FROM_RGB(236,105,13)},
        {p:75, c:FROM_RGB(236,201,11)},
        {p:100, c:FROM_RGB(235,235,235)},
    ],

    grayRed:[
        {p:0, c:FROM_RGB(218,186,175)}, // gray-ish
        {p:100/3, c:FROM_RGB(61,130,123)}, // bluegreen-ish
        {p:170/3, c:FROM_RGB(103,93,90)}, // gray-ish
        {p:100, c:FROM_RGB(227,15,18)}, // red-ish
    ],

    grayscale:[
        {p:0, c:FROM_RGB(0,0,0)},
        {p:100, c:FROM_RGB(255,255,255)}
    ],

    hottest:[
        {p:0, c:FROM_RGB(16,16,16)}, // black-ish
        {p:87, c:FROM_RGB(206,206,206)}, // white-ish
        {p:87, c:FROM_RGB(190,14,13)}, // red
        {p:100, c:FROM_RGB(190,14,13)}, // red
    ],

    lava:[
        {p:0, c:FROM_RGB(17,19,22)},
        {p:70/8, c:FROM_RGB(41,76,156)},
        {p:270/8, c:FROM_RGB(14,137,131)},
        {p:335/8, c:FROM_RGB(115,62,122)},
        {p:470/8, c:FROM_RGB(194,34,48)},
        {p:600/8, c:FROM_RGB(237,97,13)},
        {p:730/8, c:FROM_RGB(235,208,45)},
        {p:100, c:FROM_RGB(235,235,235)},
    ],

    medical:[
        {p:0, c:FROM_RGB(36,36,198)},    // blue-ish
        {p:10, c:FROM_RGB(36,36,198)},   // blue-ish
        {p:10, c:FROM_RGB(70,71,238)},   // violet-ish
        {p:20.5, c:FROM_RGB(70,71,238)}, // violet-ish
        {p:20.5, c:FROM_RGB(25,172,193)},// cyan-ish
        {p:30, c:FROM_RGB(25,172,193)},  // cyan-ish
        {p:30, c:FROM_RGB(14,158,13)},   // green-ish
        {p:40.5, c:FROM_RGB(14,158,13)}, // green-ish
        {p:40.5, c:FROM_RGB(15,15,123)}, // darkblue-ish
        {p:50, c:FROM_RGB(15,15,123)},   // darkblue-ish
        {p:50, c:FROM_RGB(237,65,197)},  // darkpink-ish
        {p:60.5, c:FROM_RGB(237,65,197)},// darkpink-ish
        {p:60.5, c:FROM_RGB(238,28,28)}, // red-ish
        {p:70.5, c:FROM_RGB(238,28,28)},   // red-ish
        {p:70.5, c:FROM_RGB(236,152,93)},  // beige-ish
        {p:80.5, c:FROM_RGB(236,152,93)},  // beige-ish
        {p:80.5, c:FROM_RGB(230,125,12)},  // orange-ish
        {p:90.5, c:FROM_RGB(230,125,12)},  // orange-ish
        {p:90.5, c:FROM_RGB(236,196,37)},  // yellow-ish
        {p:100, c:FROM_RGB(236,196,37)}, // yellow-ish
    ],

    rainbow:[
        {p:0, c:FROM_RGB(1,3,74)},
        {p:25, c:FROM_RGB(3,129,212)},
        {p:38, c:FROM_RGB(67,171,60)},
        {p:55, c:FROM_RGB(236,209,7)},
        {p:67, c:FROM_RGB(255,157,23)},
        {p:78, c:FROM_RGB(250,27,65)},
        {p:100, c:FROM_RGB(255,233,208)}
    ],

    wheel1:[
        {p:0, c:FROM_RGB(238,13,239)},
        {p:20, c:FROM_RGB(14,236,13)},
        {p:40, c:FROM_RGB(20,18,235)},
        {p:60, c:FROM_RGB(236,234,13)},
        {p:80, c:FROM_RGB(14,236,237)},
        {p:100, c:FROM_RGB(240,14,16)},
    ],

    wheel2:[
        {p:0, c:FROM_RGB(17,14,17)}, // black
        {p:10.5, c:FROM_RGB(14,236,13)}, // green
        {p:21.4, c:FROM_RGB(238,14,239)}, // magenta
        {p:39.5, c:FROM_RGB(15,235,237)}, // cyan
        {p:59, c:FROM_RGB(239,14,18)}, // red
        {p:77.5, c:FROM_RGB(23,21,232)}, // blue
        {p:100, c:FROM_RGB(235,233,14)} // yellow
    ],

    wheel3:[
        {p:0, c:FROM_RGB(17,14,17)}, // black
        {p:100/11, c:FROM_RGB(128,13,240)}, // violet
        {p:200/11, c:FROM_RGB(15,235,12)}, // green
        {p:300/11, c:FROM_RGB(238,14,239)}, // magenta
        {p:398/11, c:FROM_RGB(13,236,125)}, // green (lighter)
        {p:495/11, c:FROM_RGB(238,14,125)}, // red
        {p:596/11, c:FROM_RGB(15,235,237)}, // cyan
        {p:695/11, c:FROM_RGB(239,14,18)}, // red
        {p:794/11, c:FROM_RGB(15,122,238)}, // blue
        {p:890/11, c:FROM_RGB(237,122,15)}, // orange
        {p:990/11, c:FROM_RGB(15,14,241)}, // blue
        {p:100, c:FROM_RGB(235,233,14)} // yellow
    ],

    whiteHot:[
        {p:0, c:FROM_RGB(16,16,16)},
        {p:100, c:FROM_RGB(235,235,235)}
    ],

    yellow:[
        {p:0, c:FROM_RGB(62,16,15)},
        {p:20, c:FROM_RGB(79,65,14)},
        {p:60, c:FROM_RGB(185,125,14)},
        {p:100, c:FROM_RGB(212,235,11)}
    ],
};