/****************************************************************************
**
** DCOP Skeleton created by dcopidl2cpp from latexexportdia.kidl
**
** WARNING! All changes made in this file will be lost!
**
*****************************************************************************/

#include "./latexexportdia.h"

#include <kdatastream.h>


static const char* const LATEXExportDia_ftable[2][3] = {
    { "void", "useDefaultConfig()", "useDefaultConfig()" },
    { 0, 0, 0 }
};

bool LATEXExportDia::process(const QCString &fun, const QByteArray &data, QCString& replyType, QByteArray &replyData)
{
    if ( fun == LATEXExportDia_ftable[0][1] ) { // void useDefaultConfig()
	replyType = LATEXExportDia_ftable[0][0]; 
	useDefaultConfig( );
    } else {
	return DCOPObject::process( fun, data, replyType, replyData );
    }
    return TRUE;
}

QCStringList LATEXExportDia::interfaces()
{
    QCStringList ifaces = DCOPObject::interfaces();
    ifaces += "LATEXExportDia";
    return ifaces;
}

QCStringList LATEXExportDia::functions()
{
    QCStringList funcs = DCOPObject::functions();
    for ( int i = 0; LATEXExportDia_ftable[i][2]; i++ ) {
	QCString func = LATEXExportDia_ftable[i][0];
	func += ' ';
	func += LATEXExportDia_ftable[i][2];
	funcs << func;
    }
    return funcs;
}


