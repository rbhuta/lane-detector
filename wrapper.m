
sequence = 'BDXD54/';
par_folder = strcat('roma/',sequence,'*.jpg');
image1_dir = dir(par_folder);

%BDXD54\images\*.jpg
parm_cal_location = strcat('roma/',sequence,'param.cal');
parm = dlmread(parm_cal_location, ' ');
horizon = parm(1);
w_pixel = parm(2);

for image_number=1:length(image1_dir)
    
img_dir = strcat('roma/',sequence,image1_dir(image_number).name);
display   
%img1 = imread();
detected_lanes = detectLanes(img_dir,horizon);
imshow(detected_lanes);

pause
%/sequence/situation/algoname_imagename.txt
%[TP,FP,TN,FN,values]=loadroc(sequence,situation,imagelist,algoname,TP,FP,TN,FN)
  
end
