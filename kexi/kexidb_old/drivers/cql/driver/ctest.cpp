#include <CqlSqlInclude.h>
#include <iostream.h>

int main()
{
	try
	{
		SqlHandle *h = new SqlHandle();
	
		try
		{
			h->connect("PUBLIC");
		}
		catch(CqlException &err)
		{
			cerr << err << endl;
		}
	
		try
		{
			Cursor *cur = h->declareCursor("SELECT * FROM fish;");
		
			try
			{
				cur->open();

				CqlString testFill1;
				CqlString testFill2;
				CqlString testFill3;
				bool isNull;
				try
				{
					cur->bindColumn(0, testFill1, isNull, false);
					cur->bindColumn(1, testFill2, isNull, false);
					cur->bindColumn(2, testFill3, isNull, false);
				}
				catch(CqlException &err)
				{
					cerr << err << endl;
				}

				cur->fetch();
				cout << testFill1.text() << endl;
				cout << testFill2.text() << endl;
				cout << testFill3.text() << endl;
			}
			catch(CqlException &err)
			{
				cerr << err << endl;
			}

		}
		catch(CqlException &err)
		{
			cerr << err << endl;
		}
	

	}
	catch(CqlException &err)
	{
		cerr << err << endl;
	}
	
}
