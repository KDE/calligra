import libpymico
import CORBA
import KOM

class Base( KOM.Base_skel ):
	def __init__( self ):
		KOM.Base_skel.__init__( self )
		
		self.bIsClean = 0
		self.dctSignals = { }
		self.lstClients = [ ]
		self.lstRelatives = [ ]
		self.lstFilters = [ ]
		self.refCount = 0

	def __del__( self ):
		if ( self.bIsClean == 0 ):
			self.disconnectAll()
			self.bIsClean = 1

	def signal_impl( self, sig ):
		self.dctSignals[ sig ] = [ ]

	def connect( self, sig, obj, function ):
		try:
			dummy = self.dctSignals[ sig ]
		except KeyError, e:
			raise KOM.Base.UnknownSignal, KOM.Base.UnknownSignal()
		self.dctSignals[ sig ] = [ ( obj._ior(), function ), ] + self.dctSignals[ sig ]
		obj.connectNotify( self )

	def disconnect( self, sig, obj, function ):
		try:
			dummy = self.dctSignals[ sig ]
		except KeyError, e:
			raise KOM.Base.UnknownSignal, KOM.Base.UnknownSignal()
		try:
			self.dctSignals[ sig ].remove( ( obj._ior(), function ) )
		except ValueError, e:
			return
		obj.disconnectSignalNotify( self, sig, function )

	def disconnectSignalNotify( self, obj, sig, function ):
		try:
			self.lstClients.remove( obj._ior() )
		except ValueError, e:
			return

	def describeConnections( self ):
		lst = [ ]
		for sig in self.dctSignals.keys():
			for c in self.dctSignals[ sig ]:
				lst.append( KOM.Base.Connection( sig, KOM.Base( ior=c[0] ), c[1] ) )
		return lst	

	def connectNotify( self, obj ):
		self.lstClients.append( obj._ior() )

	def disconnectNotify( self, obj ):
		ior = obj._ior()
		# Remove him from our signals
		for sig in self.dctSignals.keys():
			delete = [ ]
			for c in self.dctSignales[ sig ]:
				if ( c[0] == ior ):
					delete.append( c )
					obj.disconnectSignalNotify( self, sig, c[1] )
			for d in delete:
				self.dctSignals[ sig ].remove( c )

		uninstallFilter( obj )

	def receiveASync( self, type, value ):
		self.receive( type, value )

	def receive( self, type, value ):
		print "========= TODO: call event filters here"
		self.event( type, value )

	def eventFilter( self, obj, type, value ):
		return 0

	def installFilter( self, obj, function, events, mode ):
		obj.connectNotify( self )
		self.lstFilters.append( ( obj, function, events, mode ) )

	def uninstallFilter( self, obj ):
		ior = obj._ior()
		for f in self.lstFilters:
			if ( f[0]._ior() == ior ):
				obj.disconnectFilterNotify( obj )
				self.lstFilters.remove( f )
				self.uninstallFilter( obj )
				return

	def disconnectFilterNotify( self, obj ):
		try:
			self.lstClients.remove( obj._ior() )
		except ValueError, e:
			return
		
	def describeEventFilters( self ):
		lst = [ ]
		for f in self.lstFilters:
                         lst.append( KOM.Base.EventFilter( KOM.Base( ior=f[0] ), f[1], f[2] ) )
		return lst

	def adopt( self, obj ):
		self.lstRelatives.append( obj._ior() )
		obj.adoptNotify( self )

	def adoptNotify( self, obj ):
		self.lstRelatives.append( obj._ior() )

	def leave( self, obj ):
		try:
			self.lstRelatives.remove( obj._ior() )
		except ValueError, e:
			return
		obj.leaveNotify( self )
		
	def leaveNotify( self, obj ):
		try:
			self.lstRelatives.remove( obj._ior() )
		except ValueError, e:
			return
		
	def describeRelatives( self ):
		lst = [ ]
		for i in self.lstRelatives:
			 lst.append( KOM.Base( ior=i ) )
		return lst

	def incRef( self ):
		 self.refCount = self.refCount + 1

	def decRef( self ):
		if self.refCount > 0:
			self.refCount = self.refCount - 1
		if self.refCount == 0:
			print "==== TODO: Kill this object"

	def refCount( self ):
		return self.refCount

	def destroy( self ):
		self.incRef()
		self.decRef()

	def disconnectObject( self, obj ):
		# He is no longer a relative
		ior = obj._ior()
		err = 0
		while( err == 0 ):
			try:
				self.lstRelatives.remove( ior )
				obj.leaveNotify( self )
			except ValueError, e:
				err = 1

		# If we are one of his clients =>
		# tell him that we wont be any longer
		err = 0
		while( err == 0 ):
			try:
				self.lstClients.remove( ior )
				obj.disconnectNotify( self )
			except ValueError, e:
				err = 1

		# Remove him from our signals
		for sig in self.dctSignals.keys():
			delete = [ ]
			for c in self.dctSignals[ sig ]:
				if ( c[0] == ior ):
					delete.append( c )
					obj.disconnectSignalNotify( self, sig, c[1] )
			for d in delete:
				self.dctSignals[ sig ].remove( c )
		
		uninstallFilter( obj )	
			
	def disconnectAll( self ):
		# Tell all of our clients that we wont be any longer
		for c in self.lstClients:
			b = KOM.Base( ior=c )
			b.disconnectNotify( self )
		self.lstClients = [ ]

		# We wont have any more relatives
		for r in self.lstRelatives:
			b = KOM.Base( ior=r )
			b.leaveNotify( self )
		self.lstRelatives = [ ]

		for sig in self.dctSignals.keys():
			for c in self.dctSignals[ sig ]:
				b = KOM.Base( ior=c[0] )
				b.disconnectSignalNotify( self, sig, c[1] )
		self.dctSignals = { }

		for f in self.lstFilters:
			b = KOM.Base( ior=f[0] )
			b.disconnectFilterNotify( self )
		self.lstFilters = [ ]

	def event( self, type, value ):
		print "===== Event not handled"
		return