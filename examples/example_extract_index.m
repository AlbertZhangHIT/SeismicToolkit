% This script aims at extract pre-assigned shots from large sgy file

%inFile = 'D:\SeismicDataSet\CNN\Northwest_AI\005Split\005_3A_group_dctl_8337_shotID_443676_450848.sgy';
%outPrefix = 'D:\SeismicDataSet\CNN\Northwest_AI\TrainSet\005_dctl_8337';
inFile =  'D:\SeismicDataSet\segy\mz16_sy1_3d_s110_tseq_shotID_8146_8110.sgy';
outPrefix = 'D:\SeismicDataSet\segy\mz16_sy1_3d_s110_tseq';
leftShotIndex = 1;
rightShotIndex = 1;
maxTracesPerShot = 2010;
tic
shotExtractIndexed(inFile, outPrefix, leftShotIndex, rightShotIndex, maxTracesPerShot);
toc