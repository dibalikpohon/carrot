---------------------------
-- an example of variable
---------------------------
name as str = "John"
print(name)

---------------------------
-- uninitialized variable
---------------------------
age as int
print(age) -- should return 0

---------------------------
-- other variables
---------------------------
pi as float = 3.13

-- carrot performs type inference if we specfy "any" as the data type
some_object as any = 123
print(some_object)
