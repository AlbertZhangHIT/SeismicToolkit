

orig = 'D:\SeismicDataSet\segy\mz16_cmp_stk.segy';
proc = 'D:\SeismicDataSet\segy\mz16_cmp_stk_sdct_pcg0.999.sgy';

numTraces = 4000;
numSamples = 8001;
formatCode = 1;
bit = 4;

offset = 3600;
orig_data = readCertainTraces(orig, numTraces, numSamples, offset, formatCode);
proc_data = readCertainTraces(proc, numTraces, numSamples, offset, formatCode);

res = orig_data - proc_data;
figure(1)
imagesc(res), colormap(gray), colorbar
