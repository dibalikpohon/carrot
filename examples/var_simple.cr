-- Literals
println("Purple parrot eating carrot ", 123, " ", 1.23) 

-- An example of variable
name: str = "John" 
println("My name is ", name)

-- You can get the type of variable using type() function
t: str = type(name)
println("The type of name is ", t)

-- Another variable
pi: float = 3.14 
println("pi: ", pi) 

-- Lists
fruits: list = ["banana", "orange", "strawberry"]
println("Fruits: ", fruits) 

nested_list: list = [1234, [123, ["abc", 1.23, [123, name], "def"]]]
println("I can understand nested list: ", nested_list)

-- Let's try printing undeclared variable
print(new_var)
