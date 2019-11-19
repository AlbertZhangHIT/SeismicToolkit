%%% Extract velocity model in sgy file and save it into .mat 

sgyFile = '../../SEG/hessvti/timodel_vp.segy';

savepath = '../../VelModel/velocities/hessvti.mat';

[numTraces, numTracesPerGather, numSamplePerTrace, sampleFormat, ~] = sgyParser(sgyFile);

offset = 3600;
vel = readCertainTraces(sgyFile, numTraces, numSamplePerTrace, sampleFormat);

figure, imagesc(vel), colorbar

save(savepath, 'vel')