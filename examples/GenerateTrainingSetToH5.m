% This script aims at construct h5 file of training dataset
% Notice: The dataset in .h5 format is much space-saving compared with .mat, and by creating the 
% .h5 file where the data hierarchy is in the following manner,
%       /sampleIndex
%               -- data
%               -- label
% it saves amount of running memory.
% The training data comes from the following folders
% 002Split : the original noisy data, which should be used as input
% 005Split : the scattering surface wave removed data, which could
%               be used as labels
% 006Split : the dune ringing noise removed data,  which could
%               be used as labels

% each file in these folders contains 1000 shots, and each shots consists
% of 294 traces, wherein there are 1876 samples in each trace.

data_folder = '../../../CNN/Northwest_AI/002Split';
label_folder = '../../../CNN/Northwest_AI/005Split';
useFiles = 5;
folder = '../../../CNN/Northwest_AI/TrainSet';
if ~exist(folder, 'dir')
    mkdir(folder);
end
savepath = fullfile(folder, 'train_testset.h5');
if exist(savepath, 'file')
    fprintf('Warning: replacing existing file %s \n', savepath);
    delete(savepath);  
end
scale = 1;

numShotsPerFile = 1000;
numTracesPerShot = 294;
numSamplePerTrace = 1876;
formatCode = 1;
bit = 4;
switch formatCode
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

data_filepaths = dir(fullfile(data_folder, '*.sgy'));
label_filepaths = dir(fullfile(label_folder, '*.sgy'));
if useFiles > length(data_filepaths)
    useFiles = length(data_filepaths);
end

count = 0;
w = ceil(numTracesPerShot/scale);
h = ceil(numSamplePerTrace/scale);

for i = 1 : useFiles % for training
    data_file = fullfile(data_folder, data_filepaths(i).name);
    shotID = data_file(end-17:end-4);
    label_file = fullfile(label_folder, label_filepaths(i).name);
    if ~contains(label_file, shotID)
        disp('Label file does not match data file. ')
    else
        offset = 3600;
        for k = 1 : numShotsPerFile
            disp([i, k])
            orig_data = readCertainTraces(data_file, numTracesPerShot, numSamplePerTrace, offset, formatCode);
            orig_label = readCertainTraces(label_file, numTracesPerShot, numSamplePerTrace, offset, formatCode);
            offset = offset + numTracesPerShot*(numSamplePerTrace * bit + 240);
            data = orig_data(600:end, :);
            label = orig_label(600:end, :);
%             data = imresize(orig_data, 1/scale, 'bicubic');
%             label = imresize(orig_label, 1/scale, 'bicubic');
            %figure(1); imagesc(sub_data), colormap(gray), colorbar;
            %figure(2); imagesc(sub_label), colormap(gray), colorbar;
            % store to h5 file
            count = count + 1;
            h5create(savepath, ['/', num2str(count), '/data'], [h, w], 'Datatype', 'single');
            h5create(savepath, ['/', num2str(count), '/label'], [h, w], 'Datatype', 'single');
            h5write(savepath, ['/', num2str(count), '/data'], data);
            h5write(savepath, ['/', num2str(count), '/label'], label);
        end
    end
end
disp(['Number of samples in training data: ', num2str(count)]);
%h5disp(savepath);
