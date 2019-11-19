% This script aims at construct .h5 file of seismic training dataset
% which contains only inputs for CNN, no labels

sgyFile = 'D:/DataSets/SeismicData/SEG/1994BPstatics_benchmark_model/7m_shots_0201_0329.segy';
saveFolder = 'D:/DataSets/TrainSets/SeismicTrainSet';

batchSize = 128;
patchsize = 100;
stride    = 200;
step      = 0;

normalOpt = 3; %%% 1 for max-min normalization; 
              %%% 2 for max-abs  normalization;
              %%% 3 for row-wise normalization;
              %%% 4 for column-wise normalization;
sampleOpt = 1; %%% 1 for Monte-Carlo sample method;
               %%% 2 for norm-threshold sample method.
thresh = 0.3; %%% threshold value for Monte-Carlo sampling. Smaller the `thresh` is, fewer samples you will get. 

if ~exist(saveFolder, 'dir')
    mkdir(saveFolder);
end
savepath = fullfile(saveFolder, ['train_',num2str(patchsize),'.h5']);
if exist(savepath, 'file')
    fprintf('Warning: replacing existing file %s \n', savepath);
    delete(savepath);  
end
%% information about shot in file, you could find it by viewing the sgy file via SeiSee.
[totalTraces, numTracePerShot, numSamplePerTrace, sampleFormat, ~, bit] = sgyParser(sgyFile);  

numArrays = totalTraces/numTracePerShot;
if fix(numArrays) ~= numArrays
    disp([num2str(numArrays), ' numArrays is not an Int.']);
    numArrays = (totalTraces - mod(totalTraces,numTracePerShot))/numTracePerShot;
end

%% calculate the number of extracted patches
count = 0;
for i = 1 : numArrays
    for x = 1+step : stride : (numSamplePerTrace-patchsize+1) % height
        for y = 1+step : stride : (numTracePerShot-patchsize+1) % width
            count = count + 1;
        end
    end
end
numPatches = ceil(count/batchSize)*batchSize;

% pause

count = 0;
offset = 3600;
tic;
for i = 1 : numArrays
    disp([i, numArrays]);
    orig_data = readCertainTraces(sgyFile, numTracePerShot, numSamplePerTrace, offset, sampleFormat);
    offset = offset + numTracePerShot*(numSamplePerTrace*bit + 240);

    if normalOpt == 1
    %%% rescaled to [0, 255] or other numeric range as you like
    xmin = min(orig_data(:));
    xmax = max(orig_data(:));
    orig_data = (orig_data - xmin)/(xmax - xmin)*255;
    end
    if normalOpt == 2
    %%% max normalization
    xmax = max(abs(orig_data(:)));
    orig_data = orig_data /xmax;
    end
    if normalOpt == 3
    %%% row-wise normalization, then rescaled to [0, 255]
    xmax = max(abs(orig_data), [], 2);
    xmax = repmat(xmax, [1, size(orig_data, 2)]);
    orig_data = orig_data./(xmax+1e-30);    
    
    xmin = min(orig_data(:));
    xmax = max(orig_data(:));
    orig_data = (orig_data - xmin)/(xmax - xmin)*255;   
    end
    if normalOpt == 4
    %%% colume-wise normalization, then rescaled to [0, 255]
    xmax = max(abs(orig_data), [], 1);
    xmax = repmat(xmax, [size(orig_data, 1), 1]);
    orig_data = orig_data./(xmax+1e-30);
    
    xmin = min(orig_data(:));
    xmax = max(orig_data(:));
    orig_data = (orig_data - xmin)/(xmax - xmin)*255;   
    end
    
    if sampleOpt ~= 1
        %%% norm-threshold sample
        for x = 1+step : stride : (numSamplePerTrace-patchsize+1) % heigth
            for y = 1+step : stride : (numTracePerShot-patchsize+1) % width
                data = orig_data(x : x+patchsize-1, y : y+patchsize-1,:);
                if norm(data,2) > 1e-4
                    count = count + 1;
                    h5create(savepath, ['/', num2str(count), '/data'], [patchsize, patchsize], 'Datatype', 'single');
                    h5write(savepath, ['/', num2str(count), '/data'], single(data));
                end
            end
        end
    else
        %%% monto-carlo sample
        D = mx2patches(orig_data, 'patchsize', patchsize);
        varD = var(D);
        varDmax = max(varD);
        mcSeed = varDmax * rand(size(varD));
        mcFlag = find(mcSeed<(varD*thresh));
        patchD = D(:, mcFlag);    
        for j = 1 : length(mcFlag)
            count = count + 1;
            h5create(savepath, ['/', num2str(count), '/data'], [patchsize, patchsize], 'Datatype', 'single');
            h5write(savepath, ['/', num2str(count), '/data'], reshape(single(patchD(:, j)), [patchsize, patchsize]));            
        end
    end
end
toc;

disp('-------Datasize-------')
disp([count, patchsize])




