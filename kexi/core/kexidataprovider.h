#ifndef _KEXI_DATA_PROVIDER_H_
#define _KEXI_DATA_PROVIDER_H_

#include <qstring.h>
#include <qmap.h>
#include <klocale.h>

class QStringList;
class KexiDBRecord;


//Interface KexiDataProvider
class KexiDataProvider
{
public:
	class Parameter{
	public:
		Parameter(const QString& name_, const int type_){name=name_;type=type_;}
		Parameter(){}
		enum Type{Unknown=0,Text=1,Float=2,Int=3,Date=4,Time=5,DateTime=6,TheLastOne=0xff};
		static const char* typeNames[];
		static const char* typeDescription[];
		static const int maxType;
		QString name;
		int type;
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
