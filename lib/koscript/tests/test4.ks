// Test 4 - Scoping tests

main {
    println("Test 4");
    foo = 1;
    {
        foo += 2;
        bar = -4;
        print("foo: ", foo, " bar: ", bar, "\n");
        {
            print("foo: ", foo, " bar: ", bar, "\n");
        }
    }
    print("foo: ", foo, " bar: ", bar);  // raises an exception as bar isn't visible
    print("Done.");
}
