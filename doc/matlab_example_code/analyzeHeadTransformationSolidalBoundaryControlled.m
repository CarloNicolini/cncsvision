function analyzeHeadTransformationSolidalBoundaryControlled(data,anglesData,passiveMatrixData,IOD,focalDistance,Translation,TranslationConstant,Slant,nTrial)
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

	[junk, eyeCalibration] = getEyes(glassCoordsT2(1:3)',glassCoordsT2(4:6)',IOD);
	%eyeCalibration
	dataHeadCalibrationDone3=data(find(data(:,2)==3),:);
	
	trialData=dataHeadCalibrationDone3(find(dataHeadCalibrationDone3(:,1)==nTrial),:);
	trialData = trialData(find(trialData(:,3)==1),:);
	angleTrialData  = anglesData(find(anglesData(:,1)==nTrial),6);
	passiveMatrixTrialData  = passiveMatrixData(find(passiveMatrixData(:,1)==nTrial),:);
	eyesCoords  		= nan(size(trialData,1),1);
	glassLeftCoords 	= nan(size(trialData,1),1);
	glassRightCoords	= nan(size(trialData,1),1);
	projPointEyeRightCoords = nan(size(trialData,1),1);
	yaws			= nan(size(trialData,1),1);
	pitchs			= nan(size(trialData,1),1);
	rolls			= nan(size(trialData,1),1);
	stimulusCenters 	= nan(size(trialData,1),1);
	instantPlaneSlants	= nan(size(trialData,1),1);

	objPassiveTransXs 	= nan(size(trialData,1),1);
	originalPassiveTrans = nan(size(trialData,1),1);
	projectedPoints  = nan(size(trialData,1),2);		
	for row=1:size(trialData,1)
		markers123T = reshape(trialData(row,4:12),[3 3]);
		H=getHeadTransformation(markers123T0, reshape(trialData(row,4:12),[3 3]));
		
		% Load the data coming from the saved passive matrix
		originalPassiveMatrix = eye(4);
		originalPassiveMatrix(1:3,1:4) = reshape(passiveMatrixTrialData(row,3:14),[3 4]);

		% Compute yaw pitch and roll of head transformation
		[yaw,pitch,roll] = eulerAngles(H(1:3,1:3),1,0,2);

		% Compute glass coordinates w.r.t latest known glass coordinates as 3x1 vectors
		glassLeftT = affine3D(H,glassCoordsT2(1:3)');
		glassRightT = affine3D(H,glassCoordsT2(4:6)');
			
		% Compute eye left and eye right coordinates as 3x1 vectors using a simple formula
		[ eyeLeft, eyeRight ] = getEyes(glassLeftT,glassRightT,IOD);
		% If not specified differently, cameraCenter is always eyeRight
		cameraCenter=eyeRight;
		translationFactor = zeros(3,1); % translation factor as 3x1 vector
				
		posAlongLineOfSight = (H(1:3,1:3)*(eyeRight-eyeCalibration));
		projPointEyeRight = norm(H(1:3,1:3)*[0;0;focalDistance]-eyeRight)*(H(1:3,1:3)*[0;0;-1])+eyeRight;
		
		switch (Translation)
		case -2
			cameraCenter = [0;0;eyeRight(3)];
			translationFactor = zeros(3,1);
			%projPointEyeRight=[0;0;focalDistance];
		case 0
			cameraCenter = eyeRight;
			translationFactor = zeros(3,1);
		case 1
			translationFactor = TranslationConstant*[posAlongLineOfSight(3);0;0];
			cameraCenter = eyeRight+translationFactor;			
		case 2
			translationFactor = TranslationConstant*[0;posAlongLineOfSight(3);0];
			cameraCenter = eyeRight+translationFactor;
		end
		
		instantPlaneSlant = acos( cos(Slant*pi/180)*(focalDistance-posAlongLineOfSight(3) )/(focalDistance));
		stimulusCenter = (projPointEyeRight+translationFactor);
							
		% Compute ModelViewProjectionMatrix for both active and passive observer
		MVPactive=getProjectionMatrix(screen,cameraCenter, eye(4));
		[MVPpassive,ModelViewMatrix]=getProjectionMatrix(screen,cameraCenter,H);
			
		% Compute the object active transformation
		objectActiveMatrix = eye(4);
		
		objectActiveMatrix(1:3,4) = stimulusCenter;
		if (Translation==-2 || Translation==-1 )
			objectActiveMatrix(1:3,1:3)=eye(3);
		else
			objectActiveMatrix(1:3,1:3)= getRotationMatrix(yaw,[0,1,0])*getRotationMatrix(pitch,[1,0,0]);
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
		projPointEyeRightCoords(row) = projPointEyeRight(1);
		instantPlaneSlants(row) = instantPlaneSlant;

		objPassiveTransXs(row) = objPassiveMatrix(2,4);
		originalPassiveTrans(row) = originalPassiveMatrix(2,4);
		
		projectedPoints(row,:) = project(MVPpassive,[0,0,screen.width,screen.height],stimulusCenter);
	end
	plot(1:length(objPassiveTransXs),objPassiveTransXs,'r-.',1:length(originalPassiveTrans),originalPassiveTrans,'b.-');
	%plot(projectedPoints(:,1),projectedPoints(:,2),'-o'); axis([0 screen.pixelWidth 0 screen.pixelHeight]);
	%plot(1:length(projPointEyeRightCoords),projPointEyeRightCoords,'r.-',1:length(objPassiveTransXs),objPassiveTransXs,'b.-');

