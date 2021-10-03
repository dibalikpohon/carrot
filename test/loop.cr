-- simple iteration over a list
iter [1, 2, 3, 7, 8, 9] as num:
	println("The value is ", num)
end


-- If we want to access the index of the iterable, simply
-- use "@" followed by an identifier as a reference to the index
a_list = ["banana", 123, "apple"] 
iter a_list as item @idx:
	println("The item at index ", idx, " is ", item)
end
