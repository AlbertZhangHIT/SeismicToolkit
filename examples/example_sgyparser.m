% This file is an example of using function 'sgyParser'

file = 'D:/DataSets/SeismicData/SEG/1994BPstatics_benchmark_model/7m_shots_0201_0329.segy';

[totalTraces, tracesPerGather, samplePerTrace, sampleFormat, ~] = sgyParser(file);