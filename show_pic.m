filename='shape.txt';
data=textread(filename);
data=data(:,1:size(data,2)-1);
x=data(1,:);
y=data(2,:);
z=data(3,:);
figure;
plot3(x,y,z, '.');
% plot3(x,y,z);
