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
  module::Perlin myModule;

  module::Clamp myModuleCl;
  myModuleCl.SetSourceModule(0,myModule);

//  module::Abs myModuleAbs;
//  myModuleAbs.SetSourceModule(0,myModuleCl);

  utils::NoiseMap heightMap;
  utils::NoiseMapBuilderPlane heightMapBuilder;
  heightMapBuilder.SetSourceModule (myModuleCl);
  heightMapBuilder.SetDestNoiseMap (heightMap);
  heightMapBuilder.SetDestSize (512, 512);
  heightMapBuilder.SetBounds (2.0, 30.0, 2.0, 30.0);
  heightMapBuilder.Build ();

//  std::cout << heightMapBuilder << std::endl;

  utils::RendererImage renderer;
  utils::Image image;
  renderer.SetSourceNoiseMap (heightMap);
  renderer.SetDestImage (image);

/*  renderer.ClearGradient ();
  renderer.AddGradientPoint (-1.0000, utils::Color (  0,   0, 0, 255)); // deeps
//  renderer.AddGradientPoint (0.0000, utils::Color (  127,   127, 0, 255)); // deeps
  renderer.AddGradientPoint (1.0000, utils::Color (  255,   0, 0, 255)); // shallow
 */
 renderer.Render();

  utils::WriterBMP writer;
  writer.SetSourceImage (image);
  writer.SetDestFilename ("tutorial.bmp");
  writer.WriteDestFile ();

  return 0;
}
