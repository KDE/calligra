import kspread;

percent( in v, in p )
{
	return v * p / 100.0;
}

main
{
	kspread.percent = percent;
}
