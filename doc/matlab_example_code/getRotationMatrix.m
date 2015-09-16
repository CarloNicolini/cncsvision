function R=getRotationMatrix(angle,axis)


R=eye(3);
sinaxis=sin(angle)*axis;
c = cos(angle);

cos1axis=(1-c)*axis;

tmp=cos1axis(1)*axis(2);

R(1,2)=tmp-sinaxis(3);
R(2,1)=tmp+sinaxis(3);

tmp = cos1axis(1)*axis(3);
R(1,3)=tmp+sinaxis(2);
R(3,1)=tmp-sinaxis(2);

tmp=cos1axis(2)*axis(3);
R(2,3)=tmp-sinaxis(1);
R(3,2)=tmp+sinaxis(1);


R(logical(eye(size(R))))=cos1axis.*axis+c;

