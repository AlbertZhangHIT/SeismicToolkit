% This script aims at extract pre-assigned shots from large sgy file

%inFile = 'D:\SeismicDataSet\CNN\Northwest_AI\005Split\005_3A_group_dctl_8337_shotID_443676_450848.sgy';
%outPrefix = 'D:\SeismicDataSet\CNN\Northwest_AI\TrainSet\005_dctl_8337';
inFile = 'E:\Datasets\SeismicData\Lab\P1007_NUCNS_AGORA2_SHOT_FLATST.SGY';
outPrefix = 'E:\Projects\LabProject\VSP3D-master\3D_SHOT_2.segy';
ShotID = [30321, 102116];

tic
shotExtract(inFile, outPrefix, ShotID);
toc