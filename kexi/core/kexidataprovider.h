#ifndef _KEXI_DATA_PROVIDER_H_
#define _KEXI_DATA_PROVIDER_H_

class QStringList;
class KexiDBRecord;
class QString;

//Interface KexiDataProvider
class KexiDataProvider
{
public:
	KexiDataProvider();
	virtual ~KexiDataProvider();
	virtual QStringList datasets()=0;
	virtual QStringList fields(const QString& identifier)=0;
	virtual KexiDBRecord *records(const QString& identifier)=0;
};

#endif
