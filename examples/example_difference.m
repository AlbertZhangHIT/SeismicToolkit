
orig = 'D:\SeismicDataSet\segy\mz16_cmp_stk.segy';
proc = 'D:\SeismicDataSet\segy\mz16_cmp_stk_sdct_pcg0.05.sgy';
res  = 'D:\SeismicDataSet\segy\test_difference.sgy';

maxTraces = 10000;
shotDifference(orig, proc, res, maxTraces);