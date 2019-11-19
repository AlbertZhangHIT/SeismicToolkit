% This script aims at construct .mat file of seismic training dataset
% which contains only inputs for CNN, no labels

%% You can specifiy the parameters in this section as you wish.
sgyFileList = {'D:/DataSets/SeismicData/SEG/1994BPstatics_benchmark_model', ...
                'D:/DataSets/SeismicData/SEG/bp2.5d1997/1997_2.5D_shots.segy'
               }; %% sgy file list, you can specify any folders or files.
saveFolder = 'D:/DataSets/SeismicData/SEG/TrainSets';

batchSize = 128;
patchsize = 100;
stride    = 100;
step      = 0;
arrayStep = 10; %% every 10 shot data we use 1 to generate patches

normalOpt = 3; %%% 1 for max-min normalization; 
              %%% 2 for max-abs  normalization;
              %%% 3 for row-wise normalization;
              %%% 4 for column-wise normalization;
sampleOpt = 1; %%% 1 for Monte-Carlo sample method;
               %%% 2 for norm-threshold sample method.
thresh = 0.3; %%% threshold value for Monte-Carlo sampling. Smaller the `thresh` is, fewer samples you will get. 




%% calculate the number of extracted patches
ext = {'*.segy', '*.sgy'};
filepaths = [];
for f = 1 : length(sgyFileList)
    if exist(sgyFileList{f}, 'dir')
        for i = 1 : length(ext)
            filepaths = cat(1, filepaths, dir(fullfile(sgyFileList{f}, ext{i})));
        end
    end
    if exist(sgyFileList{f}, 'file')
        if endsWith(sgyFileList{f}, '.segy') || endsWith(sgyFileList{f}, '.sgy')
            filepaths = cat(1, filepaths, dir(fullfile(sgyFileList{f})));
        end
    end
end

count = 0;
for i = 1 : length(filepaths)
    %%% information about shot in file, you could also check it by viewing the sgy file via SeiSee.
    %%%     numTracePerShot     -- width
    %%%     numSamplePerTrace   -- height
    sgyFile = fullfile(filepaths(i).folder, filepaths(i).name); % segy file 
    [totalTraces, numTracePerShot, numSamplePerTrace, sampleFormat, ~, bit] = sgyParser(sgyFile);   
    numArrays = totalTraces/numTracePerShot;
    if fix(numArrays) ~= numArrays
        disp([num2str(i), ' numArrays is not an Int.']);
        numArrays = (totalTraces - mod(totalTraces,numTracePerShot))/numTracePerShot;
    end
    for j = 1 : arrayStep : numArrays
        for x = 1+step : stride : (numSamplePerTrace-patchsize+1) % height
            for y = 1+step : stride : (numTracePerShot-patchsize+1) % width
                count = count + 1;
            end
        end
    end
end
numPatches = ceil(count/batchSize)*batchSize;

% pause
inputs = zeros(patchsize, patchsize, 1, numPatches, 'single');
disp(['Pre-alloc number of patches: ', num2str(count)]);

count = 0;
tic;
for i = 1 : length(filepaths)
    sgyFile = fullfile(filepaths(i).folder, filepaths(i).name); % segy file 
    [totalTraces, numTracePerShot, numSamplePerTrace, sampleFormat, ~, bit] = sgyParser(sgyFile);   
    numArrays = totalTraces/numTracePerShot;  
    if fix(numArrays) ~= numArrays
        disp([i, 'numArrays is not an Int.']);
        numArrays = (totalTraces - mod(totalTraces,numTracePerShot))/numTracePerShot;
    end
    offset = 3600;
    for j = 1 : arrayStep : numArrays
        disp([i, j, numArrays]);
        orig_data = readCertainTraces(sgyFile, numTracePerShot, numSamplePerTrace, offset, sampleFormat);
        offset = offset + numTracePerShot*(numSamplePerTrace*bit + 240);
        orig_data = single(orig_data);
        if normalOpt == 1
        %%% rescaled to [0, 255] or other numeric range as you like
        xmin = min(orig_data(:));
        xmax = max(orig_data(:));
        orig_data = (orig_data - xmin)/(xmax - xmin);
        end
        if normalOpt == 2
        %%% max normalize
        xmax = max(abs(orig_data(:)));
        orig_data = orig_data /xmax;
        end
        if normalOpt == 3
        %%% row-wise normalize, then rescaled to [0, 255]
        xmax = max(abs(orig_data), [], 2);
        xmax = repmat(xmax, [1, size(orig_data, 2)]);
        orig_data = orig_data./(xmax+1e-30); 
        
        xmin = min(orig_data(:));
        xmax = max(orig_data(:));
        orig_data = (orig_data - xmin)/(xmax - xmin)*255;  
        end
        if normalOpt == 4
        %%% colume-wise normalize, then rescaled to [0, 255]
        xmax = max(abs(orig_data), [], 1);
        xmax = repmat(xmax, [size(orig_data, 1), 1]);
        orig_data = orig_data./(xmax+1e-30);
    
        xmin = min(orig_data(:));
        xmax = max(orig_data(:));
        orig_data = (orig_data - xmin)/(xmax - xmin)*255;   
        end

        if sampleOpt ~= 1
            %%% norm-threshold sample
            for x = 1+step : stride : (numSamplePerTrace-patchsize+1) % height
                for y = 1+step : stride : (numTracePerShot-patchsize+1) % width
                    data = orig_data(x : x+patchsize-1, y : y+patchsize-1,:);
                    if norm(data, 2) > 1e-4
                        count = count + 1;
                        inputs(:, :, :, count) = data;
                    end
                end
            end
        else
            %%% Monte-Carlo sample
            D = mx2patches(orig_data, 'patchsize', patchsize);
            varD = var(D);
            varDmax = max(varD);
            mcSeed = varDmax * rand(size(varD));
            mcFlag = find(mcSeed<(varD*thresh));
            patchD = D(:, mcFlag);
            for k = 1 : length(mcFlag)
                count = count + 1;
                inputs(:, :, :, count) = reshape(patchD(:, k), [patchsize, patchsize]);
            end
        end

    end
end
toc;
inputs = inputs(:, :, :, 1:count);
set  = uint8(ones(1, size(inputs, 4)));

disp('-------Datasize-------')
disp([size(inputs,4), batchSize, size(inputs,4)/batchSize]);

if ~exist(saveFolder, 'dir')
    mkdir(saveFolder);
end

%%% save data
save(fullfile(saveFolder,['imdb_',num2str(patchsize),'_',num2str(batchSize)]), 'inputs','set','-v7.3');

