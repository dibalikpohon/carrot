-- check equality of string
one: str = "Hello "

two: str = "World!"
three: str = "World!"

println("one and two is the same? ", one == two)

println("two and three is the same? ",three == two)

println("one and two is not the same? ", one != two)

-- adding between two strings
four: str = one + two
println(four)
-- print one and two to check if it still
-- accessible and don't cause and segfaults
println(one)
println(two)
println(four == "Hello World!")
