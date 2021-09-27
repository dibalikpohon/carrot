-- An example of variable
name as str = "John"
println(name)

-- You can get the type of variable using type() function
println("type of name: ", type(name))

-- Uninitialized variable
age as int
println(age) -- should print 0

-- Another variable
pi as float = 3.14
println(pi)

-- Carrot performs type inference if we specfy "any" as the data type
some_object as any = 123
println(some_object)

-- However, unitialized variable without initialization will be null-valued
another_object as any
println(another_object)

