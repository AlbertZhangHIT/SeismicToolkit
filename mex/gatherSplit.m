function shotSplit(infile, outprefix, shotPerSubfile, maxTracePerShot, maxShotNumber)
% Parameter description:
%   infile          -- the name/path of input file.
%   outprefix       -- prefix of output file, the first ShotID, last ShotID
%                      in each subfile and '.sgy' will be added to this prefix
%                      automatically to construct a full file name.
%   shotPerSubfile  -- number of shots in each subfile you prefer.
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
if ~exist('shotPerSubfile', 'var')
	shotPerSubfile = 1;
end

mexShotSplit(infile, outprefix, shotPerSubfile, maxTracePerShot, maxShotNumber);

end