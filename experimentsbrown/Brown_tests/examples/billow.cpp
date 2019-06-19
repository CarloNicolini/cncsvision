// references:
// http://libnoise.sourceforge.net/
//	download libnoise headers and paste them into /usr/local/include/noise
// sudo apt-get install libnoise-dev

#include <iostream>
#include <noise/noise.h>
#include "noiseutils.h"

using namespace noise;

int main (int argc, char** argv)
{
  module::Voronoi myModule;
  myModule.EnableDistance(false);
  myModule.SetFrequency(2.0);
/*
  module::Curve myModuleT;
  myModuleT.SetSourceModule(0, myModule);
  myModuleT.AddControlPoint(-1.0, -1.0);
  myModuleT.AddControlPoint(-0.5, -1.0);
  myModuleT.AddControlPoint(0.95, -1.0);
  myModuleT.AddControlPoint(1.0, 2.0);
*/

  module::ScalePoint myModuleT;
  myModuleT.SetSourceModule(0,myModule);
  myModuleT.SetScale(-1.0);


  utils::NoiseMap heightMap;
  utils::NoiseMapBuilderPlane heightMapBuilder;
  heightMapBuilder.SetSourceModule (myModuleT);
  heightMapBuilder.SetDestNoiseMap (heightMap);
  heightMapBuilder.SetDestSize (512, 512);
//  heightMapBuilder.SetBounds (0.0, 100.0, 0.0, 100.0);
  heightMapBuilder.SetBounds (0.0, 10.0, 0.0, 10.0);
  heightMapBuilder.Build ();

//  std::cout << heightMapBuilder << std::endl;

  utils::RendererImage renderer;
  utils::Image image;
  renderer.SetSourceNoiseMap (heightMap);
  renderer.SetDestImage (image);

  renderer.ClearGradient ();
  renderer.AddGradientPoint (-1.0, utils::Color (  0,   0, 255, 255)); // deeps
  renderer.AddGradientPoint (0.95, utils::Color (  100,   0, 155, 255)); // deeps
  renderer.AddGradientPoint (1.0000, utils::Color (  255,   0, 0, 255)); // shallow

 renderer.Render();

  utils::WriterBMP writer;
  writer.SetSourceImage (image);
  writer.SetDestFilename ("../experimentsbrown/Brown_tests/examples/billow.bmp");
  writer.WriteDestFile ();

  return 0;
}
