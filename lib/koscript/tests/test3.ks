// Test 3 - Some messing with lists and dicts

const hallo = "Hallo Torben!";

main {
    println("Test 3");
    d = [ 100, 200, 300 ];
    print("d: ", d, "\n");
    e = { ( "Weis", 8 ), ( "Linzbach", 10 ) };
    print("e: ", e, "\n");
    x = d[ 1 ];
    d[ 5 ] = "Index";
    print("x: ", x, " d: ", d, "\n");
    n = e{ "Weis" };
    m = e{ "Sorg" };
    e{ "Weis" } = 9;
    e{ "Sorg" } = "Claudia";
    f = 'A';
    e{ "Weis" } += 3;
    print("f: ", f, " e: ", e, "\n");
    name = "Torben";
    g = name[ 2 ];
    name[0] = 't';
    print("name: ", name, " g: ", g, "\n");
    println( hallo );
    h = [ 42, 43, 44 ];
    i = h + d + h;
    print("i: '", i, "'\n");
    print("Done.");
}
