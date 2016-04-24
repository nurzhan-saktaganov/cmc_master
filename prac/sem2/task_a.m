function task_a()
	m = 100;
	n = m - 1;
	y = poisson(@f_a, @g_a, m);
	h = 1 / m;

	u = zeros(n, n);
	for i= 1 : n
    	for j= 1 : n
        	u(i, j) = u_a(i * h, j * h);
    	end
	end

	subplot(1, 2, 1);
	surf(u);
	title('analytical');
	subplot(1, 2, 2);
	surf(y);
	title('numerical')
end;