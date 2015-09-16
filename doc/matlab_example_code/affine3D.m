function y=affine3D(H,x)
if ( sum( size(x) == [1,1] ) )
	y=H(1:3,1:3)*x+H(1:3,4);
else
	error('Please put column vector as ordinary vectors')
end

