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

function printRGBdata(rgbData, options={scale:1, doubles:20}) {
    var scale = options.scale;
    var doubles = options.doubles;
    var width = rgbData.length / 3;
    var ele = document.createElement("canvas");
    document.body.appendChild(ele);
    document.body.appendChild(document.createElement("br"));
    ele.setAttribute("width", parseInt(width * scale));
    ele.setAttribute("height", parseInt(doubles * scale));
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
    var mult = 20;
    createAndAddGradient(mult, "#000", "#00F"); // black to blue
    createAndAddGradient(mult, "#00F", "#0FF"); // blue to cyan
    createAndAddGradient(mult, "#0FF", "#0F0"); // cyan to green
    createAndAddGradient(mult, "#0F0", "#FF0"); // green to yellow
    createAndAddGradient(mult, "#FF0", "#F00"); // green to red
    createAndAddGradient(mult, "#F00", "#FFF"); // red to white
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


function drawFromKeyColors(gradients, totalRange) {
    var keyColors = [];//gradients.keyColors;
    var keyProcents = [];//gradients.keyProcents;
    var keyRanges = new Int32Array(gradients.length);
    //var currRange = 0;
    for (var i = 0; i < gradients.length; i++)
    {
        //currRange += parseInt((totalRange * keyProcents[i]) / 100);
        keyRanges[i] = parseInt((totalRange * gradients[i].p) / 100);
        keyColors[i] = CRGB.FROM_VALUE(gradients[i].c);
    }
    // make sure that the last range item allways is equal to totalRange-1
    keyRanges[gradients.length-1] = (totalRange-1);
    
    //console.log("keyProcents:", keyProcents);
    //console.log("keyRanges:", keyRanges);
    //console.log("keyColors:",keyColors);

    var colorMap = [];
    for (var i = 0; i < (keyColors.length-1); i++)
    {
       fill_gradient_RGB(colorMap, keyRanges[i], keyColors[i], keyRanges[i+1], keyColors[i+1]);
    }
    //console.log(colorMap);
    var rgbData = convertToFlatRGB(colorMap);
    printRGBdata(rgbData, {scale:2,doubles:20});
}

function addHeader(text)
{
    var div = document.createElement("div");
    div.innerHTML = text + "<br>";
    document.body.appendChild(div);
}

function main() {
    var width = 240;
    drawIronBow();
    drawFromKeyColors(IronBow, width);

    drawRainBow();
    drawFromKeyColors(RainBow, width);
    
    printRGBdata(colorMap_arctic, {scale:2,doubles:20});
    drawFromKeyColors(arctic, colorMap_arctic.length/3);

    printRGBdata(colorMap_blackHot, {scale:2,doubles:20});
    drawFromKeyColors(blackHot, colorMap_blackHot.length/3);

    printRGBdata(colorMap_blueRed, {scale:2,doubles:20});
    drawFromKeyColors(blueRed, colorMap_blueRed.length/3);

    printRGBdata(colorMap_coldest, {scale:2,doubles:20});
    drawFromKeyColors(coldest, colorMap_coldest.length/3);

    printRGBdata(colorMap_contrast, {scale:2,doubles:20});
    drawFromKeyColors(contrast, colorMap_contrast.length/3);
    
    var options = {scale:1,doubles:20};
    for (var i = 5; i < color_schemes.length; i++) {
        
        addHeader(color_schemes[i].name + "&nbsp;&nbsp;&nbsp;&nbsp;width:" + color_schemes[i].map.length/3);
        printRGBdata(color_schemes[i].map, options);
    }
  
    // ### dev tests ###
    //testDifferentGradientGenerators();
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