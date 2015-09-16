clear all;
%close all;
clc;
screen.focalDistance= -418.5;
screen.width = 310;
screen.pixelWidth=1024.0;
screen.pixelHeight=768.0;
screen.height=screen.width*screen.pixelHeight/screen.pixelWidth;

% Edge size of the square stimulus
w = 100;
% Center of the stimulus
cx = 0;
cy = 0;
corners = w*rand(10000,3)-(w/2);
corners(:,3)=zeros(10000,1);

corners(:,1:2) = corners(:,1:2)+[ones(10000,1)*cx,ones(10000,1)*cy]; 
for i=1:size(corners,1)
	corners(i,:) = ( getRotationMatrix(pi/3,[0,1,0])*corners(i,:)')' + [0,0,screen.focalDistance];
end

viewport=[0,0,screen.pixelWidth,screen.pixelHeight];

cop=[0,0,0]';
H=eye(4);
H(1:3,1:3)= getRotationMatrix(pi/6,[0,1,0]);
H(1:3,:) = [0;0;-100];
[P,M,T]=getProjectionMatrix(screen,cop,H);
M'*T
H
projections = zeros(4,3);
I=zeros(screen.pixelWidth,screen.pixelHeight);
for i=1:size(corners,1)
	p=project(P,viewport,corners(i,:)')';
	projections(i,:)=p';
	ix = ceil(projections(i,1));
	iy = ceil(projections(i,2));
	if (ix>1 && ix<1024 && iy>1 && iy<768)
		I(ix,iy) = projections(i,3);
	end
end

colormap gray;
imagesc(I');