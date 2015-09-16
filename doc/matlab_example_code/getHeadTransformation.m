function H = getHeadTransformation(markersT0, markersT)

if ( abs( sum(markersT(:)) ) > 1E9  )
	c=1;
	R=eye(3);
	t=[0 0 0]';
else
	[c,R,t] = ralign(markersT0,markersT);
	H=eye(4);
	H(1:3,1:3)=R;
	H(1:3,4)=t;
end
