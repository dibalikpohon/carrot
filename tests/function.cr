-- Simple function
add: func(x: int, y: int) -> int:
	return x + y
end

println(add(10, 3))

-- Nested function
outer: func() -> void:
	inner: func() -> void:
		println("inner")
	end

	inner()
	println("outer")
end

outer()
