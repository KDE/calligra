main
{
	size_vars = { ( "width", [ "IntType", "width", "setWidth" ] ),
	              ( "height", [ "IntType", "height", "setHeight" ] ) };
        size_methods = { ( "isNull", [ "BoolType", [] ] ),
			 ( "isValid", [ "BoolType", [] ] ),
			 ( "isEmpty", [ "BoolType", [] ] ),
			 ( "transpose", [ "void", [] ] ),
			 ( "expandedTo", [ "QSize", [ "QSize" ] ] ),
			 ( "boundedTo", [ "QSize", [ "QSize" ] ] ) };
	size = { ( "vars", size_vars ), ( "methods", size_methods ),
		  ( "constructor", [ [], [ "IntType", "IntType" ] ] ) };

	point_vars = { ( "x", [ "IntType", "x", "setX" ] ),
	              ( "y", [ "IntType", "y", "setY" ] ) };
        point_methods = { ( "isNull", [ "BoolType", [] ] ) };
	point = { ( "vars", point_vars ), ( "methods", point_methods ),
		  ( "constructor", [ [], [ "IntType", "IntType" ] ] ) };

	rect_vars = { ( "left", [ "IntType", "left", "setLeft" ] ),
	              ( "right", [ "IntType", "right", "setRight" ] ),
		      ( "size", [ "QSize", "size", "setSize" ] ) };
        rect_methods = { ( "isNull", [ "BoolType", [] ] ),
			 ( "setRect", [ "void", [ "IntType", "IntType", "IntType", "IntType" ] ] ),
			 ( "setCoords", [ "void", [ "IntType", "IntType", "IntType", "IntType" ] ] ),
			 ( "moveBy", [ "void", [ "IntType", "IntType" ] ] ),
			 ( "moveCenter", [ "void", [ "QPoint" ] ] ),
			 ( "moveTopRight", [ "void", [ "QPoint" ] ] ),
			 ( "moveBottomRight", [ "void", [ "QPoint" ] ] ),
			 ( "moveBottomLeft", [ "void", [ "QPoint" ] ] ),
			 ( "moveTopLeft", [ "void", [ "QPoint" ] ] ),
			 ( "bottomLeft", [ "QPoint", [] ] ),
			 ( "bottomRight", [ "QPoint", [] ] ),
			 ( "topLeft", [ "QPoint", [] ] ),
			 ( "topRight", [ "QPoint", [] ] ) };
	rect = { ( "vars", rect_vars ), ( "methods", rect_methods ),
		  ( "constructor", [ [],
				     [ "IntType", "IntType", "IntType", "IntType" ],
				     [ "QPoint", "QPoint" ],
				     [ "QPoint", "QSize" ] ] ) };

	classes = { ( "QSize", size ),
		    ( "QPoint", point ),
		    ( "QRect", rect ) };

	print( classes );

	print( "#ifndef KSCRIPT_EXT_QSTRUCTS_H" );
	print( "#define KSCRIPT_EXT_QSTRUCTS_H" );
	print( "" );
	print( "#include \"kscript_struct.h\"" );
	print( "" );
	print( "#include <qpoint.h>" );
	print( "" );
	print( "namespace KSQt" );
	print( "{" );

	foreach( cname, def, classes )
	{
	    qname = cname.mid( 1 );

	    print( "    class %1 : public KSBuiltinStructClass".arg( qname ) );
	    print( "    {" );
    	    print( "    public:" );
	    print( "        %1( KSModule* module, const QString& name );".arg( qname ) );
	    print();
	    print( "        bool constructor( KSContext& c );" );
	    print( "        bool destructor( void* object );" );
	    print( "        KSStruct* clone( KSBuiltinStruct* );" );
	    print();
	    print( "        static %1 convert( KSContext& context, const KSValue::Ptr& value );".arg( cname ) );
	    print( "        static KSValue::Ptr convert( KSContext& context, const %1& value );".arg( cname ) );
	    print();	
	    print( "    protected:" );
	    print( "        KSValue::Ptr property( KSContext& context, void* object, const QString& name );" );
	    print( "        bool setProperty( KSContext& context, void* object, const QString& name, const KSValue::Ptr value );" );
    	    print( "    };" );
	    print();
	    print();
	}

	print( "};" );
	print( "" );
	print( "#endif" );

	print( "---------------------------------------------------------------" );

	foreach( cname, def, classes )
	{
	    qname = cname.mid( 1 );

	    methods = def{ "methods" };
	    foreach( mname, mdef, methods )
	    {
		print( "static bool %1_%2( void* object, KSContext& context, const QValueList<KSValue::Ptr>& args )".arg( qname ).arg( mname ) );
		print( "{" );
		print( "    %1* value = (%2*)object;".arg( cname ).arg(cname ) );
		list = [];
		for( a = 1; a < mdef.length(); ++a )
		{
			list += mdef[a];
			str = "";
			if ( mdef.length() > 2 )
			{
			    if ( a == 1 )
			    {
			        print( "    if ( args.count() == %1 )".arg( list.length() ) );
			    }
			    else
			    {
			        print( "    else if ( args.count() == %1 )".arg( list.length() ) );
			    }
			    print( "    {" );
			    str += "    ";
			}
			if ( mdef[0] == "BoolType" || mdef[0] == "IntType" || mdef[0] == "StringType" ||
			     mdef[0] == "CharType" || mdef[0] == "DoubleType" )
			{
			    str += "    context.setValue( ";
			    str += " new KSValue( value->%1(".arg( mname );
			}
			else if ( mdef[0] == "void" )
			{
			    str += "    value->%1(".arg( mname );
			}
			else
			{
			    str += "    context.setValue( ";
			    str += " KSQt::%1::convert( context, value->%2(".arg( mdef[0] ).arg( mname );
			}
			for( b = 0; b < list.length(); ++b )
			{
			    if ( list[b] == "BoolType" )
			    {
				str += " args[%1]->boolValue()".arg( b );
			    }
			    else if ( list[b] == "IntType" )
			    {
				str += " args[%1]->intValue()".arg( b );
			    }
			    else if ( list[b] == "DoubleType" )
			    {
				str += " args[%1]->doubleValue()".arg( b );
			    }
			    else if ( list[b] == "StringType" )
			    {
				str += " args[%1]->stringValue()".arg( b );
			    }
			    else if ( list[b] == "CharType" )
			    {
				str += " args[%1]->charValue()".arg( b );
			    }
			    else
			    {
				str += " KSQt::%1::convert( context, args[%2] )".arg( list[b].mid(1) ).arg( b );
			    }
			
			    if ( b + 1 < list.length() )
			    {
				str += ",";
			    }
			}
			if ( mdef[0] == "void" )
			{
			    str += " );";
			}
			else
			{
			    str += " ) ) );";
			}
			print( str );

			if ( mdef[0] == "void" )
			{
			    print( "    context.setValue( 0 );" );
			}

			if ( mdef.length() > 2 )
			{
				print( "    }" );
			}
		}
	        print( "    return TRUE;" );
		print( "}" );
		print();
	    }

	    print( "%1 KSQt::%2::convert( KSContext&, const KSValue::Ptr& v )". arg( cname ).arg( qname ) );
	    print( "{" );
	    print( "    return *((const %1*)v->structValue()->object());".arg( cname ) );
	    print( "}" );
	    print();
	    print( "KSValue::Ptr KSQt::%1::convert( KSContext& context, const %2& value )". arg( qname ).arg( cname ) );
	    print( "{" );
	    print( "    KSValue::Ptr s = context.interpreter()->module( \"qt\" )->member( context, \"%1\" );".arg( cname ) );
	    print( "    ASSERT( !s.isNull() );" );
	    print();
	    print( "    return new KSValue( new KSBuiltinStruct( s->structClassValue(), new %1( value ) ) );".arg( cname ) );
	    print( "}" );
	    print();

	    print( "KSQt::%1::%2( KSModule* module, const QString& name )".arg( qname ).arg( qname ) );
	    print( "    : KSBuiltinStructClass( module, name )" );
	    print( "{" );

	    methods = def{ "methods" };
	    foreach( mname, mdef, methods )
	    {
		    sig = "";
		    for( a = 1; a < mdef.length(); ++a )
		    {
			for( b = 0; b < mdef[a].length(); ++b )
			{
			    if ( mdef[a][b] == "BoolType" )
			    {
				sig += "b";
			    }
			    else if ( mdef[a][b] == "IntType" )
			    {
				sig += "i";
			    }
			    else if ( mdef[a][b] == "DoubleType" )
			    {
				sig += "f";
			    }
			    else if ( mdef[a][b] == "StringType" )
			    {
				sig += "s";
			    }
			    else if ( mdef[a][b] == "CharType" )
			    {
				sig += "c";
			    }
			    else
			    {
				sig += "Sqt:" + mdef[a][b] + ";";
			    }
			}

			if ( a + 1 < mdef.length() )
			{
			    sig += "|";
			}
		    }
		    print( "    addMethod( \"%1\", %2_%3, \"%4\" );".arg( mname ).arg( qname ).arg( mname ).arg( sig ) );
	    }

	    vars = def{ "vars" };
	    foreach( vname, vdef, vars )
	    {
		print( "    addVariable( \"%1\" );".arg( vname ) );
	    }
	    print( "}" );
	    print();

	    print( "bool KSQt::%1::constructor( KSContext& context )".arg( qname ) );
	    print( "{" );
	    print( "    QValueList<KSValue::Ptr>& args = context.value()->listValue();" );
	    print();
	    cons = def{ "constructor" };
	    first = TRUE;
	    foreach( list, cons )
	    {
		sig = "";
		for( b = 0; b < list.length(); ++b )
		{
		    if ( list[b] == "BoolType" )
		    {
			sig += "b";
		    }
		    else if ( list[b] == "IntType" )
		    {
			sig += "i";
		    }
		    else if ( list[b] == "DoubleType" )
		    {
			sig += "f";
		    }
		    else if ( list[b] == "StringType" )
		    {
			sig += "s";
		    }
		    else if ( list[b] == "CharType" )
		    {
			sig += "c";
		    }
		    else
		    {
			sig += "Sqt:" + list[b] + ";";
		    }
		}
		if ( first )
		{
		    print( "    if ( KSUtil::checkArgs( context, \"%1\", \"%2\", FALSE ) )".arg( sig ).arg( cname ) );
		    first = FALSE;
		}
		else
		{
		    print( "    else if ( KSUtil::checkArgs( context, \"%1\", \"%2\", FALSE ) )".arg( sig ).arg( cname ) );
		}

		str= "";
		for( b = 0; b < list.length(); ++b )
		{
		    if ( list[b] == "BoolType" )
		    {
			str += " args[%1]->boolValue()".arg( b );
		    }
		    else if ( list[b] == "IntType" )
		    {
			str += " args[%1]->intValue()".arg( b );
		    }
		    else if ( list[b] == "DoubleType" )
		    {
			str += " args[%1]->doubleValue()".arg( b );
		    }
		    else if ( list[b] == "StringType" )
		    {
			str += " args[%1]->stringValue()".arg( b );
		    }
		    else if ( list[b] == "CharType" )
		    {
			str += " args[%1]->charValue()".arg( b );
		    }
		    else
		    {
			str += " KSQt::%1::convert( context, args[%2] )".arg( list[b].mid(1) ).arg( b );
		    }

		    if ( b + 1 < list.length() )
		    {
			str += ",";
		    }
		}

		print( "        context.setValue( new KSValue( new KSBuiltinStruct( this, new %1(%2) ) ) );".arg( cname ).arg( str ) );
	    }
	    print( "    else" );
	    print( "        KSUtil::argumentsMismatchError( context, \"%1\" );".arg( cname ) );
	    print();
	    print( "    return TRUE;" );
	    print( "}" );
	    print();
	    print( "bool KSQt::%1::destructor( void* object )".arg( qname ) );
	    print( "{" );
	    print( "    delete ( (%1*)object );".arg( cname ) );
	    print();
	    print( "    return TRUE;" );
	    print( "}" );
	    print();
	    print( "KSStruct* KSQt::%1::clone( KSBuiltinStruct* s )".arg( qname ) );
	    print( "{" );
	    print( "    return new KSBuiltinStruct( this, new %1( *((%2*)s->object()) ) );".arg( cname ).arg( cname ) );
	    print( "}" );
	    print();
	    print( "KSValue::Ptr KSQt::%1::property( KSContext& context, void* object, const QString& name )".arg( qname ) );
	    print( "{" );
	    print( "    %1* value = (%2*)object;".arg( cname ).arg( cname ) );
	    print();
	    vars = def{ "vars" };
	    foreach( vname, vdef, vars )
	    {
		    print( "    if ( name == \"%1\" )".arg( vname ) );
		    if ( vdef[0] == "BoolType" || vdef[0] == "IntType" || vdef[0] == "StringType" ||
			 vdef[0] == "CharType" || vdef[0] == "DoubleType" )
		    {
		        print( "        return new KSValue( value->%1() );".arg( vdef[1] ) );
		    }
		    else
		    {
			print( "        return KSQt::%1::convert( context, value->%2() );".arg( vdef[0].mid(1) ).arg( vdef[1] ) );
		    }
	    }
	    print();
	    print( "    return 0;" );
	    print( "}" );
	    print();

	    print( "bool KSQt::%1::setProperty( KSContext& context, void* object, const QString& name, const KSValue::Ptr value )".arg( qname ) );
	    print( "{" );
	    print( "    %1* v = (%2*)object;".arg( cname ).arg( cname ) );
	    print();
	    foreach( vname, vdef, vars )
	    {
		type = "";
		if ( vdef[0] == "BoolType" )
		{
		    type = "bool";
		}
		else if ( vdef[0] == "IntType" )
		{
		    type = "int";
		}
		else if ( vdef[0] == "DoubleType" )
		{
		    type= "double";
		}
		else if ( vdef[0] == "StringType" )
		{
		    type = "string";
		}
		else if ( vdef[0] == "CharType" )
		{
		    type = "char";
		}
		else
		{
		    type = "struct";
		}

	        print( "    if ( name == \"%1\" )".arg( vname ) );
		print( "    {" );
		if ( type != "struct" )
		{
		    print( "        if ( !KSUtil::checkType( context, value, KSValue::%1 ) )".arg( vdef[0] ) );
		    print( "            return FALSE;" );
		    print( "        v->%1( value->%2Value() );".arg( vdef[2] ).arg( type ) );
		}
		else
		{
		    print( "        if ( !KSUtil::checkType( context, value, KSValue::StructType ) )" );
		    print( "            return FALSE;" );
		    print( "        v->%1( KSQt::%2:convert( context, value ) );".arg( vdef[2] ).arg( vdef[0].mid(1) ) );
		}
		print( "        return TRUE;" );
		print( "    }" );
	    }
	    print();
	    print( "    return FALSE;" );
	    print( "}" );
	    print();
	}
}
