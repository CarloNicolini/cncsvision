% Returns euler angles in radians given a wanted convention, implementation from GPU Gems IV and 
% from Eigen libraries

function [x,y,z] = eulerAngles(R,a0,a1,a2)

	res=zeros(1,3);
	odd=-1;
	if ( mod(a0+1,3)==a1 )
		odd=0;
	else
		odd=1;
	end
 
  i = a0;
  j = mod((a0 + 1 + odd),3);
  k = mod((a0 + 2 - odd),3);

	i=i+1;
	j=j+1;
	k=k+1;
	
  if (a0==a2)
    s = norm([ R(j,i),R(k,j) ]);
    y = atan2(s, R(i,i));
    if (s > eps)
      x = atan2(R(j,i), R(k,i));
      z = atan2(R(i,j),-R(i,k));
    else
      x = 0.0;
      if (R(i,i)>0)
      	z=atan2(-R(k,j), R(j,j));
      else
      	z=-atan2(-R(k,j), R(j,j));
      end
     end
  else
    c = norm([ R(i,i),R(i,j)] );
    y = atan2(-R(i,k), c);
    if (c > eps)
      x = atan2(R(j,k), R(k,k));
      z = atan2(R(i,j), R(i,i));
    else
      x = 0.0;
      if (R(i,k)>0)
      	z=atan2(-R(k,j), R(j,j));
      else
      	z=-atan2(-R(k,j), R(j,j));
      end
    end
  end
  
  res=[x,y,z];
  if (~odd)
    res = -res;
  end

   x=res(1);
   y=res(2);
   z=res(3);
