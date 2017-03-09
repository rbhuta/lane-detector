function [detected_lanes] = detectLanes(img_path,horizon, width)

% Load in image
imFile = fullfile(img_path);
img = imread(imFile);
tic;
% Convert to grayscale
img_gray = rgb2gray(img);

% Set Filter Size
filterSize = [9 9];

% Using Integral Image for Box Avg Filter
padSize = (filterSize-1)/2;
Apad = padarray(img_gray, padSize, 'replicate','both');
intA = integralImage(Apad);
int_img = integralBoxFilter(intA, filterSize);
filt_img = uint8(int_img);

% Tophat filtering 
slope = -width / (1024 - horizon);

j = 0;
% Computing width of markers into array
for i=size(filt_img,1):-1:horizon
    SM(i) = max(round(width + j*slope),1);
    j = j + 1;
end
s = width;
thresh = 5;
img_grad = img_gray;

col = size(filt_img,2);
for v = size(filt_img,1):-1:horizon
    % compute int image
    sum = double(filt_img(v,1));
    for u = 1:col
        int_img_gray(v,u) = sum;
        sum = sum + double(filt_img(v,u));
    end

    for u = 1:col
        u_max = min(col, u+6*SM(v));
        u_min = max(col, u-6*SM(v));
        du = round(u_max-u_min)+1;
        if (img_gray(v,u) > (thresh+(int_img_gray(v,round(u_max))-int_img_gray(v,round(u_min)))/du))
            img_grad(v,u) = 255;
        end
    end
    s = round(SM(v)*0.5)+1;
    for u = 2*s+1:col-2*s
        top_hat(v,u) = 1/(4*s)*(2*(int_img_gray(v,u+s)-int_img_gray(v,u-s))-(int_img_gray(v,u+2*s)-int_img_gray(v,u-2*s)));
    end
    thresh_tophat = 5;
end
toc;
    
% Binarize and cut off above horizon
detected_lanes = (top_hat) > (1.5);
%detected_lanes = img_top & detected_lanes;
detected_lanes(1:horizon,:) = 0;
figure
imshow(detected_lanes)
end