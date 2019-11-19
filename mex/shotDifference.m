function shotDifference(infile1, infile2, outfile, maxTraces)
if ~exist('maxTraces', 'var')
	maxTraces = 100000;
end

mexDifference(infile1, infile2, outfile, maxTraces);

end