seq = char({'BDXD54/';'BDXN01/';'IRC041500/';'IRC04510/';'LRAlargeur13032003/';'LRAlargeur14062002/';'LRAlargeur26032003/';'RouenN8IRC051900/';'RouenN8IRC052310/'});
%sequence = ['BDXN01/',';
imagelist = 'imgnormal';
situation = 'normal';

for s=1:size(seq,1)
    sequence = seq(s,:);
    head =  strcat('roma/',sequence);
    lname=sprintf('%s/%s.mov',head, imagelist);
    [names,num]=loadlist(lname);
    
    
    
    par_folder = strcat(head,'*.jpg');
    grd_files = strcat(head,'*.pgm');
    
    %image1_dir = dir(par_folder);
    grd_files = dir(grd_files);
    
    %BDXD54\images\*.jpg
    parm_cal_location = strcat(head,'param.cal');
    parm = dlmread(parm_cal_location, ' ');
    horizon = parm(1);
    w_pixel = parm(2);
    
    
    
    for image_number=1:num
        
        image_num = char(names(image_number));
        grd_num = grd_files(image_number).name;
        img_dir = strcat(head,image_num,'.jpg');
        
        grd_T_loc = strcat(head,grd_num);
        grd_tru = imread(grd_T_loc);
        output_file = strcat(head,situation, '/', 'detectLanes','_',image_num,'.txt');
        
        if(exist(output_file, 'file') ~= 2)
            %img1 = imread();
            detected_lanes = detectLanes(img_dir,horizon,w_pixel);
            %imshow(detected_lanes);
            
            %imshow(grd_tru)
            %pause
            TP_alg = detected_lanes & grd_tru;
            TP_alg = sum(sum(TP_alg));
            
            FP_alg = (detected_lanes & (~grd_tru));
            FP_alg = sum(sum(FP_alg));
            
            FN_alg = (~detected_lanes & grd_tru);
            FN_alg = sum(sum(FN_alg));
            
            TN_alg = (~detected_lanes & ~grd_tru);
            TN_alg = sum(sum(TN_alg));
            
            
            
            fileID = fopen(output_file,'w');
            if(fileID == -1)
                mkdir(head,situation) ;
                fileID = fopen(output_file,'w');
            end
            fprintf(fileID,'%f\n',0.35);
            fprintf(fileID,'%d\n',TP_alg);
            fprintf(fileID,'%d\n',FP_alg);
            fprintf(fileID,'%d\n',FN_alg);
            fprintf(fileID,'%d\n',TN_alg);
            fclose(fileID);
            
            % - threshold value used by the extraction algorithm
            % - True Positive number of pixels
            % - False Positive number of pixels
            % - True Negative number of pixels
            % - False Negative number of pixels
            %     %/sequence/situation/algoname_imagename.txt
            %[TP,FP,TN,FN,values]=loadroc(head(1:end-1),situation,'img','detectLanes',TP_alg,FP_alg,TN_alg,FN_alg);
        end
    end
end
