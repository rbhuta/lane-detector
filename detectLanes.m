function [detected_lanes] = detectLanes(img_path,horizon, width)

% Load in image
imFile = fullfile(img_path);
img = imread(imFile);

% Convert to grayscale
img_gray = rgb2gray(img);

% Set Filter Size
filterSize = [5 5];

% Using Integral Image for Box Avg Filter
padSize = (filterSize-1)/2;
Apad = padarray(img_gray, padSize, 'replicate','both');
intA = integralImage(Apad);
int_img = integralBoxFilter(intA, filterSize);
img_int = uint8(int_img);

% Tophat filtering
slope = -width / (1024 - horizon);

% Computing width of markers into array
for i=size(img_int,1):-1:horizon
    SM(i) = round(width + i*slope)+1;
end
s = width;
thresh = 10;
img_grad = img_gray;

col = size(img_int,2);
for v = size(img_int,1):-1:horizon
    for u = 1:col
        u_max = min(col, u+6*s);
        u_min = max(col, u-6*s);
        du = round(u_max-u_min)+1;
        if (img_gray(v,u) > (thresh+(img_int(v,round(u_max))-img_int(v,round(u_min)))/du))
            img_grad(v,u) = 255;
        end
    end
    s = round(SM(v)*0.5)+1;
    for u = 2*s+1:col-2*s
        top_hat(v,u) = 1/(4*s)*(2*(int_img(v,u+s)-int_img(v,u-s))-(int_img(v,u+2*s)-int_img(v,u-2*s)));
    end
    thresh_tophat = 5;
end

    
    
% Binarize and cut off above horizon
detected_lanes = (top_hat) > (0.35*255);
%detected_lanes = img_top & detected_lanes;
detected_lanes(1:horizon,:) = 0;
figure
imshow(detected_lanes)
end