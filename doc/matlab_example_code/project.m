function p = project(ModelViewProjectionMatrix,Viewport,X)

% X is the world position vector
ndc = ModelViewProjectionMatrix*[X;1];
ndc = ndc/ndc(4);


offsetX=0;
offsetY=0;

p = [ (ndc(1)+1)*(Viewport(3))+offsetX, (ndc(2)+1)*(Viewport(4))+offsetY ];

