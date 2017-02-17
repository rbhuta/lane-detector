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
i = 1024;
j = 1;

while i > horizon
    img_top(i,:) = imtophat(img_int(i,:),strel('rectangle',[1,round(70+slope*j)+1]));
    i = i - 1;
    j = j + 1;
end

% Binarize and cut off above horizon
detected_lanes = (img_top) > (0.35*255);
detected_lanes = img_top & detected_lanes;
detected_lanes(1:horizon,:) = 0;

end
