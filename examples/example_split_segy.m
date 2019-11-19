% This script aims at splitting large sgy file into small pieces

inFile = 'D:\SeismicDataSet\CNN\Northwest_AI\005Split\005_3A_group_dctl_8337_shotID_443676_450848.sgy';
outPrefix = 'D:\SeismicDataSet\CNN\Northwest_AI\TrainSet\005\005_dctl_8337';
shotPerfile = 1;
maxTracesPerShot = 294;

shotSplit(inFile, outPrefix, shotPerfile, maxTracesPerShot);