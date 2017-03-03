%% IPM

input_matrix = imread('/Users/yeabserak/Box Sync/398/roma/RouenN8IRC052310/IMG00223.jpg');
input_matrix = rgb2gray(input_matrix);
[rows, cols] = size(input_matrix);

%%% Setup Cord sys
uno_vec = ones(rows,cols); 

x_cord = (1:rows)'; %%look up https://www.mathworks.com/help/matlab/ref/ind2sub.html
x_cord = repmat(x_cord,1,cols); %%just 1,2,3,....rows copied by cols times
x = x_cord.* uno_vec;

y = x'; %%Cause transpose of x

M=[x(:) y(:) ones(rows*cols,1)]; %Flatten

%% Setup of Homogeneous Matrix

Rx = 2.5627222141;
Ry = 1.1357644644e-002;
Rz = 4.9923207876e-002;

tx = -2.2789169032e-2;
ty = 6.9994357911e-1;
tz = 1.0713151183;

trans = [tx, ty,tz];
trans = repmat(trans,size(M,1),1);

r1 = cos(Ry) * cos(Rz);
r2 = cos(Rz) * sin(Rx) * sin(Ry) - cos(Rx) * sin(Rz);
r3 = sin(Rx) * sin(Rz) + cos(Rx) * cos(Rz) * sin(Ry);
r4 = cos(Rx) * sin(Rz);
r5 = sin(Rx) * sin(Ry) * sin(Rz) + cos(Rx) * cos(Rz);
r6 = cos(Rx) * sin(Ry) * sin(Rz)  - cos(Rz) * sin(Rx);
r7 = -sin(Ry);
r8 = cos(Ry) * sin(Rx);
r9 = cos(Rx) * cos(Ry);

R = [ r1 r2 r3; r4 r5 r6; r7 r8 r9];

%% Tranformation 

out = M*R + trans;


