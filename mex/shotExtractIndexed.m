function shotExtractIndexed(infile, outprefix, leftIndex, rightIndex, maxTracePerShot, maxShotNumber)
% Parameter description:
%   infile          -- the name/path of input file.
%   outprefix       -- prefix of output file, the leftShotID, rightShotID
%                      and '.sgy' will be added to this prefix
%                      automatically to construct a full file name.
%   leftIndex      -- the index of shot which would be extracted first.
%   rightIndex     -- the index of shot which would be extracted last.
%
%   maxTracePerShot -- max number of traces in each shot, it should not be
%                      smaller than the number of traces of each shot.
%   maxShotNumber   -- max number of shots in the input file, it should not be
%                      smaller than the number of shots in the input file.

if ~exist('maxShotNumber', 'var')
	maxShotNumber = 1000000;
end
if ~exist('maxTracePerShot', 'var')
	maxTracePerShot = 100000;
end

mexShotExtractIndexed(infile, outprefix, leftIndex, rightIndex, maxTracePerShot, maxShotNumber);

end