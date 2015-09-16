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
	myModule.SetSeed(140.0);
	myModule.SetDisplacement(1.0);
	myModule.EnableDistance(true);
	myModule.SetFrequency(0.5);

	module::Clamp myModuleCl;
	myModuleCl.SetSourceModule(0,myModule);
	myModuleCl.SetBounds(-1.0,1.0);

	module::Curve myModuleCur;
	myModuleCur.SetSourceModule(0,myModuleCl);
	myModuleCur.AddControlPoint(-1.0,-1.0);
	myModuleCur.AddControlPoint(-0.5,1.0);
	myModuleCur.AddControlPoint(0.0,-1.0);
	myModuleCur.AddControlPoint(1.0,-1.0);

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule (myModuleCl);
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (512, 512);
	heightMapBuilder.SetBounds (1.0, 10.0, 1.0, 10.0);
	heightMapBuilder.Build ();

  std::cout << heightMap.GetValue(1,1) << std::endl;

  utils::RendererImage renderer;
  utils::Image image;
  renderer.SetSourceNoiseMap (heightMap);
  renderer.SetDestImage (image);

  renderer.ClearGradient ();
  renderer.AddGradientPoint (-1.0000, utils::Color (  0,   0, 0, 255)); // deeps
//  renderer.AddGradientPoint (0.0000, utils::Color (  127,   127, 0, 255)); // deeps
  renderer.AddGradientPoint (1.0000, utils::Color (  255,   0, 0, 255)); // shallow
  renderer.AddGradientPoint (2.0000, utils::Color (  255,   255, 255, 255)); // shallow

 renderer.Render();

  utils::WriterBMP writer;
  writer.SetSourceImage (image);
  writer.SetDestFilename ("voronoi.bmp");
  writer.WriteDestFile ();

  return 0;
}
