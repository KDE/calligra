/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: KWAutoFormat (header)                                  */
/******************************************************************/

#ifndef kwautoformat_h
#define kwautoformat_h

#include <qstring.h>
#include <qcolor.h>

#include "format.h"

class KWordDocument;
class KWFormatContext;
class KWParag;
class KWString;

/******************************************************************/
/* Class: KWAutoFormatEntry                                       */
/******************************************************************/

class KWAutoFormatEntry
{
public:
	KWAutoFormatEntry();

	void setFind(const QString &str);
	void setReplace(const QString &str);
	void setCheckFamily(bool b);
	void setCheckColor(bool b);
	void setCheckSize(bool b);
	void setCheckBold(bool b);
	void setCheckItalic(bool b);
	void setCheckUnderline(bool b);
	void setCheckVertAlign(bool b);
	void setFamily(const QString &str);
	void setColor(const QColor &c);
	void setSize(int size);
	void setBold(bool b);
	void setItalic(bool b);
	void setUnderline(bool b);
	void setVertAlign(KWFormat::VertAlign va);


	QString getFind();
	QString getReplace();
	bool getCheckFamily();
	bool getCheckColor();
	bool getCheckSize();
	bool getCheckBold();
	bool getCheckItalic();
	bool getCheckUnderline();
	bool getCheckVertAlign();
	QString getFamily();
	QColor getColor();
	int getSize();
	bool getBold();
	bool getItalic();
	bool getUnderline();
	bool getVertAlign();

	bool isValid()
	{ return !find.isEmpty() && !replace.isEmpty(); }

protected:
	QString find,replace;
	bool checkFamily,checkColor,checkSize,checkBold,checkItalic,checkUnderline,checkVertAlign;
	QString family;
	QColor color;
	int size;
	bool bold,italic,underline;
	KWFormat::VertAlign vertAlign;

};

/******************************************************************/
/* Class: KWAutoFormat                                            */
/******************************************************************/

class KWAutoFormat
{
public:
	struct TypographicQuotes
	{
		TypographicQuotes() : begin('»'), end('«'), replace(true)
		{}

		QChar begin,end;
		bool replace;
	};

	enum AutoformatType {AT_TypographicQuotes,AT_UpperCase,AT_UpperUpper};

	struct AutoformatInfo
	{
		QChar c;
		AutoformatType type;
	};

	KWAutoFormat(KWordDocument *_doc);

	void startAutoFormat(KWParag *parag,KWFormatContext *fc);
	bool doAutoFormat(KWParag *parag,KWFormatContext *fc);
	void endAutoFormat(KWParag *parag,KWFormatContext *fc);
	bool doTypographicQuotes(KWParag *parag,KWFormatContext *fc);
	bool doUpperCase(KWParag *parag,KWFormatContext *fc);

	void setEnabled(bool e) { enabled = e; }
	bool isEnabled() { return enabled; }

	void configTypographicQuotes(TypographicQuotes _tq);
	void configUpperCase(bool _uc);
	void configUpperUpper(bool _uu);

	TypographicQuotes getConfigTypographicQuotes()
	{ return typographicQuotes; }
	bool getConfigUpperCase()
	{ return convertUpperCase; }
	bool getConfigUpperUpper()
	{ return convertUpperUpper; }

	static bool isUpper(const QChar &c);
	static bool isLower(const QChar &c);
	static bool isMark(const QChar &c);

protected:
	KWordDocument *doc;
	TypographicQuotes typographicQuotes;
	bool enabled;
	KWString *tmpBuffer;
	bool lastWasDotSpace,convertUpperCase;
	bool lastWasUpper,convertUpperUpper;

};

#endif
