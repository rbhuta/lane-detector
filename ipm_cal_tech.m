%% IPM

input_matrix = imread('/Users/yeabserak/Box Sync/398/roma/BDXD54/IMG00002.jpg');
input_matrix = rgb2gray(input_matrix);
[rows, cols] = size(input_matrix);

%%% Setup Cord sys


x_cord = (1:rows)'; %%look up https://www.mathworks.com/help/matlab/ref/ind2sub.html
x_cord = repmat(x_cord,1,cols); %%just 1,2,3,....rows copied by cols times
%x = x_cord.* uno_vec;

y_cord = (1:cols); %%Cause transpose of x
y_cord = repmat(y_cord,rows,1);

M=[x_cord(:) y_cord(:) ones(rows*cols,1) ones(rows*cols,1)]; %Flatten
return;
%% Setup of Homogeneous Matrix

Rx = 1.8612884876e+000
Ry = 6.2500108196e-003
Rz = 1.0302968043e-002

f = 7.8429542068e+000;
f= f/6.7000000000e-003; %%Normalize

a = Rx;
B = Rz;

dx = 6.7000000000e-003;
dy = 6.7000000000e-003;

ku = dx;
kv = dy;

cx = 6.3423199573e+002;

cy = 7.4783942715e+002;

u0 = cx;
v0 = cy;



H = [ku 0 0; 0 kv 0; 0 0 1];


m_tilda = M * H';


tx = -2.0547200987e+000
ty = 4.1145446883e-001
tz =4.5443213795e+000


trans = [tx, ty,tz];
trans = repmat(trans,size(M,1),1);

r1 = cos(Ry) * cos(Rz);
r2 = cos(Rz) * sin(Rx) * sin(Ry) - cos(Rx) * sin(Rz);
r3 = sin(Rx) * sin(Rz) + cos(Rx) * cos(Rz) * sin(Ry);
r4 = cos(Ry) * sin(Rz);
r5 = sin(Rx) * sin(Ry) * sin(Rz) + cos(Rx) * cos(Rz);
r6 = cos(Rx) * sin(Ry) * sin(Rz)  - cos(Rz) * sin(Rx);
r7 = -sin(Ry);
r8 = cos(Ry) * sin(Rx);
r9 = cos(Rx) * cos(Ry);

R = [ r1 r2 r3; r4 r5 r6; r7 r8 r9];

%% Tranformation 

out = m_tilda*(R') + trans;


M_xy = [M(:,1) (M(:,2)- 1).*rows];
% out_xy = [out(:,1) (out(:,2) - 1).*rows];
out_xy = [f.*out(:,1)./out(:,3) f.*out(:,2)./out(:,3)];

return;
M_xy = M_xy(:,1) + M_xy(:,2);
out_xy = out_xy(:,1) + out_xy(:,2);

out_matrix = zeros(rows,cols);

test_out = reshape(input_matrix(M_xy),[rows,cols]);

out_xy = floor(out_xy);
check = out_xy <= 0;
out_xy(check) = 1;



out_matrix(out_xy) = input_matrix(M_xy);
imshow(out_matrix);
%out_matrix = reshape(out_matrix,[3*rows/4, cols])