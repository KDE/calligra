#ifndef _KEXI_DATA_PROVIDER_H_
#define _KEXI_DATA_PROVIDER_H_

#include <qstring.h>
#include <qmap.h>

class QStringList;
class KexiDBRecord;


//Interface KexiDataProvider
class KexiDataProvider
{
public:
	class Parameter{
	public:
		enum Type{Unknown=0,String=1,Float=2,Date=3,TheLastOne=0xff};
		QString name;
		Type type;
	};
	typedef QValueList<Parameter> ParameterList;

	typedef QMap<QString,QString> Parameters;

	KexiDataProvider();
	virtual ~KexiDataProvider();
	virtual QStringList datasets()=0;
	virtual QStringList fields(const QString& identifier)=0;
	virtual KexiDBRecord *records(const QString& identifier,Parameters params)=0;
	virtual ParameterList parameters(const QString &identifier)=0;
};

#endif
