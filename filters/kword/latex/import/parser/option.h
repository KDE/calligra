
#ifndef __LATEXPARSER_OPTION_H__
#define __LATEXPARSER_OPTION_H__

#include <qstring.h>

class Option
{
	public:
		Option() { }

		~Option() { }

		QString getKey() const { return _key; }
		
		QString getValue() const { return _value; }
		
		void setKey(QString key) { _key = key; }

		void setValue(QString value) { _value = value; }

	private:
		QString _key;
		QString _value;
};

#endif /* __LATEXPARSER_OPTION_H__ */
