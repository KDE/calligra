
#ifndef __LATEXPARSER_H__
#define __LATEXPARSER_H__

#include <qstring.h>

class LatexParser
{
	public:
		LatexParser();

		LatexParser(QString fileIn);

		~LatexParser();

		QString getInputFile() const { return _filename; }

		void setInputFile(QString filename) { _filename = filename; }

		void parse();

	private:
		QString _filename;

};

#endif /* __LATEXPARSER_H__ */

