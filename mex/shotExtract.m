function shotExtract(infile, outprefix, ffids)
% Single shot data extraction from setting shotID.
% Parameter description:
%   infile          -- the name/path of input file.
%   outprefix       -- prefix of output file, the leftShotID, rightShotID
%                      and '.sgy' will be added to this prefix
%                      automatically to construct a full file name.
%   ffids          -- array of the FFIDs of shots which would be extracted.

numIDs = length(ffids);

mexShotExtract(infile, outprefix, ffids, numIDs);

end