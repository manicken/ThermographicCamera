



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
function generateGradientColorMap(colorMap, gradients, totalRange)
{
    for (var i = 0; i < (gradients.length-1); i++)
    {
        fill_gradient_RGB(
            colorMap, 
            parseInt((totalRange * gradients[i].p) / 100),
            gradients[i].c, 
            (i != (gradients.length-2))?parseInt((totalRange * gradients[i+1].p) / 100):(totalRange-1),
            gradients[i+1].c
        );
    }
}

function drawGradientColors(gradients, totalRange) {
    var colorMap = [];
    generateGradientColorMap(colorMap, gradients, totalRange);
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
    
    //printAll_ToCompare();

    printAllGradientPalettes();
    //printAll_ReferenceColorSchemes();
  
    // ### dev tests ###
    //testDifferentGradientGenerators();
    //console.log(CRGB.FROM_VALUE(0xFF1112));
}
document.addEventListener("DOMContentLoaded", main());

function printAllGradientPalettes()
{
    var names = Object.getOwnPropertyNames(gps);
    for (var i=0;i<names.length;i++)
    {
        var gp = gps[names[i]];
        addHeader(names[i]);
        drawGradientColors(gp, 480);
    }
}

function printAll_ToCompare()
{
    var width = 240;
    addHeader("IronBox (own)");
    drawIronBow();
    drawGradientColors(gps.IronBow, width);

    addHeader("RainBow (own)");
    drawRainBow();
    drawGradientColors(gps.RainBow, width);
    
    addHeader("arctic");
    printRGBdata(colorMap_arctic, {scale:2,doubles:20});
    drawGradientColors(gps.arctic, colorMap_arctic.length/3);

    addHeader("blackHot");
    printRGBdata(colorMap_blackHot, {scale:2,doubles:20});
    drawGradientColors(gps.blackHot, colorMap_blackHot.length/3);

    addHeader("blueRed");
    printRGBdata(colorMap_blueRed, {scale:2,doubles:20});
    drawGradientColors(gps.blueRed, colorMap_blueRed.length/3);

    addHeader("coldest");
    printRGBdata(colorMap_coldest, {scale:2,doubles:20});
    drawGradientColors(gps.coldest, colorMap_coldest.length/3);

    addHeader("contrast");
    printRGBdata(colorMap_contrast, {scale:2,doubles:20});
    drawGradientColors(gps.contrast, colorMap_contrast.length/3);

    addHeader("doubleRainbow");
    printRGBdata(colorMap_doubleRainbow, {scale:2,doubles:20});
    drawGradientColors(gps.doubleRainbow, colorMap_doubleRainbow.length/3);

    addHeader("glowBow");
    printRGBdata(colorMap_glowBow, {scale:2,doubles:20});
    drawGradientColors(gps.glowBow, colorMap_glowBow.length/3);

    addHeader("grayRed");
    printRGBdata(colorMap_grayRed, {scale:2,doubles:20});
    drawGradientColors(gps.grayRed, colorMap_grayRed.length/3);

    addHeader("grayscale");
    printRGBdata(colorMap_grayscale, {scale:2,doubles:20});
    drawGradientColors(gps.grayscale, colorMap_grayscale.length/3);

    addHeader("hottest");
    printRGBdata(colorMap_hottest, {scale:2,doubles:20});
    drawGradientColors(gps.hottest, colorMap_hottest.length/3);

    addHeader("lava");
    printRGBdata(colorMap_lava, {scale:2,doubles:20});
    drawGradientColors(gps.lava, colorMap_lava.length/3);

    addHeader("medical");
    printRGBdata(colorMap_medical, {scale:2,doubles:20});
    drawGradientColors(gps.medical, colorMap_medical.length/3);

    addHeader("rainbow");
    printRGBdata(colorMap_rainbow, {scale:2,doubles:20});
    drawGradientColors(gps.rainbow, colorMap_rainbow.length/3);

    addHeader("wheel1");
    printRGBdata(colorMap_wheel1, {scale:2,doubles:20});
    drawGradientColors(gps.wheel1, colorMap_wheel1.length/3);

    addHeader("wheel2");
    printRGBdata(colorMap_wheel2, {scale:2,doubles:20});
    drawGradientColors(gps.wheel2, colorMap_wheel2.length/3);

    addHeader("wheel3");
    printRGBdata(colorMap_wheel3, {scale:2,doubles:20});
    drawGradientColors(gps.wheel3, colorMap_wheel3.length/3);

    addHeader("whiteHot");
    printRGBdata(colorMap_whiteHot, {scale:2,doubles:20});
    drawGradientColors(gps.whiteHot, colorMap_whiteHot.length/3);

    addHeader("yellow");
    printRGBdata(colorMap_yellow, {scale:2,doubles:20});
    drawGradientColors(gps.yellow, colorMap_yellow.length/3);
    
}

function printAll_ReferenceColorSchemes()
{
    var options = {scale:1,doubles:20};
    for (var i = 0; i < color_schemes.length; i++) {
        
        addHeader(color_schemes[i].name + "&nbsp;&nbsp;&nbsp;&nbsp;width:" + color_schemes[i].map.length/3);
        printRGBdata(color_schemes[i].map, options);
    }
}

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