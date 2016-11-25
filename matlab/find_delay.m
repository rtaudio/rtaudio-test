A = e.timeData;
B = [zeros(1000,1) ;e.timeData(1:(end-1000))];

%B(11000) = 0.5;

%B(3000) = 0.4;

%A = A .* gausswin(size(A,1));
%B = B .* gausswin(size(B,1));

C_spec = fft(B) ./ (fft(A) + ones(size(A))*0.001);
C = ifft(C_spec);
figure;
 plot(abs(C_spec));
  set(gca, 'YScale', 'log');
 figure;
 plot(abs(C));
  set(gca, 'YScale', 'log');
  
return;

C_spec = C_spec(1:end/2);
C_spec = C_spec(1:end*0.8);

C_spec = C_spec ^ 2;
%flatness = nthroot(prod(C_spec),size(C_spec,1)) / ( sum(C_spec) / size(C_spec,1))

%C_spec(400:700) = C_spec(400:700) *1.1;

flatness = exp( 1/size(C_spec,1) * sum(log(C_spec))) / ( sum(C_spec) / size(C_spec,1))


C = abs(C);

[max_sig, delay] = max(C);


figure;
plot((C_spec));
 set(gca, 'YScale', 'log');
 grid on;
 
 
 
 
 delay
 Q
