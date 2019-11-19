function D = mx2patches(data, varargin)
if length(size(data))~=2
    error('Input data required to be 2D matrix');
    return;
end
[hei, wid] = size(data);
% Parse the optional inputs.
patchsize = 16;
stride = patchsize;
step = 0;
if (mod(length(varargin), 2) ~= 0 )
    error(['Extra Parameters passed to the function ''' mfilename ''' lambdast be passed in pairs.']);
end
parameterCount = length(varargin)/2;

for parameterIndex = 1:parameterCount
    parameterName = varargin{parameterIndex*2 - 1};
    parameterValue = varargin{parameterIndex*2};
    switch lower(parameterName)
        case 'patchsize'
            patchsize = parameterValue;
        case 'stride'
            stride = parameterValue;
        case 'step'
            step = parameterValue;
    end
end

count = 0;

%%% count dimension of D
for x = 1+step : stride : (hei-patchsize+1)
    for y = 1+step : stride : (wid-patchsize+1)
        count = count + 1;
    end
end
D = zeros(patchsize*patchsize, count);

count = 0;
for x = 1+step : stride : (hei-patchsize+1)
    for y = 1+step : stride : (wid-patchsize+1)
        count = count + 1;
        d = data(x : x+patchsize-1, y : y+patchsize-1,:);
        D(:, count) = d(:);
    end
end