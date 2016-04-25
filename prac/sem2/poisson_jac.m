function Y = poisson_jac(f, g, m, eps)

    h = 1.0 / m;

    % constructing the matrices
    n = m - 1;
    Y = zeros(n);
    Y_next = zeros(n);
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
    A(n, n) = -4.0;

    % go iterations
    j = 0;
    while 1
        j = j + 1;
        Y_next(1, :) = Y(1, :) * A + Y(2, :);
        for i = 2 : n - 1
            Y_next(i, :) = Y(i - 1, :) + ...
                    Y(i, :) * A + Y(i + 1, :);
        end
        Y_next(n, :) = Y(n - 1, :) + Y(n, :) * A;

        Y_next = (Y_next - F + 4.0 * Y) / 4.0;
        if max(max(abs(Y_next - Y))) < eps
           break;
        end
        Y = Y_next;
    end

    for i = 1 : n
        for j = 1 : n
            Y(i, j) = Y(i, j) + g(i * h, j * h);
        end;
    end;
end