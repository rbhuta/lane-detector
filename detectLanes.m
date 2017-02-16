function [detected_lanes] = detectLanes(img_path,horizon)

% Load in image
imFile = fullfile(img_path);
img = imread(imFile);

% Convert to grayscale
img_gray = rgb2gray(img);

% Basic filtering of image
img_med = medfilt2(img_gray,[5,5]);

% Tophat filtering
%img_top = imtophat(img_med,strel('rectangle',[10,69]));

slope = -0.0856;
i = 1024;
j = 1;

while i > horizon
    img_top(i,:) = imtophat(img_med(i,:),strel('rectangle',[1,round(69+slope*j)]));
    i = i - 1;
    j = j + 1;
end

% Binarize and cut off above horizon
detected_lanes = imbinarize(img_top,0.35);
detected_lanes(1:horizon,:) = 0;

end