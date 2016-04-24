function Y = poisson(f, g, m)

    h = 1.0 / m;

    % constructing the matrices
    n = m - 1;
    Y = zeros(n);
    F = zeros(n);

    % filling the F
    for i = 1 : n
        x1 = i * h;
        for j = 1 : n
            x2 = j * h;
            F(i, j) = -(f(x1, x2) * h ^ 2 + ...
                g(x1 + h, x2) + g(x1 - h, x2) + ...
                g(x1, x2 + h) + g(x1, x2 - h) - ...
                4 * g(x1, x2));
        end
    end

    % preparing the A-matrix
    A = zeros(n);
    for i = 1: n - 1
        A(i, i + 1) = 1.0;
        A(i, i) = -4.0;
        A(i + 1, i) = 1.0;
    end;
    A(1, 1) = A(n, n) = -4.0;

    % sweep
    alpha = zeros(n);
    beta = zeros(n, 1);

    alphas = zeros(n, n, n);
    betas = zeros(n, n);

    for i = 1 : n
        K = pinv(A + alpha);
        alpha = -K;
        alphas(:, :, i) = alpha;
        beta = K * (F(i, :)' - beta); %'
        betas(:, i) = beta;
    end;

    Y(n, :) = beta';
    for i = n - 1 : -1 : 1
        Y(i, :) = (alphas(:, :, i) * Y(i + 1, :)' + betas(:, i))';
    end;

    for i = 1 : n
        for j = 1 : n
            Y(i, j) = Y(i, j) + g(i * h, j * h);
        end;
    end;
end;