// Bug 1 - Wrong result.
// In C++ it works that way:
//#include <iostream>
//void foo(int a, int b, int c, int d, int e) {
//    cout << a << " " << b << " " << c << " " << d << " " << e << endl;
//}
//int main( int, char ** )
//{
//    int a=503;
//    foo(a, ++a, a, a++, a);
//    cout << "Done: a=" << a << endl;
//}

// The output is: 505 505 504 503 503
//                Done: a=505

// Problems: KoScript has a right recursive parser, so the evaluation is from
//           left to right (as opposed to right to left in C++). What do we
//           expect? 505 505 504 503 503 as in C++? Or rather 503 504 504 504
//           505?

main {
    a = 503;
    println(a, ++a, a, a++, a);
}
