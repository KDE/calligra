/****************************************************************************
** VCDlgEllipse meta object code from reading C++ file 'vcdlg_ellipse.h'
**
** Created: Fri Jul 27 18:04:43 2001
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#if !defined(Q_MOC_OUTPUT_REVISION)
#define Q_MOC_OUTPUT_REVISION 9
#elif Q_MOC_OUTPUT_REVISION != 9
#error "Moc format conflict - please regenerate all moc files"
#endif

#include "vcdlg_ellipse.h"
#include <qmetaobject.h>
#include <qapplication.h>



const char *VCDlgEllipse::className() const
{
    return "VCDlgEllipse";
}

QMetaObject *VCDlgEllipse::metaObj = 0;

void VCDlgEllipse::initMetaObject()
{
    if ( metaObj )
	return;
    if ( qstrcmp(KDialog::className(), "KDialog") != 0 )
	badSuperclassWarning("VCDlgEllipse","KDialog");
    (void) staticMetaObject();
}

#ifndef QT_NO_TRANSLATION

QString VCDlgEllipse::tr(const char* s)
{
    return qApp->translate( "VCDlgEllipse", s, 0 );
}

QString VCDlgEllipse::tr(const char* s, const char * c)
{
    return qApp->translate( "VCDlgEllipse", s, c );
}

#endif // QT_NO_TRANSLATION

QMetaObject* VCDlgEllipse::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    (void) KDialog::staticMetaObject();
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    QMetaData::Access *slot_tbl_access = 0;
    metaObj = QMetaObject::new_metaobject(
	"VCDlgEllipse", "KDialog",
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    metaObj->set_slot_access( slot_tbl_access );
#ifndef QT_NO_PROPERTIES
#endif // QT_NO_PROPERTIES
    return metaObj;
}
