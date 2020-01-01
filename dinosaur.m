clear;
W = load('rotdino.mat');
W = full(W.M_inliers');
W(W==0) = nan;
W_init = W;
T = nanmean(W_init,1); %mean values
T = repmat(T, [size(W_init,1),1]);
W_init(isnan(W_init)) = T(isnan(W_init)); %replace nan's with mean values
t = mean(W_init,1)'; 
W_init = W_init - repmat(t', [size(W_init,1) 1]);

[u,d,v] = svds(W_init,3);
d=d^(1/2);
S=(u*d)';

global R;
R=(d*v')';

syms a11 a12 a13 a21 a22 a23 a31 a32 a33;
x=[a11 a12 a13 a22 a23 a33];
Q=[a11,a12,a13;a12,a22,a23;a13,a23,a33];
QT=Q';
eqs=[];
cnt=0;
eqnum=3;
start=randi(30);
for i=start:2:start+2
    eqs=[eqs, R(i,:)*Q*R(i,:)'==1];
    cnt=cnt+1;
%     if cnt>=eqnum
%         break
%     end
    eqs=[eqs, R(i,:)*Q*R(i+36,:)'==0];
    cnt=cnt+1;
%     if cnt>=eqnum
%         break
%     end
    eqs=[eqs, R(i+36,:)*Q*R(i+36,:)'==1];
    cnt=cnt+1;
%     if cnt>=eqnum
%         break
%     end
end
y=solve(eqs,x)
Q=[y.a11,y.a12,y.a13;y.a12,y.a22,y.a23;y.a13,y.a23,y.a33];
Q=double(Q);

% x0 = randi(100,6,1);  % Make a starting guess at the solution
% % % opts = optimoptions('fsolve','Display','iter'); % Option to display output
% % opts = optimoptions(@fsolve,'Algorithm', 'levenberg-marquardt');
% % opts = optimset('Display', 'iter');% 设定'Display'选项为'iter'模式
% % options = optimoptions(@fsolve,...
% %     'Algorithm','levenberg-marquardt','Display','iter');
% % options.StepTolerance = 1e5;
% % options.OptimalityTolerance=1e5;
% % options.FunctionTolerance=1e5;
% [x,fval] = fsolve('forQ',x0); % Call solver
% Q=[x(1),x(2),x(3);x(2),x(4),x(5);x(3),x(5),x(6)];

[V,D]=eig(Q);
Q=V*sqrt(D);


S=Q\S;

x=S(1,:);
y=S(2,:);
z=S(3,:);
figure;
plot3(x,y,z, '.','color',[0 0 0]);



