
data =        importdata('data2/data_l0.txt') + importdata('data2/data_l1.txt') + importdata('data2/data_l2.txt');
data = data + importdata('data2/data_l3.txt') + importdata('data2/data_l4.txt') + importdata('data2/data_l5.txt');
data = data + importdata('data2/data_l6.txt') + importdata('data2/data_l7.txt') + importdata('data2/data_l8.txt');
data = data + importdata('data2/data_l9.txt');

% data =        importdata('data2/data_r0.txt') + importdata('data2/data_r1.txt') + importdata('data2/data_r2.txt');
% data = data + importdata('data2/data_r3.txt') + importdata('data2/data_r4.txt') + importdata('data2/data_r5.txt');
% data = data + importdata('data2/data_r6.txt') + importdata('data2/data_r7.txt') + importdata('data2/data_r8.txt');
% data = data + importdata('data2/data_r9.txt');

data = data/10;
dt = 0.005;

[P1,P2,k] = poli(data, dt);

u = 1;
P1P2 = P1 * P2
P12 = -P1 -P2
kk = k/u*P1*P2

R = 0.056/2
d = 0.10

model = zpk([], [P2 P1], k/u*P1P2);
time = 0:dt:3.5;
sspeed = step(model, time);
plot(time, sspeed);


