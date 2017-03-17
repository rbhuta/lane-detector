%% IPM
close all
input_matrix = imread('/Users/yeabserak/Box Sync/398/roma/BDXD54/IMG00106.jpg');
input_matrix = rgb2gray(input_matrix);
[rows, cols] = size(input_matrix);

%%% Setup Cord sys

% % imtool(input_matrix);
% % pause;
x_cord = (1:rows)'; %%look up https://www.mathworks.com/help/matlab/ref/ind2sub.html
x_cord = repmat(x_cord,1,cols); %%just 1,2,3,....rows copied by cols times
%x = x_cord.* uno_vec;

y_cord = (1:cols); %%Cause transpose of x
y_cord = repmat(y_cord,rows,1);

U=[x_cord(:) y_cord(:) ones(rows*cols,1)]; %Image cordicante points (u v 1) matrix

%% Setup of Homogeneous Matrix

Rx = 1.8612884876e+000;
Ry = 6.2500108196e-003;
Rz = 1.0302968043e-002;


f = 7.8429542068e+000;
k = f/6.7000000000e-003; %%Normalize

Cx = 6.3423199573e+002; %% This is u0 form Navya slides

Cy = 7.4783942715e+002; %% This is v0 from Navya slides


tx = -2.0547200987e+000;
ty = 4.1145446883e-001;
tz =4.5443213795e+000;


trans = [tx, ty,tz];
%trans = repmat(trans,size(M,1),1);

r1 = cos(Ry) * cos(Rz);
r2 = cos(Rz) * sin(Rx) * sin(Ry) - cos(Rx) * sin(Rz);
r3 = sin(Rx) * sin(Rz) + cos(Rx) * cos(Rz) * sin(Ry);
r4 = cos(Ry) * sin(Rz);
r5 = sin(Rx) * sin(Ry) * sin(Rz) + cos(Rx) * cos(Rz);
r6 = cos(Rx) * sin(Ry) * sin(Rz)  - cos(Rz) * sin(Rx);
r7 = -sin(Ry);
r8 = cos(Ry) * sin(Rx);
r9 = cos(Rx) * cos(Ry);

%R = [ r1 r2 r3; r4 r5 r6; r7 r8 r9];
%SETUP UF Intrinsic and Extrinsic matrixs
R = [ r1 r2 tx;r4 r5 ty;r7 r8 tz];
K = [k 0 Cx; 0 k Cy; 0 0 1];

Main = K*R; %% This would be the M matrix from the Slides

%% Tranformation 

M_inv = inv(Main);
M_1 = U*M_inv'; %% Looks wierd but it works since I have all the coordinates in U

out = [M_1(:,1)./M_1(:,3) M_1(:,2)./M_1(:,3) M_1(:,3)./M_1(:,3)];


U_uv = [U(:,1) (U(:,2)- 1).*rows]; %U_uv will give us access to all the values in U

out_xy = out;

% The next two lines help to index into the image with single index as in
%  
U_uv = U_uv(:,1) + U_uv(:,2);
out_xy = out_xy(:,1) + out_xy(:,2);

%% Preparing my output
out_matrix = zeros(rows,cols);

test_out = reshape(input_matrix(U_uv),[rows,cols]);
return
%Round vlaues, this is crude and incorrect but was only way to prevent
%inproper indexing for now
out_xy = floor(out_xy);
check = out_xy <= 0 | out_xy > max(U_uv);
out_xy(check) = 1;


out_matrix(out_xy) = input_matrix(U_uv);
out_matrix = uint8(out_matrix);
% imshow(out_matrix);
% return;
subplot(1,2,1)
imshow(out_matrix);
subplot(1,2,2)
imshow(test_out);
%out_matrix = reshape(out_matrix,[3*rows/4, cols])