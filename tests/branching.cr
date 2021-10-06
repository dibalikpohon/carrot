x: int = 100
y: int = 50

if x > y:
	println("foo") -- will be printed
else:
	println("bar")
end

if x < y:
	println("foo")
elif 1 + 1 == 3:
	println("bar")
elif 1 + 1 == 2:
	println("baz") -- will be printed
else:
	println("should not be executed")
end
