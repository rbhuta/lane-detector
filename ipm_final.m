%% IPM
close all
input_matrix = imread('/Users/yeabserak/Box Sync/398/roma/BDXD54/IMG00030.jpg');
%input_matrix = imread('/Users/yeabserak/Box Sync/398/Code/images/combined_filter.jpg');
input_matrix = rgb2gray(input_matrix);
%input_matrix = imread('images/test_result.jpg');
[rows, cols] = size(input_matrix);

vh = 405 + 20;
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
%Parameters from Roma Dataset parameter file
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

%% Using Parameters to convert to IPM view
Main = K*R; %%  M matrix from the Slides

M_inv = inv(Main); %%Not needed

vh = 405; %%Horizon Of Image 

%Output Image Dimension
%Parameters from orginal image
xmin = -15; %% 
xmax = 15; %%
ymin = 0; %%
ymax = 60; %%
out = zeros(300, 600); %%Ou

for x = xmin:0.1:xmax
   for y = ymin : 0.1:ymax
       m = Main*[x;y;1];  % -----> the ground point i want to "color"
       m = m/m(end); % ---> u v coordinates in the image
       m = round(m);
       if(m(1) > 1 && m(1) < cols && m(2) > 1 && m(2) < rows)
         out(round(x*10 + 151), round(y*10 + 1), :) = double(input_matrix(m(2), m(1),:));
       end
   end
end
%figure,
%subplot(1,2,1)
out = flip(out,2)';
%imshow(mat2gray(input_matrix,[0,255]));
%subplot(1,2,2)
%imshow(input_matrix)
figure
imshow(mat2gray(out,[0,255]));

hold on
plot(OY,(OX-50),'b.','MarkerSize',10);
hold  on
plot((data(1,:)+10),yAxis,'r-','LineWidth',2);

%% RANSAC Prep

[OX,OY] = find(out);
hold on
plot(OY,OX,'b.','MarkerSize',10);
data = [OY,OX]';


%% 
% num = 2;
% iter = 100;
% threshDist = 10;
% inlierRatio = .99; %%adjustment for noise
num = 2; %% Number of points needed to make a line fit 
iter = 100; %%Number of iteration
threshDist = 20; %%Threshold on inliners
inlierRatio = .40; %%adjustment for noise

[bestParameter1,bestParameter2]=ransac_demo(data,num,iter,threshDist,inlierRatio)
yAxis = bestParameter1*data(1,:) + bestParameter2;
figure
imshow(mat2gray(out,[0,255]));
hold on
plot(OY,OX,'b.','MarkerSize',10);
hold  on
plot(data(1,:),yAxis,'r-','LineWidth',2);