n: int = 10

iter range(1, n) as j:
	iter range(1, n - j + 1) as i:
		print(n - j - i + 1, " ")
	end
	iter range(2 * (j - 1)) as k:
		print(" ")
	end
	print("| ")
	iter range(2 * (j - 1)) as k:
		print(" ")
	end
	iter range(1, n - j + 1) as i:
		print(i, " ")
	end
	println()
end

iter range(1, n * 4 - 2) as j:
	print("-")
end
println()

iter range(1, n) as j:
	iter range(1, j + 1) as i:
		print(j - i + 1, " ")
	end
	iter range(2 * (n - j - 1)) as k:
		print(" ")
	end
	print("| ")
	iter range(2*(n - j - 1)) as k:
		print(" ")
	end
	iter range(1, j + 1) as i:
		print(i, " ")
	end
	println()
end
