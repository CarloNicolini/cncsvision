void voronoi_grid();
void voronoi_patches();
void voronoi_test();

void voronoi_grid()
{
	// load voronoi module
	module::Voronoi myModule;
	myModule.SetSeed(unifRand(25.0, 75.0));
	myModule.SetDisplacement(1.0);
	myModule.EnableDistance(false);
	myModule.SetFrequency(1.0);

	// build heightmap
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule (myModule);
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (textureWidth, textureWidth);
	heightMapBuilder.SetBounds (1.0, 10.0, 1.0, 10.0);
	heightMapBuilder.Build ();

	// create the texture (feed from height map row-by-cols)
	int i, j;
	float c;
	for (i = 0; i < textureHeight; i++) // in every row of the height map
	{
		// extract all texture values of the i-th row of the height map
		const float* colValues = heightMap.GetConstSlabPtr(i);
	
		for (j = 0; j < textureWidth; j++) // in every column of the height map
		{
			if(*(colValues+1) != *colValues || *(colValues-1) != *colValues)
				c = 255.0;
			else
				c = 0.0;

			// feed the texture with RGBA colors
			textureImage[i][j][0] = (GLubyte) c;
			textureImage[i][j][1] = (GLubyte) 0;
			textureImage[i][j][2] = (GLubyte) 0;
			textureImage[i][j][3] = (GLubyte) 255;
			
			// move to the next value on this column
			++colValues;
		}
	}
}

void voronoi_patches()
{
	// load voronoi module
	module::Voronoi myModule;
	myModule.SetSeed(unifRand(25.0, 75.0));
	myModule.SetDisplacement(1.0);
	myModule.EnableDistance(false);
	myModule.SetFrequency(1.0);

	// build heightmap
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule (myModule);
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (textureWidth, textureWidth);
	heightMapBuilder.SetBounds (1.0, 10.0, 1.0, 10.0);
	heightMapBuilder.Build ();

	// create the texture (feed from height map row-by-cols)
	int i, j;
	float c;
	for (i = 0; i < textureHeight; i++) // in every row of the height map
	{
		// extract all texture values of the i-th row of the height map
		const float* colValues = heightMap.GetConstSlabPtr(i);
	
		for (j = 0; j < textureWidth; j++) // in every column of the height map
		{
			c = (*colValues - 2.0);
			c = c/4.0 * 255.0;

			// feed the texture with RGBA colors
			textureImage[i][j][0] = (GLubyte) c;
			textureImage[i][j][1] = (GLubyte) 0;
			textureImage[i][j][2] = (GLubyte) 0;
			textureImage[i][j][3] = (GLubyte) 255;
			
			// move to the next value on this column
			++colValues;
		}
	}
}

void checkerboard_messy()
{
	// load voronoi module
	module::Checkerboard myModule;

	module::Turbulence turbo;
	turbo.SetSourceModule(0, myModule);
	turbo.SetSeed(unifRand(25.0, 75.0));
	turbo.SetRoughness(1);
	turbo.SetFrequency(.5);
	turbo.SetPower(2);
	

	// build heightmap
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule (myModule);
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (textureWidth, textureWidth);
	heightMapBuilder.SetBounds (1.0, 10.0, 1.0, 10.0);
	heightMapBuilder.Build ();

	// create the texture (feed from height map row-by-cols)
	int i, j;
	float c;
	for (i = 0; i < textureHeight; i++) // in every row of the height map
	{
		// extract all texture values of the i-th row of the height map
		const float* colValues = heightMap.GetConstSlabPtr(i);
	
		for (j = 0; j < textureWidth; j++) // in every column of the height map
		{
			c = (*colValues);
			c = c/4.0 * 255.0;

			// feed the texture with RGBA colors
			textureImage[i][j][0] = (GLubyte) c;
			textureImage[i][j][1] = (GLubyte) 0;
			textureImage[i][j][2] = (GLubyte) 0;
			textureImage[i][j][3] = (GLubyte) 255;
			
			// move to the next value on this column
			++colValues;
		}
	}
}

