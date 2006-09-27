import krosskspreadcore
doc = krosskspreadcore.get("KSpreadDocument")

doc.openUrl("/home/ak016/ccc/invoicetemplate.ods")

#for sheetname in doc.sheetNames():
#	sheet = doc.sheetByName( sheetname )
#	cell = sheet.firstCell()
#	while cell:
#		print "Cell col=%s row=%s value=%s" % (cell.column(),cell.row(),cell.value())
#		row = cell.row()
#		cell = cell.nextCell()


sheet1 = doc.sheetByName( "Sheet1" )
sheet1.cell(0,7).setText("Joe User")
sheet1.cell(0,8).setText("Userstreet. 1")
sheet1.cell(0,9).setText("Testcasecity")

items=[[3, "KDE T-Shirt", 15.00],
       [1, "KDE pin",      1.00]]

row = 17
for item in items:
	sheet1.cell(1,row).setText(item[0])
	sheet1.cell(2,row).setText(item[1])
	sheet1.cell(5,row).setText(item[2])
	
	row += 1

for i in xrange(row, 32):
	sheet1.cell(6, i).setText("")

#print doc.saveNativeXML()

#doc.exp0rt("/home/ak016/ccc/test.html")
doc.saveUrl("/home/ak016/ccc/invoice.ods")
