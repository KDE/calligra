/**
 * specify the stuff ;)
 */

#ifndef SQLTYPES_H
#define SQLTYPES_H

extern int current;
extern QString ctoken;

struct dateType
{
		int year;
		int month;
		int day;
};

struct realType
{
	int integer;
	int fractional;
};

#endif

