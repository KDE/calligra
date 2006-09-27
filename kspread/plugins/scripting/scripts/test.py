import krosskspreadcore
doc = krosskspreadcore.get("KSpreadDocument")
print doc.sheetNames()

doc.openUrl("/home/ak016/ccc/test.ods")
for sheetname in doc.sheetNames():
	sheet = doc.sheetByName( sheetname )
	cell = sheet.firstCell()
	while cell:
		print "Cell col=%s row=%s value=%s" % (cell.column(),cell.row(),cell.value())
		row = cell.row()
		cell = cell.nextCell()
