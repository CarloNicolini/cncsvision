function [eyeLeft,eyeRight] = getEyes(glassLeftT,glassRightT,IOD)
	eyeLeft =  ( glassLeftT  + glassRightT )/2 - IOD/2*( glassRightT - glassLeftT )/norm( glassRightT - glassLeftT );
	eyeRight = ( glassLeftT  + glassRightT )/2 + IOD/2*( glassRightT - glassLeftT )/norm( glassRightT - glassLeftT );
