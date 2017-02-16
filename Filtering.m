close all

horizon = 405;

% Load in image
imFile = fullfile('roma','BDXD54','IMG00019.jpg');
img = imread(imFile);

% Convert to grayscale
img_g = rgb2gray(img);

% Basic filtering of image
img_f = imgaussfilt(img_g,3);
img_f = medfilt2(img_g,[5,5]);

% Tophat filtering
%img_a = imtophat(img_f,strel('line',100,1));
img_a = imtophat(img_f,strel('rectangle',[10,69]));
%img_d = img_a + img_b;
figure
imshow(img_a)
slope = -0.0856;
i = 1024;
j = 1;

while i > horizon
    img_a(i,:) = imtophat(img_f(i,:),strel('rectangle',[1,round(69+slope*j)]));
    i = i - 1;
    j = j + 1;
end

% Binarize and cut off above horizon
img_a = imbinarize(img_a,0.35);
img_a(1:horizon,:) = 0;

figure;
imshow(img_a);