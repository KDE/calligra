
#ifndef _KEXI_TABLE_IMPORTER_SOURCE_INTERFACE_
#define _KEXI_TABLE_IMPORTER_SOURCE_INTERFACE_

#include <qstringlist.h>
#include <qvariant.h>
#include <kexiDB/kexidbtable.h>
#include <qwidget.h>
#include <qptrlist.h>

#define KEXITABLEIMPORTSOURCEIFACE(obj) (obj?static_cast<KexiTableImportSourceIface*>(\
		obj->qt_cast("KexiTableImportSourceIface")):0)

class KexiTableImportSourceIface {
public:
	KexiTableImportSourceIface() {}
	virtual ~KexiTableImportSourceIface() {}

	virtual KexiDBTable tableStructure()=0;

	virtual bool firstTableRow()=0;
	virtual bool nextTableRow()=0;
	virtual QVariant tableValue(int field)=0;

	virtual QPtrList<QWidget> tableSourceWidgets(QWidget *parent)=0;
};
#endif
