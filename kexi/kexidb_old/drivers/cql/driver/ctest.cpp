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