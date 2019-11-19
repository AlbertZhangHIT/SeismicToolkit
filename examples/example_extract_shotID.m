% This script aims at extract pre-assigned shots from large sgy file

%inFile = 'D:\SeismicDataSet\CNN\Northwest_AI\005Split\005_3A_group_dctl_8337_shotID_443676_450848.sgy';
%outPrefix = 'D:\SeismicDataSet\CNN\Northwest_AI\TrainSet\005_dctl_8337';
inFile = 'G:\Data_Field\Data_GeoEast\mz16_sy1_3d_sl10_tseq.segy';
outPrefix = 'D:\SeismicDataSet\segy\mz16_sy1_3d_s110_tseq';
leftShotID = 8134;
rightShotID = 8106;
maxTracesPerShot = 2010;
tic
shotExtract(inFile, outPrefix, leftShotID, rightShotID, maxTracesPerShot);
toc