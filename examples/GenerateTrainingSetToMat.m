% This script aims at construct .mat file of seismic training dataset
% which contains only inputs for CNN, no labels

sgyFile = 'D:/DataSets/SeismicData/SEG/1994BPstatics_benchmark_model/7m_shots_0201_0329.segy';
saveFolder = 'D:/DataSets/SeismicData/SEG/TrainSets';

batchSize = 128;
patchsize = 64;
stride    = 64;
step      = 0;

patchOpt = 1; %%% 1 for max-min normalization and monto-carlo sample; 
              %%% 2 for max-abs  normalization and norm-threshold sample;
              %%% 3 for row-wise normalization and norm-thresholod sample;
              %%% 4 for column-wise normalizatin and norm-threshold sample;
thresh = 0.3;
%%% information about shot in file, you could also check it by viewing the sgy file via SeiSee.
%%%     numTracePerShot     -- width
%%%     numSamplePerTrace   -- height
[totalTraces, numTracePerShot, numSamplePerTrace, sampleFormat, ~] = sgyParser(sgyFile);

numArrays = totalTraces/numTracePerShot;


if fix(numArrays) ~= numArrays
    error('numArrays is not an Int.');
    return;
end

switch sampleFormat
    case 1              % 4-byte IBM floating-point
        bit = 4;
    case 2              % 4-byte two's complement integer
        bit = 4;
    case 3              % 2-byte two's complement integer
        bit = 2;
    case 4              % 4-byte fixed point with gain (obsolete)
        bit = 4;
    case 5              % 4-byte IEEE floating-point
        bit = 4;
    case 8              % 1-byte two's complement integer
        bit = 1;
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
inputs = zeros(patchsize, patchsize, 1, numPatches, 'single');
count = 0;
offset = 3600;
tic;
for i = 1 : numArrays
    disp([i, numArrays]);
    orig_data = readCertainTraces(sgyFile, numTracePerShot, numSamplePerTrace, offset, sampleFormat);
    offset = offset + numTracePerShot*(numSamplePerTrace*bit + 240);
    orig_data = single(orig_data);
    if patchOpt == 1
    %%% normalize to [0, 1] or other numeric range as you like
    xmin = min(orig_data(:));
    xmax = max(orig_data(:));
    orig_data = (orig_data - xmin)/(xmax - xmin);
    end
    if patchOpt == 2
    %%% max normalize
    xmax = max(abs(orig_data(:)));
    orig_data = orig_data /xmax;
    end
    if patchOpt == 3
    %%% row-wise normalize
    xmax = max(abs(orig_data), [], 2);
    xmax = repmat(xmax, [1, size(orig_data, 2)]);
    orig_data = orig_data./(xmax+1e-30);    
    end
    if patchOpt == 4
    %%% colume-wise normalize
    xmax = max(abs(orig_data), [], 1);
    xmax = repmat(xmax, [size(orig_data, 1), 1]);
    orig_data = orig_data./(xmax+1e-30);
    end

    if patchOpt ~= 1
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
        %%% monto-carlo sample
        D = mx2patches(orig_data, 'patchsize', patchsize);
        varD = var(D);
        varDmax = max(varD);
        mcSeed = varDmax * rand(size(varD));
        mcFlag = find(mcSeed<(varD*thresh));
        patchD = D(:, mcFlag);
        for j = 1 : length(mcFlag)
            count = count + 1;
            inputs(:, :, :, count) = reshape(patchD(:, j), [patchsize, patchsize]);
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

