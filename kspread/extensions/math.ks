import kspread

percent( in v; in p )
{
	return v * p / 100.0
}

main
{
	print("!!!!!!! math.ks Extension !!!!!!!!!!")
	kspread.percent = percent
}
