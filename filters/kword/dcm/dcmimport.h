#pragma once

#include <KoFilter.h>

class DCMImport : public KoFilter
{

	Q_OBJECT

public:
	DCMImport(QObject *parent, QStringList const&)
		: KoFilter(parent) { }

	~DCMImport() {}

	KoFilter::ConversionStatus convert(QByteArray const& from, QByteArray const& to);

};
