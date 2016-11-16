A = music.timeData;
B = music_recorded.timeData;

%A = A .* gausswin(size(A,1));
%B = B .* gausswin(size(B,1));

C_spec = fft(B) ./ (fft(A) + ones(size(A))*0.01);
C = ifft(C_spec);
C = abs(C);

[max_sig, delay] = max(C);

C_rmp = C;
C_rmp(delay-32:delay+32) = C_rmp(delay+64)/2;
%C_rmp = medfilt1(C_rmp, 90);
C_rmp = conv(C_rmp, ones(64,1)/64);
[max_noise] = max(C_rmp);
 Q = 10*log10(max_sig / max_noise);

figure;
plot(C(1:3000));
 set(gca, 'YScale', 'log');
 grid on;
 
 
 delay
 Q
