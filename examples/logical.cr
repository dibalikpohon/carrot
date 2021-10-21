println("boolean literal")
println(true)
println(false)

println()
println("logical comparison:")
println(1.0 == 1)    -- true
println(1 == 1.0)    -- true
println(2 == 1.0)    -- false
println(2 > 1.0)     -- true
println(4. > 5)      -- false
println(5. >= 5)     -- true
println(1 <= 1)      -- true
println(1 < 1.0)     -- false

println()
println("logical operator:")
println((1==1) || (2<3)) -- true
println((1==0) || (5<3)) -- false
println((1==0) || (1<3)) -- true
println((1==0) && (1<3)) -- false

println()
println("negation")
println(!1)
println(!false)
