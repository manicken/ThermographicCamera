class CRGB
{
	constructor(red,green,blue)
    {
    	this.r = red;
        this.g = green;
        this.b = blue;
    }
    /*constructor (crgb)
    {
    	this.r = crgb.r;
        this.g = crgb.g;
        this.b = crgb.b;
    }*/
}

function fill_gradient_RGB( colorMap,
                   startpos, startcolor,
                   endpos, endcolor )
{
    // if the points are in the wrong order, straighten them
    /*if( endpos < startpos ) {
        var t = endpos;
        CRGB tc = endcolor;
        endcolor = startcolor;
        endpos = startpos;
        startpos = t;
        startcolor = tc;
    }*/

    var rdistance87;
    var gdistance87;
    var bdistance87;

    rdistance87 = (endcolor.r - startcolor.r) << 7;
    gdistance87 = (endcolor.g - startcolor.g) << 7;
    bdistance87 = (endcolor.b - startcolor.b) << 7;

    var pixeldistance = endpos - startpos;
    var divisor = pixeldistance ? pixeldistance : 1;

    var rdelta87 = parseInt(rdistance87 / divisor); 
    var gdelta87 = parseInt(gdistance87 / divisor);
    var bdelta87 = parseInt(bdistance87 / divisor);

    rdelta87 *= 2;
    gdelta87 *= 2;
    bdelta87 *= 2;

    var r88 = startcolor.r << 8;
    var g88 = startcolor.g << 8;
    var b88 = startcolor.b << 8;
    for( var i = startpos; i <= endpos; ++i) {
        colorMap[i] = new CRGB( r88 >> 8, g88 >> 8, b88 >> 8);
        r88 += rdelta87;
        g88 += gdelta87;
        b88 += bdelta87;
    }
}
var colorMap = [];
fill_gradient_RGB(colorMap, 0, new CRGB(0,0,0), 15, new CRGB(255,255,255) )
console.log(colorMap);

function createAndAddGradient(count, color1, color2) {
    var height = 40;
    count *= 4;
    var ele = document.createElement("canvas");

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
    var mult = 24; // calculated from 240/(1+2.5+3.5+1.5+1.5)=240/10=24
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
function main() {
    var newData = getRGBdata(0,0,0,0,0,10,11);
    console.log(newData);
    
    printRGBdata(colorMap_arctic);
    printRGBdata(colorMap_glowBow);
    drawIronBow();
    drawRainBow();
    printRGBdata(colorMap_wheel1);
}
document.addEventListener("DOMContentLoaded", main());
