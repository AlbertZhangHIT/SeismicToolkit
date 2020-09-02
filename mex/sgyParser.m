function [totalTraces, tracesPerGather, samplePerTrace, sampleFormat, sampleRate, bit] = sgyParser(sgyFile)
%%% Parameter description
%%% INPUT:      
%%%     sgyFile             --file path of sgy file.
%%% OUTPUT:
%%%     totalTraces         --total number of traces in the assigned sgy file
%%%     tracesPerGahter     --number of traces in each shot gather, this
%%%                           may be not correct
%%%     samplePerTrace      --number of sample per trace
%%%     sampleFormat        --sample format
%%%     sampleRate          --sample interval(s)
%%%     bit                 --storation bit of each sample point

[totalTraces, tracesPerGather, samplePerTrace, sampleFormat, sampleRate, bit] = mexSgyParser(sgyFile);

end