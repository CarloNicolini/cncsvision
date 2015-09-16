// references:
// http://libnoise.sourceforge.net/
//	download libnoise headers and paste them into /usr/local/include/noise
// sudo apt-get install libnoise-dev

#include <iostream>
#include <stdio.h>
#include <noise/noise.h>
#include "noiseutils.h"

using namespace noise;

int main (int argc, char** argv)
{
  module::Perlin myModule;
  myModule.SetSeed(140.0);
  myModule.SetDisplacement(.5);
  myModule.EnableDistance(false);
  myModule.SetFrequency(4.0);

  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  int c = 0;

  do 
  {
	c = getchar();
	double value = myModule.GetValue(x,y,z);
	std::cout << "x = " << x << " y = " << y << " z = " << z << ". value: " << value << std::endl;
	if(c == 'q')
		x+=.5;
	if(c == 'w')
		y+=.5;
	if(c == 'e')
		z+=.5;
	if(c == 'a')
		x--;
	if(c == 's')
		y--;
	if(c == 'd')
		z--;
  } while (c != '.');

  utils::NoiseMap heightMap;
  utils::NoiseMapBuilderPlane heightMapBuilder;
  heightMapBuilder.SetSourceModule (myModule);
  heightMapBuilder.SetDestNoiseMap (heightMap);
  heightMapBuilder.SetDestSize (512, 512);
  heightMapBuilder.SetBounds (0.0, 1.0, 0.0, 1.0);
  heightMapBuilder.Build ();

//  std::cout << heightMapBuilder << std::endl;

  utils::RendererImage renderer;
  utils::Image image;
  renderer.SetSourceNoiseMap (heightMap);
  renderer.SetDestImage (image);

  renderer.ClearGradient ();
  renderer.AddGradientPoint (-1.0000, utils::Color (  0,   0, 0, 255)); // deeps
  renderer.AddGradientPoint (1.0000, utils::Color (  255,   255, 255, 255)); // shallow
  renderer.Render();

  utils::WriterBMP writer;
  writer.SetSourceImage (image);
  writer.SetDestFilename ("tutorial.bmp");
  writer.WriteDestFile ();

  return 0;
}
