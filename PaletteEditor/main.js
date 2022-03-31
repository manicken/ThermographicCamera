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

var COLOR = {
    WHITE:0xFFFFFF,
    GOLD:0xFFD700,
    ORANGE:0xFFA500,
    MAGENTA_RED:0xCC0077,
    DARKBLUE:0x20008C,
    BLACK:0x000000,
}

function createAndAddGradient(count, color1, color2) {
    var height = 40;
    count *= 4;
    var ele = document.createElement("canvas");
    ele.setAttribute("class", "gradient");
    document.body.appendChild(ele);
    ele.setAttribute("width", count);
    ele.setAttribute("height", height);


    var ctx = ele.getContext("2d");
    var gr = ctx.createLinearGradient(0, 0, count, 0);
    gr.addColorStop(0, color1);
    gr.addColorStop(1, color2);
    ctx.fillStyle = gr;
    ctx.fillRect(0, 0, count, height);
}

function putRGBdata(rgb, imageData, doubles) {
    doubles = (doubles != undefined) ? doubles : 1;
    var pixels = (rgb.length / 3);
    for (var di = 0; di < doubles; di++) {
        for (var i = 0; i < pixels; i++) {
            imageData.data[di * pixels * 4 + 4 * i] = rgb[3 * i];
            imageData.data[di * pixels * 4 + 4 * i + 1] = rgb[3 * i + 1];
            imageData.data[di * pixels * 4 + 4 * i + 2] = rgb[3 * i + 2];
            imageData.data[di * pixels * 4 + 4 * i + 3] = 255; // non transparent
        }
    }
}

function printRGBdata(rgbData) {
    var scale = 2;
    var doubles = 20;
    var width = rgbData.length / 3;
    var ele = document.createElement("canvas");
    document.body.appendChild(ele);
    document.body.appendChild(document.createElement("br"));
    ele.setAttribute("width", width * scale);
    ele.setAttribute("height", doubles * scale);
    var ctx = ele.getContext("2d");
    var imageData = ctx.createImageData(width, doubles);

    putRGBdata(rgbData, imageData, doubles)
    if (scale > 1)
        imageData = scaleImageData(ctx, imageData, scale);
    ctx.putImageData(imageData, 0, 0);

}

function drawIronBow() {
    var mult = 12; // calculated from 240/(1+2.5+3.5+1.5+1.5)=240/10=24
    createAndAddGradient((2 / 2) * mult, "black", "#20008c"); // 2
    createAndAddGradient((5 / 2) * mult, "#20008c", "#c07"); // 4
    createAndAddGradient((7 / 2) * mult, "#c07", "orange"); // 8
    createAndAddGradient((3 / 2) * mult, "orange", "gold"); // 3
    createAndAddGradient((3 / 2) * mult, "gold", "white"); // 3 
    document.body.appendChild(document.createElement("br"));
}

function drawRainBow() {
    var mult = 36.5;
    createAndAddGradient(mult, "#000", "#00F");
    createAndAddGradient(mult, "#00F", "#0FF");
    createAndAddGradient(mult, "#0FF", "#0F0");
    createAndAddGradient(mult, "#0F0", "#FF0");
    createAndAddGradient(mult, "#FF0", "#F00");
    createAndAddGradient(mult, "#F00", "#FFF");
    document.body.appendChild(document.createElement("br"));
}

/**
 * 
 * @param {CRGB[]} colors 
 * @returns {Uint8ClampedArray}
 */
function convertToFlatRGB(colors)
{
    var out = new Uint8ClampedArray(colors.length*3);
    for (var i=0;i<colors.length;i++)
    {
        out[i*3] = colors[i].r;
        out[i*3+1] = colors[i].g; 
        out[i*3+2] = colors[i].b; 
    }
    return out;
}


function drawFromKeyColors(keyColors, keyProcents, totalRange) {
    var keyRanges = new Int32Array(keyProcents.length);
    var currRange = 0;
    for (var i = 0; i < (keyProcents.length-1); i++)
    {
        currRange += parseInt((totalRange * keyProcents[i]) / 100);
        keyRanges[i] = currRange;
    }
    // make sure that the last range item is equal to totalRange-1
    keyRanges[keyRanges.length-1] = (totalRange-1);
    
    console.log("keyProcents:", keyProcents);
    console.log("keyRanges:", keyRanges);
    for (var i = 0; i < keyColors.length; i++)
    {
        keyColors[i] = CRGB.FROM_VALUE(keyColors[i]);
    }
    console.log("keyColors:",keyColors);
    var colorMap = [];
    for (var i = 0; i < (keyColors.length-1); i++)
    {
       fill_gradient_RGB(colorMap, keyRanges[i], keyColors[i], keyRanges[i+1], keyColors[i+1]);
    }
    console.log(colorMap);
    var rgbData = convertToFlatRGB(colorMap);
    printRGBdata(rgbData);
}

function main() {
    var keyColors = [COLOR.BLACK, COLOR.DARKBLUE, COLOR.MAGENTA_RED, COLOR.ORANGE, COLOR.GOLD, COLOR.WHITE];
    var keyProcents = [0, 10, 25, 35, 15, 15]; // in procents of 100

    drawIronBow();
    drawFromKeyColors(keyColors, keyProcents, 240);
    
    //testDifferentGradientGenerators();
    
    printRGBdata(colorMap_arctic);
    printRGBdata(colorMap_glowBow);
    
    drawRainBow();
    printRGBdata(colorMap_wheel1);

    

    //console.log(CRGB.FROM_VALUE(0xFF1112));
}
document.addEventListener("DOMContentLoaded", main());


function testDifferentGradientGenerators()
{
    var colorMap = [];
    getRGBdata(colorMap,0,0,1,0,255,255,16);
    console.log(colorMap);

    var colorMap = [];
    fill_gradient_RGB(colorMap, 0, new CRGB(0,0,1), 15, new CRGB(0,255,255) )
    console.log(colorMap);

    var colorMap = [];
    fill_gradient_RGB_lowres(colorMap, 0, new CRGB(0,0,1), 15, new CRGB(0,255,255) )
    console.log(colorMap);
}