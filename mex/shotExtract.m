function shotExtract(infile, outprefix, ShotID)
% Single shot data extraction from setting shotID.
% Parameter description:
%   infile          -- the name/path of input file.
%   outprefix       -- prefix of output file, the leftShotID, rightShotID
%                      and '.sgy' will be added to this prefix
%                      automatically to construct a full file name.
%   ShotID          -- array of the IDs of shots which would be extracted.

numIDs = length(ShotID);

mexShotExtract(infile, outprefix, ShotID, numIDs);

end