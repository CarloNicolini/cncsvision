function analyzeHeadTransformationRotationYawOptimized(data,anglesData,IOD,focalDistance,rotation,followingSpeed,nTrial)
	screen.width=310; % screen width in mm
	screen.focalDistance=focalDistance; % focal distance in mm

	screen.pixelWidth=1024; % pixel width
	screen.pixelHeight=768; % pixel height

	screen.height=screen.width*screen.pixelHeight/screen.pixelWidth;

	% The order is the following
	% trialNumber headCalibrationDone trialMode markers[1] markers[2] markers[3] markers[17] markers[18]

	% We must find the interpolated glass coordinates at headCalibrationDone==1
	rows0 = data(find(data(:,2)==0),:);
	rows1 = data(find(data(:,2)==1),:);
	rows2 = data(find(data(:,2)==2),:);
	
	lastRowHeadCalibrationDone0= rows0(end,:);
	lastRowHeadCalibrationDone1= rows1(end,:);
	lastRowHeadCalibrationDone2= rows2(end,:);
	
	clear rows0;
	clear rows1;
	clear rows2;
	
	% These are the two 3D coordinates of the glasses
	glassCoordsT0 = lastRowHeadCalibrationDone0(13:18);
	markers123T0 = reshape(lastRowHeadCalibrationDone2(4:12),[3 3]);

	Ht0t1 = getHeadTransformation(reshape( lastRowHeadCalibrationDone0(4:12),[3 3]),reshape( lastRowHeadCalibrationDone1(4:12),[3 3]));
	
	glassCoordsT1 = [ affine3D(Ht0t1,glassCoordsT0(1:3)'),affine3D(Ht0t1,glassCoordsT0(4:6)')];

	Ht1t2 = getHeadTransformation(reshape( lastRowHeadCalibrationDone1(4:12),[3 3]),reshape( lastRowHeadCalibrationDone2(4:12),[3 3]));
	glassCoordsT2 = [ affine3D(Ht1t2,glassCoordsT1(1:3)'),affine3D(Ht1t2,glassCoordsT1(4:6)')];

	dataHeadCalibrationDone3=data(find(data(:,2)==3),:);
	
	trialData=dataHeadCalibrationDone3(find(dataHeadCalibrationDone3(:,1)==nTrial),:);
	trialData = trialData(find(trialData(:,3)==1),:);
	angleTrialData  = anglesData(find(anglesData(:,1)==1),6);
		
	eyesCoords  	= nan(size(trialData,1),1);
	glassLeftCoords = nan(size(trialData,1),1);
	glassRightCoords= nan(size(trialData,1),1);
	projPointCoords = nan(size(trialData,1),1);
	yaws		= nan(size(trialData,1),1);
	pitchs		= nan(size(trialData,1),1);
	rolls		= nan(size(trialData,1),1);
	stimulusCenters = nan(size(trialData,1),1);
	planeSlants	= nan(size(trialData,1),1);
	omegaYs		= nan(size(trialData,1),1);
	objPassiveTransXs = nan(size(trialData,1),1);
	
	projectedPoints  = nan(size(trialData,1),2);		
	for row=1:size(trialData,1)
		markers123T = reshape(trialData(row,4:12),[3 3]);
		H=getHeadTransformation(markers123T0, reshape(trialData(row,4:12),[3 3]));
		
		% Compute glass coordinates w.r.t latest known glass coordinates
		glassLeftT = affine3D(H,glassCoordsT2(1:3)');
		glassRightT = affine3D(H,glassCoordsT2(4:6)');
			
		% Compute eye left and eye right coordinates using a simple formula
		eyeLeft =  ( glassLeftT  + glassRightT )/2 - IOD/2*( glassRightT - glassLeftT )/norm( glassRightT - glassLeftT );
		eyeRight = ( glassLeftT  + glassRightT )/2 + IOD/2*( glassRightT - glassLeftT )/norm( glassRightT - glassLeftT );
	
		% Compute yaw pitch and roll of head transformation
		[yaw,pitch,roll] = eulerAngles(H(1:3,1:3),1,0,2);
					
		% Simulate a stimulus with [0,0,focalDistance]
		stimulusCenter = [0;0;0];
		if (rotation==0)
			stimulusCenter = (H(1:3,1:3)*[ eyeRight(1)*followingSpeed;eyeRight(2);eyeRight(3)+focalDistance]);
		end
		if (rotation==2)
			stimulusCenter = ((getRotationMatrix(yaw*followingSpeed,[0,1,0])*getRotationMatrix(pitch,[1,0,0]))*[0;0;focalDistance] + H(1:3,4));
        end
        
		projPoint = norm(affine3D(H,stimulusCenter)-eyeRight)*(H(1:3,1:3)*[0;0;-1])+eyeRight;
		
		planeSlant =  atan2(eyeRight(1),abs(focalDistance+eyeRight(3)));
		omegaY	   =  atan2( (projPoint(1)-eyeRight(1)),abs(projPoint(3))); % this is the projPointAngleX variable
		
		% Compute ModelViewProjectionMatrix for both active and passive observer
		MVPactive=getProjectionMatrix(screen,eyeRight,eye(4));
		[MVPpassive,ModelViewMatrix]=getProjectionMatrix(screen,eyeRight,H);
			
		% Compute the object active transformation
		objectActiveMatrix = eye(4);
		if (rotation==0)
			objectActiveMatrix(1:3,1:3) = getRotationMatrix(yaw,[0,1,0])*getRotationMatrix(pitch,[1,0,0]);
			objectActiveMatrix(1:3,4)=stimulusCenter;
		end
		if (rotation==2)
			objectActiveMatrix(1:3,1:3) = getRotationMatrix(yaw*followingSpeed,[0,1,0])*getRotationMatrix(pitch,[1,0,0]);
			objectActiveMatrix(1:3,4)=stimulusCenter;
		end
		
		% Compute the object passive transformation
		objPassiveMatrix = ModelViewMatrix*objectActiveMatrix;
		
		%%%%%%% PLOTTING THINGS %%%%%%%%%%%%%%%
		% Push back glass coordinates
		glassLeftCoords(row) = glassLeftT(1);
		glassRightCoords(row)= glassRightT(1);
		% Push back eyes coordinates
		eyesCoords(row) = eyeRight(1);
			
		yaws(row)=yaw;
		pitchs(row) = pitch;
		rolls(row)=roll;
		stimulusCenters(row) = stimulusCenter(1);
		projPointCoords(row) = projPoint(1);
		planeSlants(row) = planeSlant;
		omegaYs(row) = omegaY;
		objPassiveTransXs(row) = objPassiveMatrix(1,4);
		
		projectedPoints(row,:) = project(MVPpassive,[0,0,screen.width,screen.height],stimulusCenter);
	end
	%plot(projectedPoints(:,1),projectedPoints(:,2),'-o'); axis([0 screen.pixelWidth 0 screen.pixelHeight]);
	plot(1:length(projPointCoords),projPointCoords,'r.-',1:length(objPassiveTransXs),objPassiveTransXs,'b.-');

