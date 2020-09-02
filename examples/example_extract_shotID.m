% This script extracts pre-assigned shots from large sgy file

inFile = 'E:\Datasets\SeismicData\SEG\C3_45shot\SEG_45Shot_shots1-9.sgy';
outPrefix = 'E:\Datasets\SeismicData\SEG\C3_45shot\SEG_45Shot_shots1.sgy';
ShotID = [1];

tic
shotExtract(inFile, outPrefix, ShotID);
toc