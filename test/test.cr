-- variable declaration
x as int = 2
s as str = "Hello world"
fruits as list of str = ["orange", "strawberry", "mango"]

x as list = 
  [[1.0, 1.0],
   [0.1, 0.0],
   [0.0, 0.1],
   [0.0, 0.0]]

x = 1 + 1

-- function definition and function call
func tell(name:str | age:int):
	print(
		"Hi, my name is " << 
		 ". I am " << 
		 age <<
		 " years old."
	)
end

tell: "John" | 25

-- loops
for fruit in fruits:
	print("I have " << fruit)
end


for i in [1 ... 10, 2]:
  print(i)
