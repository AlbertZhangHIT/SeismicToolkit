function data = readCertainTraces(filename, numTraces, numSamples, OffSet, formatCode)
% Parameter description
%   filename            -- the name/path of input file.
%   numTraces           -- number of traces you would like to read out.
%   numSamples          -- sample rate in each trace.
%   OffSet              -- the pointer location in input file where you
%                          would like to start reading data.
%   formatCode          -- data format code.

if ~exist('formatCode', 'var')
	formatCode = 1;
end

switch formatCode
    case 1              % 4-byte IBM floating-point
        bit = 4;
    case 2              % 4-byte two's complement integer
        bit = 4;
    case 3              % 2-byte two's complement integer
        bit = 2;
    case 4              % 4-byte fixed point with gain (obsolete)
        bit = 4;
    case 5              % 4-byte IEEE floating-point
        bit = 4;
    case 8              % 1-byte two's complement integer
        bit = 1;
end

data = mexReadCertainTraces(filename, numTraces, numSamples, OffSet, bit);
end