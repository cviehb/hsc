function [xcos, ysin] = cordic(phi, N)
% [ycos ysin] = cordic_algo(x, N)
% Floating point implementation of CORDIC algorithm 
% calculating sin and cos
% in range [-pi/2, +pi/2].
% x ... angle in degrees
% N ... amount of iterations; e.g. 16 is already quite sufficient
% ------------------
% AUTHOR:   Florian Eibensteiner
% VERSION:  1.0
% HISTORY:  V1.0 - 11 March 2010 - initial version
% ------------------
xy = zeros(2, N+1);
z = zeros(1, N+1);

% init 
xy(:,1) = [1; 0];
% convert angle in radian
z(1)  = phi/180*pi;

%gain
K = 1;

% gen plot to see vector rotation
clf
plot(exp(pi*i*[0:.01:1/2]));
hold on
grid();
axis([0,1.75,0,1.75]);

for n=0:(N-1)
	%compute direction of rotation;
    if(z(n+1) < 0)
        d = -1;
    else
        d = 1;
    end
    % calculate gain
    K = K*1/sqrt(1+2^(-2*n));    
    % compute next z
	z(n+2) = z(n+1) - atan(2^-n)*d;
    %calculate x and y
	xy(:,n+2) = [1, -d*2^(-n); d*2^(-n), 1]*xy(:,n+1);
    % plot vector
	plot([0, xy(1,n+2)], [0, xy(2,n+2)]);
	plot(xy(1,n+2), xy(2,n+2));
end
xcos = xy(1, n+2)*K;
ysin = xy(2, n+2)*K;
end
