def findWorkbook( name ):
	raise notImplemented

def findWorksheet( workbook, name ):
	raise notImplemented

class ksRange:
	def init1( self, table, string ):
		ret = parseRange( _document_id, table, string )
		self.__dict__["table"] = ret[0]
		self.__dict__["column1"] = ret[1]
		self.__dict__["row1"] = ret[2]
		self.__dict__["column2"] = ret[3]
		self.__dict__["row2"] = ret[4]

	def init2( self, table, column, row ):
		self.__dict__["table"] = table
		self.__dict__["column1"] = column
		self.__dict__["row1"] = row
		self.__dict__["column2"] = column
		self.__dict__["row2"] = row

	def init4( self, table, column1, row1, column2, row2 ):
		self.__dict__["table"] = table
		self.__dict__["column1"] = column1
		self.__dict__["row1"] = row1
		self.__dict__["column2"] = column2
		self.__dict__["row2"] = row2

	def __getattr__( self, key ):
		if key == "Value":
			if column1 == column2 and row1 == row2:
				return cell( _document_id, self.table, self.column1, self.row1 )
			raise valueOfRangeRequested
		raise unknownKey

	def __setattr__( self, key, value ):
		print "######## Setting"
		print key
		print value
		if key == "Value":
			for x in range( self.column1, self.column2 + 1 ):
				for y in range( self.row1, self.row2 + 1 ):
					setCell( _document_id, self.table, x, y, value )
			return
		raise unknownKey

class ksWorksheet:
	def __init__( self, table ):
		self.table = table
	def Range( self, arg1, *rest ):
		r = ksRange()
		if len(rest) == 0:
			r.init1( self.table, arg1 )
			return r
		elif len(rest) == 1:
			r.init2( self.table, arg1, rest[0] )
			return r
		elif len(rest) == 3:
			r.init4( self.table, arg1, rest[0], rest[1], rest[2] )
			return r
		raise wrongAmountOfArguments		

class ksWorkbook:
	def __init__( self, workbook ):
		self.workbook = workbook

	def Worksheets( self, name ):
		try:
			id = int(name)
		except ValueError:
			id = findWorksheet( self.workbook, name )
		if id == -1:
			raise unknownWorksheet
		return ksWorksheet( id )

class ksApplication:
	def Workbooks( self, name ):
		print "---------- Requesting Workbook --------"
		id = -1
		try:
			id = int(name)
		except ValueError:
			id = findWorkbook( name )
		if id == -1:
			raise unknownWorkbook
		return ksWorkbook( id )

unknownKey = ''
unknownWorkbook = ''
unknownWorksheet = ''
wrongAmountOfArguments = ''
valueOfRangeRequested = ''
notImplemented = ''

Application = ksApplication()

Workbooks = Application.Workbooks
# Worksheets = Workbooks(1).Worksheets
# Range = Worksheets(2).Range

# Range(1,1).Value = "Huhu"

print "=================== CLASSES ==============="
