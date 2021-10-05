fibo: func(n: int) -> int:
	if n <= 1:
	 	return n
	end
	--return fibo(n - 1)-- + fibo(n - 2)
	return fibo(n - 1) + fibo(n - 2)
end

println(fibo(3))
