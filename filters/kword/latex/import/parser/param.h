
#ifndef __LATEXPARSER_PARAM_H__
#define __LATEXPARSER_PARAM_H__

#include <qstring.h>

class Param
{
	public:
		Param();

		~Param();

		QString getKey() const { return _key; }

		QString getValue() const { return _value; }

		void setKey(QString key) { _key = key; }

		void setValue(QString value) { _value = value; }

	private:
		QString _key;
		QString _value;
};

#endif /* __LATEXPARSER_PARAM_H__ */
