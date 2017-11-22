function r = expmean(x,a)
    r = ones(1, length(x));
    r(1) = x(1);
    for i=2 : length(x)
        r(i) = r(i-1)*a + x(i)*(1-a);
    end
end

