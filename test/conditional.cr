if 2 > 1:
	println("Print me")
end


if 1 + 1 > 4:
	println("It is true that 1 + 1 > 4")
elif 1 + 1 < 2:
	println("It is true that 1 + 1 < 2")
elif 1 - 1 == 1:
	println("It is true that 1 - 1 == 1")
else:
	-- This one will be printed
	println("No, trust nobody")
end


x: int = 1
if (x == 1) || (x == 2):
	println("Should be printed as well test")
end

