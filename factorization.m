filename='cppW.txt';
W=textread(filename);
W=W(:,1:size(W,2)-1);
T = nanmean(W,1); %mean values
T = repmat(T, [size(W,1),1]);
W=W-T;

[u,d,v]=svds(W,3);
r=(d^(1/2)*v');
x=r(1,:);
y=r(2,:);
z=r(3,:);
figure;
plot3(x,y,z, '.');
% plot3(x,y,z);
