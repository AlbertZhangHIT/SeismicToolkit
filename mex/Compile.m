
mex -I./../include mexsrc/mexReadCertainTraces.cpp ./../src/io.cpp mexsrc/reader.cpp
mex -I./../include mexsrc/mexGatherSplit.cpp ./../src/preReader.cpp ./../src/io.cpp ./../src/vhead.cpp
mex -I./../include mexsrc/mexShotExtract.cpp ./../src/preReader.cpp ./../src/io.cpp ./../src/vhead.cpp
mex -I./../include mexsrc/mexTraceExtract.cpp ./../src/preReader.cpp ./../src/io.cpp ./../src/vhead.cpp
mex -I./../include mexsrc/mexDifference.cpp ./../src/io.cpp ./../src/vhead.cpp
mex -I./../include mexsrc/mexSgyParser.cpp ./../src/io.cpp