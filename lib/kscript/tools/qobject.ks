signature( in params )
{
	sig = "";

	foreach( p, params )
	{
		if ( p == "bool" )
		{
			sig += "b";
		}
		else if ( p == "QString" )
		{
			sig += "s";
		}
		else if ( p == "double" )
		{
			sig += "f";
		}
		else if ( p == "int" )
		{
			sig += "i";
		}
		else if ( p == "QRect" )
		{
			sig += "Sqt:QRect;";
		}
		else if ( p == "QSize" )
		{
			sig += "Sqt:QSize;";
		}
		else if ( p == "QPoint" )
		{
			sig += "Sqt:QPoint;";
		}
	}

	return sig;
}

main
{
	// pb_signals = { ( "toggled", [ "bool", [ "bool" ] ] ),
	//	       ( "pressed", [ "", [] ] ) };
	pb_signals = { };
	pb_methods = { ( "setOn", [ "void", [ "bool" ] ] ),
		       ( "toggle", [ "void", [ ] ] ),
		       ( "setDefault", [ "void", [ "bool" ] ] ),
		       ( "isDefault", [ "bool", [ ] ] ),
		       ( "setAutoDefault", [ "void", [ "bool" ] ] ),
		       ( "isAutoDefault", [ "bool", [ ] ] ),
		       ( "setIsMenuButton", [ "void", [ "bool" ] ] ),
		       ( "isMenuButton", [ "bool", [ ] ] ) };
	pb = [ "QButton", pb_methods, pb_signals ];
	// 	classes = { ( "QPushButton", pb ) };

	button_methods = { ( "toggle", [ "void", [] ] ),
			   ( "animateClick", [ "void", [] ] ) };
	button_signals = { ( "pressed", [ "", [] ] ),
			   ( "clicked", [ "", [] ] ),
			   ( "released", [ "", [] ] ),
			   ( "toggled", [ "bool", [ "bool" ] ] ),
			   ( "stateChanged", [ "int", [ "int" ] ] ) };
	button = [ "QWidget", button_methods, button_signals ];
        // classes = { ( "QButton", button ) };

	grpbox_methods = { };
	grpbox_signals = { };
	grpbox = [ "QFrame", grpbox_methods, grpbox_signals ];
        classes = { ( "QGroupBox", grpbox ) };


	print( "namespace KSQt" );
	print( "{" );

	foreach( qname, cdef, classes )
	{
	    cname = qname.mid( 1 );
	    sqname = cdef[0];
	    scname = sqname.mid( 1 );

	    print( "    class Class%1 : public KSQt::Class".arg( cname ) );
	    print( "    {" );
	    print( "    public:" );
	    print( "        Class%1( KSModule*, const char* name = \"%2\" );".arg( cname ).arg( qname ) );
	    print();
	    print( "    protected:" );
	    print( "        virtual KSScriptObject* createObject( KSClass* c );" );
	    print( "    };" );
	    print();
	    print( "    class %1 : public KSQt::%2".arg( cname ).arg( scname ) );
	    print( "    {" );
	    print( "    public:" );
	    print( "        %1( KSClass* );".arg( cname ) );
	    print();
	    print( "        bool ks%1( KSContext& );".arg( cname ) );

	    methods = cdef[1];
	    foreach( method, mdef, methods )
	    {
		print( "        bool ks%1_%2( KSContext& );".arg( cname ).arg( method ) );
	    }
	
	    print();
	    print( "        static %1* convert( KSValue* v ) { return (%2*) ((KSQt::Object*)v->objectValue())->object(); }".arg( qname ).arg( qname ) );

	    signals = cdef[2];
	    print();
	    foreach( sig, sdef, signals )
	    {
		print( "        bool ks%1_%2( KSContext& );".arg( cname ).arg( sig ) );
	    }
	    if ( !signals.isEmpty() )
	    {
		print();
		print( "        void setObject( QObject* obj );" );
	    }

	    print( "    };" );
	}
    	
	print( "};" );

	// =================================================
	// Generate implementation
	// =================================================

	print( "--------------------------------------------");

	foreach( qname, cdef, classes )
	{
	    cname = qname.mid( 1 );
	    sqname = cdef[0];
	    scname = sqname.mid( 1 );

	    print( "// We want to access the signals" );
	    print( "#include <qobjectdefs.h>" );
	    print( "#ifdef signals" );
	    print( "#undef signals" );
	    print( "#define signals public" );
	    print( "#endif" );
	    print( "" );
	    print( "#include \"kscript_ext_%1.h\"".arg( qname.lower() ) );
	    print( "#include \"kscript_context.h\"" );
	    print( "#include \"kscript_util.h\"" );
	    print();
	    print( "#include <%1.h>".arg( qname.lower() ) );
	    print();
	    print( "KSQt::Class%1::Class%2( KSModule* m, const char* name ) : KSQt::Class( m, name )".arg( cname ).arg( cname ) );
	    print( "{" );
	    print( "    nameSpace()->insert( \"%1\", new KSValue( (KSBuiltinMethod)&KSQt::%2::ks%3 ) );".arg( qname ).arg( cname ).arg( cname ) );

	    methods = cdef[1];
	    foreach( method, mdef, methods )
	    {
		print( "    nameSpace()->insert( \"%1\", new KSValue( (KSBuiltinMethod)&KSQt::%2::ks%3_%4 ) );".arg( method ).arg( cname ).arg( cname ).arg( method ) );
	    }

	    signals = cdef[2];
	    print();
	    foreach( sig, sdef, signals )
	    {
		print( "    nameSpace()->insert( \"%1\", new KSValue( (KSBuiltinMethod)&KSQt::%2::ks%3_%4 ) );".arg( sig ).arg( cname ).arg( cname ).arg( sig ) );
		print( "    addQtSignal( \"%1\" );".arg( sig ) );
	    }
	    print();
	    print( "    KSValue* v = m->object( \"%1\" );".arg( sqname ) );
	    print( "    ASSERT( v );" );
	    print( "    v->ref();" );
	    print( "    setSuperClass( v );" );
	    print( "}" );
	    print();
	    print( "KSScriptObject* KSQt::Class%1::createObject( KSClass* c )".arg( cname ) );
	    print( "{" );
	    print( "    return new KSQt::%1( c );".arg( cname ) );
	    print( "}" );
	    print();
	    print( "// ------------------------------------------------------" );
	    print();
	    print( "KSQt::%1::%2( KSClass* c ) : KSQt::%3( c )".arg( cname ).arg( cname ).arg( scname ) );
	    print( "{" );
	    print( "}" );
	    print();
	    print( "bool KSQt::%1::ks%2( KSContext& context )".arg( cname ).arg( cname ) );
	    print( "{" );
	    print( "    if ( !checkDoubleConstructor( context, \"%1\" ) )".arg( qname ) );
	    print( "        return false;" );
	    print();
	    print( "    QValueList<KSValue::Ptr>& args = context.value()->listValue();" );
	    print();
	    print( "    if ( !KSUtil::checkArgs( context, args, \"|Oqt:QWidget;|s\", \"%1\", TRUE ) )".arg( qname ) );
	    print( "        return FALSE;" );
	    print();
	    print( "    QWidget* parent = 0;" );
	    print( "    QString name;" );
	    print();
	    print( "    if ( args.count() >= 1 )" );
	    print( "        parent = KSQt::%1::convert( args[0] );".arg( cname ) );
	    print( "    if ( args.count() >= 2 )" );
	    print( "        name = args[1]->stringValue();" );
	    print();
	    print( "    setObject( new %1( parent, name ) );".arg( qname ) );
	    print();
	    print( "    return true;" );
	    print( "}" );
	    print();

	    if ( !signals.isEmpty() )
	    {
		print( "void KSQt::%1::setObject( QObject* obj )".arg( cname ) );
	    	print( "{" );
	    	print( "    if ( obj )" );
		print( "    {" );
	        foreach( sig, sdef, signals )
	    	{
		    print( "        KSQt::Callback::self()->connect( obj, SIGNAL( %1(%2) ),".arg( sig ).arg( sdef[0] ) );
		    print( "              SLOT( %1(%2) ), this, \"%3\" );".arg( sig ).arg( sdef[0] ).arg( sig ) );
		}
		print( "    }" );
	    	print( "    KSQt::%1::setObject( obj );".arg( scname ) );
	    	print( "}" );
	    	print();
	    }

	    foreach( sig, sdef, signals )
	    {
		print( "bool KSQt::%1::ks%2_%3( KSContext& context )".arg( cname ).arg( cname ).arg( sig ) );
		print( "{" );
		print( "    if ( !checkLive( context, \"%1::%2\" ) )".arg( qname ).arg( sig ) );
		print( "        return false;" );
		print();
		if ( sdef[1].isEmpty() )
		{
		    print( "    if ( !KSUtil::checkArgumentsCount( context, 0, \"%1::%2\" ) )".arg( qname ).arg( sig ) );
		    print( "        return false;" );
		    print();
		    print( "    %1* w = (%2*)object();".arg( qname ).arg( qname ) );
		    print( "    w->%1();".arg( sig ) );
		}
		else
		{
		    print( "    QValueList<KSValue::Ptr>& args = context.value()->listValue();" );
		    print();
		    print( "    if ( !KSUtil::checkArgs( context, args, \"%1\", \"%2::%3\", TRUE ) )".arg( signature( sdef[1] ) ).arg( qname ).arg( sig ) );
		    print( "        return FALSE;" );
		    print();
		    print( "    %1* w = (%2*)object();".arg( qname ).arg( qname ) );
		    print( "    w->%1( ".arg( sig ) );
		    i = 0;
		    foreach( param, sdef[1] )
		    {
			str = "        args[%1]->".arg( i );
			if ( param == "bool" )
			{
			    str += "boolValue()";
			}
			else if ( param == "int" )
			{
			    str += "intValue()";
			}
			else if ( param == "double" )
			{
			    str += "doubleValue()";
			}
			else if ( param == "QString" )
			{
			    str += "stringValue()";
			}
			else if ( param == "QRect" )
			{
			    str = "    KSQt::Rect::convert( context, args[%i] )".arg( i );
			}
			else if ( param == "QSize" )
			{
			    str = "    KSQt::Size::convert( context, args[%i] )".arg( i );
			}
			else if ( param == "QPoint" )
			{
			    str = "    KSQt::Point::convert( context, args[%i] )".arg( i );
			}

			i += 1;
			if ( i < sdef[1].length() )
			{
			    str += ",";
			}
			print( str );
		    }
		    print( "    );" );		
		}
	        print();
		print( "    context.setValue( 0 );" );
		print();
		print( "    return TRUE;" );
		print( "}" );
		print();
	    }


	    foreach( method, mdef, methods )
	    {
		print( "bool KSQt::%1::ks%2_%3( KSContext& context )".arg( cname ).arg( cname ).arg( method ) );
		print( "{" );
		print( "    if ( !checkLive( context, \"%1::%2\" ) )".arg( qname ).arg( method ) );
		print( "        return false;" );
		print();
		if ( mdef[1].isEmpty() )
		{
		    print( "    if ( !KSUtil::checkArgumentsCount( context, 0, \"%1::%2\" ) )".arg( qname ).arg( method ) );
		    print( "        return false;" );
		}
		else
		{
		    print( "    QValueList<KSValue::Ptr>& args = context.value()->listValue();" );
		    print();
		    print( "    if ( !KSUtil::checkArgs( context, args, \"%1\", \"%2::%3\", TRUE ) )".arg( signature( mdef[1] ) ).arg( qname ).arg( method ) );
		    print( "        return FALSE;" );
		}
		print();
		
		print( "    %1* w = (%2*)object();".arg( qname ).arg( qname ) );
		ende = "";
		if ( mdef[0] == "void" )
		{
		    ende = "    );";
		}
		else
		{
		    ende = "    ) ) );";
		    tmp = "";
		    if ( mdef[0] == "QRect" )
		    {
			tmp = "context.setValue( KSQt::Rect::convert( context, ";
		    }
		    else if ( mdef[0] == "QPoint" )
		    {
			tmp = "context.setValue( KSQt::Point::convert( context, ";
		    }
		    else if ( mdef[0] == "QSize" )
		    {
			tmp = "context.setValue( KSQt::Size::convert( context, ";
		    }
		    else if ( mdef[0] == "int" || mdef[0] == "bool" || mdef[0] == "double" || mdef[0] == "string" )
		    {
		        tmp = "    context.setValue( new KSValue( ";
		    }
		    else
		    {
			// Error
			0 = 0;
		    }
		    print( tmp );
		}
		print( "    w->%1( ".arg( method ) );
	    	i = 0;
		foreach( param, mdef[1] )
		{
			str = "        args[%1]->".arg( i );
			if ( param == "bool" )
			{
			    str += "boolValue()";
			}
			else if ( param == "int" )
			{
			    str += "intValue()";
			}
			else if ( param == "double" )
			{
			    str += "doubleValue()";
			}
			else if ( param == "QString" )
			{
			    str += "stringValue()";
			}
			else if ( param == "QRect" )
			{
			    str = "    KSQt::Rect::convert( context, args[%i] )".arg( i );
			}
			else if ( param == "QSize" )
			{
			    str = "    KSQt::Size::convert( context, args[%i] )".arg( i );
			}
			else if ( param == "QPoint" )
			{
			    str = "    KSQt::Point::convert( context, args[%i] )".arg( i );
			}

			i += 1;
			if ( i < mdef[1].length() )
			{
			    str += ",";
			}
			print( str );
		}
		print( ende );
		if ( mdef[0] == "void" )
		{
	            print();
		    print( "    context.setValue( 0 );" );
		}
		print();
		print( "    return TRUE;" );
		print( "}" );
		print();
	    }
	}
}
