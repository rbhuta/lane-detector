% Authors: ROMA 
% this function compute the mean roc and dice curves
% 
% situation is the name of the subdirectory where files generated by the extraction algorithm are
% imagelist is the name of the file with the list of the images to be used (img.mov, imgnormal.mov, imgadvlight.mov, imghighcurv.mov)
% algoname is the name of the used extraction algorithm
% color is the curve color for display
% dispflag is 0 to display roc curve and 1 to display dice curve
%
% filenames generated by the extraction algorithm must be with the format : /sequence/situation/algoname_imagename.txt
%
function done=meangraph(situation,imagelist,algoname,color,dispflag)

TP = [];
FP = [];
TN = [];
FN = [];
[TP,FP,TN,FN,values]=loadroc('roma/BDXD54', situation, imagelist, algoname, TP,FP,TN,FN);
[TP,FP,TN,FN,values]=loadroc('roma/BDXN01', situation, imagelist, algoname, TP,FP,TN,FN);
[TP,FP,TN,FN,values]=loadroc('roma/IRC041500', situation, imagelist, algoname, TP,FP,TN,FN);
[TP,FP,TN,FN,values]=loadroc('roma/IRC04510', situation, imagelist, algoname, TP,FP,TN,FN);
[TP,FP,TN,FN,values]=loadroc('roma/LRAlargeur13032003', situation, imagelist, algoname, TP,FP,TN,FN);
[TP,FP,TN,FN,values]=loadroc('roma/LRAlargeur14062002', situation, imagelist, algoname, TP,FP,TN,FN);
[TP,FP,TN,FN,values]=loadroc('roma/LRAlargeur26032003', situation, imagelist, algoname, TP,FP,TN,FN);
%[TP,FP,TN,FN,values]=loadroc('roma/RD116', situation, imagelist, algoname, TP,FP,TN,FN);
[TP,FP,TN,FN,values]=loadroc('roma/RouenN8IRC051900', situation, imagelist, algoname, TP,FP,TN,FN);
[TP,FP,TN,FN,values]=loadroc('roma/RouenN8IRC052310', situation, imagelist, algoname, TP,FP,TN,FN);

totalTP =  sum(TP);
totalFP =  sum(FP);
totalTN =  sum(TN);
totalFN =  sum(FN);
totalN = totalTN + totalFP;
totalP = totalTP + totalFN;

% roc curve computation
totalTPR = totalTP./totalP
totalFPR = totalFP./totalN

% dice curve computation
totalDice = 2*totalTP./(totalTP+totalFP+totalP);

% display roc curve
hold on;
set(gca, 'FontSize', 12);
if (dispflag==0) 
	xlabel('False positive rate', 'fontsize', 12);
	ylabel('True positive rate', 'fontsize', 12);
	plot(totalFPR, totalTPR, 'LineWidth', 2);
end

% display dice max value
[valmax,indmax]=max(totalDice);
fprintf('algo %s: threshold value at dice max=%f, TPR at dice max=%f, dice max value=%f\n',algoname,values(indmax),totalTPR(indmax),totalDice(indmax));

% display dice curve
if (dispflag==1)
        xlabel('Threshold', 'fontsize', 12);
        ylabel('Dice', 'fontsize', 12);
        plot(values, totalDice, color);
end




