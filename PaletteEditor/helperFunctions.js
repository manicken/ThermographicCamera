


function scaleImageData(c, imageData, scale) {
    var scaled = c.createImageData(parseInt(imageData.width * scale), parseInt(imageData.height * scale));
  
    for(var row = 0; row < imageData.height; row++) {
      for(var col = 0; col < imageData.width; col++) {
        var sourcePixel = [
          imageData.data[(row * imageData.width + col) * 4 + 0],
          imageData.data[(row * imageData.width + col) * 4 + 1],
          imageData.data[(row * imageData.width + col) * 4 + 2],
          imageData.data[(row * imageData.width + col) * 4 + 3]
        ];
        for(var y = 0; y < scale; y++) {
          var destRow = row * scale + y;
          for(var x = 0; x < scale; x++) {
            var destCol = col * scale + x;
            for(var i = 0; i < 4; i++) {
              scaled.data[(destRow * scaled.width + destCol) * 4 + i] =
                sourcePixel[i];
            }
          }
        }
      }
    }
  
    return scaled;
  }

function fill_gradient_RGB( colorMap,
    startpos, startcolor,
    endpos, endcolor )
{
    console.log("startpos:" + startpos + ", endpos:" + endpos + ", startColor:", startcolor, ", endcolor:", endcolor);
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

    //console.log(typeof rdistance87 + rdistance87);

    var pixeldistance = endpos - startpos;
    var divisor = pixeldistance ? pixeldistance : 1;

    var rdelta87 = parseInt(rdistance87 / divisor); 
    var gdelta87 = parseInt(gdistance87 / divisor);
    var bdelta87 = parseInt(bdistance87 / divisor);

    rdelta87 *= 2;
    gdelta87 *= 2;
    bdelta87 *= 2;

    //console.log("rdelta87:" + rdelta87 + ", gdelta87:" + gdelta87 + ", bdelta87:" + bdelta87);

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
/**
 * lower resulution version
 * @param {*} colorMap 
 * @param {*} startpos 
 * @param {*} startcolor 
 * @param {*} endpos 
 * @param {*} endcolor 
 */
function fill_gradient_RGB_lowres( colorMap,
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

    rdistance87 = (endcolor.r - startcolor.r);
    gdistance87 = (endcolor.g - startcolor.g);
    bdistance87 = (endcolor.b - startcolor.b);

    //console.log(typeof rdistance87 + rdistance87);

    var pixeldistance = endpos - startpos;
    var divisor = pixeldistance ? pixeldistance : 1;

    var rdelta87 = parseInt(rdistance87 / divisor); 
    var gdelta87 = parseInt(gdistance87 / divisor);
    var bdelta87 = parseInt(bdistance87 / divisor);

    //rdelta87 *= 2;
    //gdelta87 *= 2;
    //bdelta87 *= 2;

    //console.log("rdelta87:" + rdelta87 + ", gdelta87:" + gdelta87 + ", bdelta87:" + bdelta87);

    var r88 = startcolor.r;
    var g88 = startcolor.g;
    var b88 = startcolor.b;
    for( var i = startpos; i <= endpos; ++i) {
        colorMap[i] = new CRGB( r88 , g88, b88);
        r88 += rdelta87;
        g88 += gdelta87;
        b88 += bdelta87;
    }
}

  // my obsolete version of  fill_gradient_RGB

  function getRGBdata(rgbData, fromR, fromG, fromB, toR, toG, toB, steps) {
    //var rgbData = new Uint8ClampedArray(steps * 3)
    var Rrange = (toR-fromR);
    var Grange = (toG-fromG);
    var Brange = (toB-fromB);
    if (Rrange != 0) Rrange += 1;
    if (Grange != 0) Grange += 1;
    if (Brange != 0) Brange += 1;
    var Rstep = Rrange/steps;
    var Gstep = Grange/steps;
    var Bstep = Brange/steps;
    console.log("Rstep:" + Rstep + ", Gstep:" + Gstep + ", Bstep:" + Bstep);
    for (var i=0;i<steps;i++) {
        rgbData[i] = new CRGB( (fromR + i * Rstep), (fromG + i * Gstep), (fromB + i * Bstep));
    }
    return rgbData;
}