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

var IronBow = [
    {p:0, c:COLOR.BLACK},
    {p:10, c:COLOR.DARKBLUE},
    {p:35, c:COLOR.MAGENTA_RED},
    {p:70, c:COLOR.ORANGE},
    {p:85, c:COLOR.GOLD},
    {p:100, c:COLOR.WHITE}
]

var RainBow = [
    {p:0, c:COLOR.BLACK},
    {p:100/6, c:COLOR.BLUE},
    {p:200/6, c:COLOR.CYAN},
    {p:300/6, c:COLOR.GREEN},
    {p:400/6, c:COLOR.YELLOW},
    {p:500/6, c:COLOR.RED},
    {p:100, c:COLOR.WHITE}
];

var arctic = [
    {p:0, c:FROM_RGB(15,16,146)},
    {p:11, c:FROM_RGB(15,34,239)},   // 66/6  = 11
    {p:29, c:FROM_RGB(54,227,232)}, // 168/6 = 28
    {p:50, c:FROM_RGB(107,101,97)}, // 300/6 = 50
    {p:72, c:FROM_RGB(237,101,13)}, // 432/6 = 72
    {p:87, c:FROM_RGB(235,202,13)}, // 522/6 = 87
    {p:100, c:FROM_RGB(235,230,194)},
];

var blackHot = [
    {p:0, c:FROM_RGB(255,255,255)},
    {p:100, c:FROM_RGB(0,0,0)}
];

var blueRed = [
    {p:0, c:FROM_RGB(19,64,206)},
    {p:15, c:FROM_RGB(32,141,232)},
    {p:34, c:FROM_RGB(83,225,161)},
    {p:50, c:FROM_RGB(156,227,87)},
    {p:67, c:FROM_RGB(230,147,33)},
    {p:85, c:FROM_RGB(207,64,16)},
    {p:100, c:FROM_RGB(126,28,15)}

];

var coldest = [
    {p:0, c:FROM_RGB(15,15,239)},
    {p:13, c:FROM_RGB(15,15,239)},
    {p:13, c:FROM_RGB(45,45,45)},
    {p:100, c:FROM_RGB(255,255,255)},
];

var contrast = [
    {p:0, c:FROM_RGB(16,16,16)},
    {p:100/8, c:FROM_RGB(217,14,216)},
    {p:195/8, c:FROM_RGB(15,15,149)},
    {p:290/8, c:FROM_RGB(12,214,214)},
    {p:386/8, c:FROM_RGB(15,84,15)},
    {p:492/8, c:FROM_RGB(213,215,6)},
    {p:595/8, c:FROM_RGB(121,15,15)},
    {p:695/8, c:FROM_RGB(203,55,55)},
    {p:100, c:FROM_RGB(255,255,255)}
];