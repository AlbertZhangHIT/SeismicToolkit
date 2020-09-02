%%% Extract velocity model in sgy file and save it into .mat 

sgyFile = 'timodel_vp.segy';

savepath = 'hessvti.mat';

[numTraces, numTracesPerGather, numSamplePerTrace, sampleFormat, ~] = sgyParser(sgyFile);

offset = 3600;
vel = readCertainTraces(sgyFile, numTraces, numSamplePerTrace, sampleFormat);

fig = figure('Position', [100, 100, 500, 300]); set(gcf, 'color', 'white');
imagesc(vel), colorbar

print(fig, ['../figs/hessvti.png'], '-dpng');