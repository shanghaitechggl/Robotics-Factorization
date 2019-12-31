function eqs=forQ(x)
global R;
eqs=[];
Q=[x(1),x(2),x(3);x(2),x(4),x(5);x(3),x(5),x(6)];
% QT=Q';
% cnt=0;
% eqnum=9;
start=randi(30)*2+1;
for i=start:2:start+2
    eqs=[eqs, R(i,:)*Q*R(i,:)'-1];
    eqs=[eqs, R(i,:)*Q*R(i+1,:)'];
    eqs=[eqs, R(i+1,:)*Q*R(i+1,:)'-1];
end

end