function scaleImageData(c, imageData, scale) {
    var scaled = c.createImageData(imageData.width * scale, imageData.height * scale);
  
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

  // my obsolete version of  fill_gradient_RGB

  function getRGBdata(fromR, fromG, fromB, toR, toG, toB, steps) {
    var rgbData = new Uint8ClampedArray(steps * 3)
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
    	rgbData[i*3+0] = (fromR + i * Rstep);
        rgbData[i*3+1] = (fromG + i * Gstep);
        rgbData[i*3+2] = (fromB + i * Bstep);
    }
    return rgbData;
}