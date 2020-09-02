function tracesExtract(infile, outprefix, firstTraceIndex, lastTraceIndex, maxTracePerGather, maxGatherNumber)
% Parameter description:
%   infile          -- the name/path of input file.
%   outprefix       -- prefix of output file, the gather index range, 
%                      in each subfile and '.sgy' will be attached to this
%                      prefix as subfix
%                      automatically to construct a full file name.
%   firstTraceIndex  -- number of gathers in each subfile you prefer.
%   lastTraceIndex -- max number of traces in each gather, it should not be
%                      smaller than the number of traces of each gather.
%   maxTracePerGather -- max number of traces in each gather, it should not be
%                      smaller than the number of traces of each gather.
%   maxGatherNumber   -- max number of gathers in the input file, it should not be
%                      smaller than the number of gathers in the input file.
if ~exist('maxGatherNumber', 'var')
	maxGatherNumber = 10000;
end
if ~exist('maxTracePerGather', 'var')
	maxTracePerGather = 10000;
end
if ~exist('lastTraceIndex', 'var')
	lastTraceIndex = 2;
end
if ~exist('firstTraceIndex', 'var')
	firstTraceIndex = 1;
end

mexTraceExtract(infile, outprefix, firstTraceIndex, lastTraceIndex, maxTracePerGather, maxGatherNumber);

end
