main
{
	t = time("12:34:20");
        // Expected output: 12:34:20
	println( t );
	t2 = t + 2;
        // Expected output: 12:34:22
	println( t2 );
	t3 = t;
	t3 += 5;
        // Expected output: 12:34:25
	println( t3 );

	t = time("12:34:20");
	t2 = t - 40;
        // Expected output: 12:33:40
	println( t2 );
	t3 = t;
	t3 -= 50;
        // Expected output: 12:34:30
	println( t3 );

	t = time("12:34:20");
	t2 = t - time("12:32:10");
	// Expected output: 130
	println( t2 );
	t3 = t;
	t3 -= time("12:32:09");
	// Expected output: 131
	println( t3 );

	d = date("2000-8-10");
	// Expected output: 2000-08-10
	println( d );
	d2 = d + 3;
	// Expected output: 2000-08-13
	println( d2 );
	d3 = d;
	d3 += 8;
	// Expected output: 2000-08-18
	println( d3 );

	d = date("2000-8-10");
	// Expected output: 2000-08-10
	println( d );
	d2 = d - 30;
	// Expected output: 2000-07-11
	println( d2 );
	d3 = d;
	d3 -= 32;
	// Expected output: 2000-07-09
	println( d3 );

	d = date("2000-8-10");
	// Expected output: 2000-08-10
	println( d );
	d2 = d - date("2000-8-9");
	// Expected output: 1
	println( d2 );
	d3 = d;
	d3 -= date("2000-7-10");
	// Expected output: 31
	println( d3 );
}
