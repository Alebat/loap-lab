

dt = 0.005;
R = 0.056/2;
d = .125;
u = 1;
time = 0:dt:3.5;
t = 0.054;
hold on;


% --- Left Motor ---
data =        importdata('data2/data_l0.txt') + importdata('data2/data_l1.txt') + importdata('data2/data_l2.txt');
data = data + importdata('data2/data_l3.txt') + importdata('data2/data_l4.txt') + importdata('data2/data_l5.txt');
data = data + importdata('data2/data_l6.txt') + importdata('data2/data_l7.txt') + importdata('data2/data_l8.txt');
data = data + importdata('data2/data_l9.txt');
data = data/10;

[lP1,lP2,lk] = poli(data, dt, t, 1);


lP1P2 = lP1 * lP2;
lP12 = -lP1 -lP2;
lkk = lk/u*lP1*lP2;

lP1
lP2

total = tf([.1], [1 0 0]);
modell = zpk([], [lP2 lP1], lk/u*lP1P2);

sspeed = step(modell, time);
plot(time, sspeed);


% --- Right Motor ---
data =        importdata('data2/data_r0.txt') + importdata('data2/data_r1.txt') + importdata('data2/data_r2.txt');
data = data + importdata('data2/data_r3.txt') + importdata('data2/data_r4.txt') + importdata('data2/data_r5.txt');
data = data + importdata('data2/data_r6.txt') + importdata('data2/data_r7.txt') + importdata('data2/data_r8.txt');
data = data + importdata('data2/data_r9.txt');
data = data/10;

[rP1,rP2,rk] = poli(data, dt, t, 1);


rP1P2 = rP1 * rP2;
rP12 = -rP1 -rP2;
rkk = rk/u*rP1*rP2;

rP1
rP2

modelr = zpk([], [rP2 rP1], rk/u*rP1P2);
sspeed = step(modelr, time);
plot(time, sspeed);

% Left motor controller
% from root locus C(s) = (s1 s + s2) / s
s1 = 5;
s2 = 100;
q1 = s1 + dt * s2 / 2
q2 = -s1 + dt * s2 / 2

% C(q) = (q1 q + q2) / q

% Right motor controller
% from root locus C(s) = (s1 s + s2) / s
s1 = 100;
s2 = 22;
q1r = s1 + dt * s2 / 2;
q2r = -s1 + dt * s2 / 2;

% C(q) = (q1 q + q2) / q

% % --- SONAR DATA plot ---
% hold on;
% data = importdata('end_data/data_6.txt2f') + importdata('end_data/data_7.txt2f')+ importdata('end_data/data_8.txt2f') + importdata('end_data/data_9.txt2f') + importdata('end_data/data_10.txt2f');
% data = data / 5 ;
% sspeed = data(:, 2) * 0.846153846 / 100;
% plot(data(:, 1)/1000, sspeed);


modellc = modell * zpk([1 20], [1 0], 4.5);
modelrc = modelr * zpk([1 20], [1 0], 4);
