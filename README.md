# SeismicToolkit
Toolkit for processing seismic data file (SEGY). You can use this toolkit to divide a large SEGY file into small ones, extract certain traces from a large SEGY file, extract the traces according to FFIDs and so on without loading the huge size data into memory. Linux and MATLAB on Windows are supported. 

[segymat](http://segymat.sourceforge.net/) and [SeiSee](https://seisee.software.informer.com/) are recommended for further loading data from MATLAB and visualizing.
 #

# Compile

1. Linux

 You can compile the source files through 

    $ make

in `src` subfolder. Three executables `gatherSplitor, shotExtractor` and `traceExtractor` will be compiled out in subfolder `bin`. Three parameter template files are provided in `bin` folder. 

2. Windows with MATLAB
   
Run `Compile.m` in `mex` folder. (Don't forget to set up your C++ compiler)
# Functions for MATLAB

1. [sgyParser.m](https://github.com/AlbertZhangHIT/SeismicToolkit/tree/master/mex/sgyParser.m) This function parses the volume header of the SEGY file. The example can be found in [example_sgyparser.m](https://github.com/AlbertZhangHIT/SeismicToolkit/tree/master/examples/example_sgyparser.m).

2. [shotExtract.m](https://github.com/AlbertZhangHIT/SeismicToolkit/tree/master/mex/shotExtract.m) This function extracts the traces according to the preset FFIDs into a new SEGY file. The example could be found in [example_extract_shotID.m](https://github.com/AlbertZhangHIT/SeismicToolkit/tree/master/examples/example_extract_shotID.m).

3. [tracesExtract.m](https://github.com/AlbertZhangHIT/SeismicToolkit/tree/master/mex/tracesExtract.m) This function extracts the traces according to the preset index range into a new SEGY file. The example could be found in [example_extract_traces.m](https://github.com/AlbertZhangHIT/SeismicToolkit/tree/master/examples/example_extract_traces.m).

4. [readCertainTraces.m](https://github.com/AlbertZhangHIT/SeismicToolkit/tree/master/mex/readCertainTraces.m) This function directly read traces from the large SGY file. It is not recommend to use the function directly if you are unfamiliar with the structrue of SGY file. Instead I recommend you to use [tracesExtract.m](https://github.com/AlbertZhangHIT/SeismicToolkit/tree/master/mex/tracesExtract.m) combined with `ReadSegy`function from [segymat](http://segymat.sourceforge.net/). The example can be found in [example_readtraces.m](https://github.com/AlbertZhangHIT/SeismicToolkit/tree/master/examples/example_readtraces.m)

 # Examples
1. Extracting all the gathers belong to one FFID

2. Extracting certain traces from SGY file 

   [transferVelocitytoMat.m](https://github.com/AlbertZhangHIT/SeismicToolkit/tree/master/examples/transferVelocitytoMat.m) is an example using function `readCertainTraces` to load velocity data from SGY file. The segy file can be download from [SEG open data](https://s3.amazonaws.com/open.source.geoscience/open_data/hessvti/timodel_vp.segy.gz). The loaded velocity data is presented as follows.

&emsp;&emsp;![](/figs/hessvti.png)