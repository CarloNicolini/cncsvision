function [avgDeltaT,stdDeltaT] = computeTimeLag(filename)

% Load the data, first column observation number, second column delta theta in deg, third column pixel size in deg
A=load(filename);

T = 6; %period of rotation is 6 seconds

omega = 2*pi/T;

deltaTheta = A(:,2)*pi/180.0; % delta theta in radians

% Compute the DeltaT as "DeltaT = deltaTheta /omega"
deltaT = deltaTheta/omega;

avgDeltaT = mean(deltaT);
stdDeltaT = std(deltaT);

fprintf('Average time lag is %2.3f +- %2.3f [ms]\n', avgDeltaT*1000, stdDeltaT*1000 );