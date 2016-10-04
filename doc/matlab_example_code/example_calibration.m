% Identify 4 markers as 4 3d-column vectors

m0_t0 = [0 0 0]'; % marker m0 (finger tip for example)
m1_t0 = [1 0 0]'; % reference marker m1
m2_t0 = [0 1 0]'; % reference marker m2
m3_t0 = [0 0 1]'; % reference marker m3

% simulate a translation right + 50 mm

Tx = [50 0 0 ]';
m1_t = m1_t0 + Tx;
m2_t = m2_t0 + Tx;
m3_t = m3_t0 + Tx;

% Reconstruct the transformation matrix after this translation (must just contain a translation part, as a 4° column)

% The function getHeadTransformation has as input two matrices, of size n x 3
% where n is the number of reference markers. The first matrix X_0 contains in each column the reference markers at time t0, the second matrix X_t contains the reference markers at time t. It then estimates the 4x4 affine transformation such that ||A*X_0 - X_t||^2 is minimized, thanks to the Umeyama algorithm implemented in the function ralign.m
H = getHeadTransformation([m1_t0,m2_t0,m3_t0],[m1_t,m2_t,m3_t]);

% How H is an affine 4x4 matrix. To apply this affine trasformation to a 3d column vector, we use the function affine3D

% Now get the position of marker m0 after translation Tx
m0_t = affine3D(H,m0_t0);

% Let's try with rotations, let's define a rotation matrix using the function getRotationMatrix. This function has as inputs an axis as a 3d vector and an angle. To create a rotation around y axis of 30° we can do as follows:

R = getRotationMatrix(pi/6,[0 1 0]);

% Now let's apply this transformation to our reference markers m1,m2,m3
m1_t = R*m1_t;
m2_t = R*m2_t;
m3_t = R*m3_t;

% and again let's try to infer the rigid-body transformation, that now will be a part of rotation and a part of translation
H = getHeadTransformation([m1_t0,m2_t0,m3_t0],[m1_t,m2_t,m3_t]);

% now the m0_t marker will be at the following coordinate:

m0_t = affine3D(H,m0_t0)


% P.S. if some of the reference markers is not visibile, its coordinates will be set to something like 1E+20, and the function getHeadTransformation returns an identity matrix.