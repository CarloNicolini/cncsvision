function [pa,pb,pc]=getScreenBorders(screen,HeadTransformation)

w=screen.width;
h=screen.height;
fz=screen.focalDistance;

pa=[-w/2;-h/2;fz];
pb=[w/2;-h/2;fz];
pc=[-w/2;h/2;fz];

pa = affine3D(HeadTransformation,pa);
pb = affine3D(HeadTransformation,pb);
pc = affine3D(HeadTransformation,pc);
