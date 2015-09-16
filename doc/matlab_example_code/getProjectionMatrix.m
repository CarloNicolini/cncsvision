function [ProjectionMatrix,ModelViewMatrix] = getProjectionMatrix(screen,cop,HeadTransformation)   

[pa,pb,pc]=getScreenBorders(screen,HeadTransformation);

vr=(pb-pa)/norm(pb-pa);
vu=(pc-pa)/norm(pc-pa);
vn=cross(vr,vu)/norm(cross(vr,vu));

va=pa-cop;
vb=pb-cop;
vc=pc-cop;

d=-dot(vn,va);

n=100;
f=1800;

l=(dot(vr,va))*n/d;
r=dot(vr,vb)*n/d;
b=dot(vu,va)*n/d;
t=dot(vu,vc)*n/d;

P=eye(4,4);

P(1,1)=  2*n/(r-l);
P(1,3)=  (r+l)/(r-l);
P(2,2)=  2*n/(t-b);
P(2,3)=  (t+b)/(t-b);
P(3,3)=  -(f+n)/(f-n);
P(3,4)=  -2*f*n/(f-n);
P(4,3)=  -1;

M=eye(4,4);
T=eye(4,4);

M(1:3,1)=vr;
M(1:3,2)=vu;
M(1:3,3)=vn;

T(1:3,4)=-cop;

ProjectionMatrix=P*M'*T;

ModelViewMatrix=M'*T;
