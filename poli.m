function [P1,P2,k] = poli(data,dt)



time = data(1:length(data)-1, 1) / 1000;
angle = data(:, 2) / 180 * 3.141592653595 / 65.5;
speed = diff(angle) / dt;
sspeed = speed;

windowSize = 8; 
b = (1/windowSize)*ones(1,windowSize);
a = 1;
% sspeed = filter(b, a, sspeed);
% sspeed = filter(b, a, sspeed);
% sspeed = filter(b, a, sspeed);
% sspeed = filter(b, a, sspeed);
sspeed = expmean(sspeed, .5);

hold on;
plot(time, speed);
delay = 0.0;
plot(time-delay, sspeed);

time_stable = 3;
time_stable_filtered = time_stable;
y_inf = mean(sspeed(time_stable_filtered/dt));

t = 0.05;

t1 = floor((t - delay)/dt)
t2 = floor((2*t - delay)/dt)
t3 = floor((3*t - delay)/dt)

y_t1 = sspeed(t1)
y_t2 = sspeed(t2)
y_t3 = sspeed(t3)

line([t t], [0 .2]);
line(2*[t t], [0 .2]);
line(3*[t t], [0 .2]);

k = y_inf;

k1 = y_t1/k - 1;
k2 = y_t2/k - 1;
k3 = y_t3/k - 1;

b = 4*k1^3*k3 - 3*k1^2*k2^2 - 4*k2^3 + k3^2 + 6*k1*k2*k3;
a1 = (k1*k2 + k3 - sqrt(b)) / (2*(k1^2 + k2));
a2 = (k1*k2 + k3 + sqrt(b)) / (2*(k1^2 + k2));

P1 = log(a1)/t
P2 = log(a2)/t

end

