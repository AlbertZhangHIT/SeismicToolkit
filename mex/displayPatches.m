function I = displayPatches(D, numRows, numCols, sortFlag)
% function I = displayPatches(D, numRows, numCols)
% displays the patches as blocks.

borderSize = 1;
columScanFloag = 1;
strechEachVecFlag = 1;
showImFlag = 1;

patchSize = sqrt(size(D, 1));
numPatches = size(D, 2);

if (isempty(who('sortFlag')))
    sortFlag = 1;
end
if (isempty(who('numRows')))
    numRows = floor(sqrt(numPatches));
    numCols = numRows;
end
if (isempty(who('strechEachVecFlag'))) 
    strechEachVecFlag = 0;
end
if (isempty(who('showImFlag'))) 
    showImFlag = 1;
end

sizeForEachImage = patchSize + borderSize;
I = zeros(sizeForEachImage*numRows+borderSize, sizeForEachImage*numCols+borderSize, 3);
%%% fill all this image in blue
I(:,:,1) = 0; %min(min(D));
I(:,:,2) = 0; %min(min(D));
I(:,:,3) = 1; %max(max(D));

%%% now fill the image squares with the elements (in row scan or column
%%% scan).
if (strechEachVecFlag)
    for counter = 1:size(D,2)
        D(:,counter) = D(:,counter)-min(D(:,counter));
        if (max(D(:,counter)))
            D(:,counter) = D(:,counter)./max(D(:,counter));
        end
    end
end

if (sortFlag)
    vars = var(D);
    [~,indices] = sort(vars);
    indices = fliplr(indices);
    D = [D(:,1:sortFlag-1),D(:,indices+sortFlag-1)];
    signs = sign(D(1,:));
    signs(signs==0) = 1;
    D = D.*repmat(signs,size(D,1),1);
    D = D(:,1:numRows*numCols);
end    

counter = 1;
for j = 1 : numRows
    for i = 1 : numCols
        I(borderSize+(j-1)*sizeForEachImage+1:j*sizeForEachImage,borderSize+(i-1)*sizeForEachImage+1:i*sizeForEachImage,1)=reshape(D(:,counter),patchSize,patchSize);
        I(borderSize+(j-1)*sizeForEachImage+1:j*sizeForEachImage,borderSize+(i-1)*sizeForEachImage+1:i*sizeForEachImage,2)=reshape(D(:,counter),patchSize,patchSize);
        I(borderSize+(j-1)*sizeForEachImage+1:j*sizeForEachImage,borderSize+(i-1)*sizeForEachImage+1:i*sizeForEachImage,3)=reshape(D(:,counter),patchSize,patchSize);
        counter = counter+1;
    end
end

if (showImFlag) 
    I = I-min(min(min(I)));
    I = I./max(max(max(I)));
    imshow(I,[]);
end