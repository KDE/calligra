// Test 2 - Some messing with plain vaiables

main {
    println("Test 2");
    a = 2;
    b = 3;
    d = c = a * -b;
    print("a: '", a, "' b: '", b, "' c: '", c, "' d: '", d, "'\n");
    a = 42.42;
    b = 3.14159;
    d = c = a * -b;
    print("a: '", a, "' b: '", b, "' c: '", c, "' d: '", d, "'\n");
    a = "Test";
    b = "This";
    c = a + " " + b;
    print("a: '", a, "' b: '", b, "' c: '", c, "'\n");
    a = true;
    b = !a;
    c = !(!TRUE);
    print("a: '", a, "' b: '", b, "' c: '", c, "'\n");
    print("Done.");
}
