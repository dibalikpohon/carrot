-- A simple function that an argument
echo: func(text: str) -> int:
	return text
end

println("result: ", echo("hi there"))


-- Variable scoping
x: int = 100

-- A function that does not return any value, usually only has side effects
-- such as printing to screen or writing to file.
greet: func(my_name: str, your_name: str) -> void:
	println("Hello ", your_name, "!")
	println("My name is ", my_name)

	x: int = 10
	println(x) -- will print 10 instead of 100
end

greet("Mike", "Jim")


-- Nested function
outer: func() -> void:
	inner: func() -> void:
		println("from inner")
	end

	inner()
	println("from outer")
end

outer()

