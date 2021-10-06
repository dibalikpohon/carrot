-- escape sequence
e: func() -> int:
    return "Returning from\na function e()"
end

println("===WITHOUT NEW")
println("LINE")
println("ESCAPE===")
println("")
println("===WITH NEW\nLINE\nESCAPE===")

println("\n\n====WITHOUT    TAB    ESCAPE====")
println("====WITH\tTAB\tESCAPE====")

println("\nis carriage\rreturn works?")

println("\nUsing a \\(backslash)")

println("\nSingle \'Quote\'")

println("\n")

println("Double \"Quote\"")

println("\n", e())
