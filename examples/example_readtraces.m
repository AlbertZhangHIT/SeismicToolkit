% This script reads a preset number of traces from large sgy file

orig = 'D:\SeismicDataSet\segy\mz16_cmp_stk.segy';

numTraces = 4000;
numSamples = 8001;
formatCode = 1;

offset = 3600;
data = readCertainTraces(orig, numTraces, numSamples, offset, formatCode);

figure(1)
imagesc(data), colormap(gray), colorbar
