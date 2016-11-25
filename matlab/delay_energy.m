A = d048.timeData;

[max_sig, delay] = max(abs(A));

peak_energy = max_sig*max_sig;

noise_energy = (sum(A .^ 2)) / size(A,1) * 4;

peak_energy / noise_energy

10*log10(peak_energy / noise_energy)