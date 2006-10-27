/* This file is part of the KDE project
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005 Martin Ellis <martin.ellis@kdemail.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "identifier.h"
#include <kstaticdeleter.h>
#include <q3dict.h>
//Added by qt3to4:
#include <Q3CString>

using namespace KexiUtils;

bool KexiUtils::isIdentifier(const QString& s)
{
	uint i;
	const uint sLength = s.length();
	for (i=0; i<sLength; i++) {
		QChar c = s.at(i).toLower();
		if (!(c=='_' || c>='a' && c<='z' || i>0 && c>='0' && c<='9'))
			break;
	}
	return i>0 && i==sLength;
}

QString KexiUtils::string2FileName(const QString &s)
{
	QString fn = s.simplified();
	fn.replace(' ',"_"); fn.replace('$',"_");
	fn.replace('\\',"-"); fn.replace('/',"-"); 
	fn.replace(':',"-"); fn.replace('*',"-"); 
	return fn;
}

// These are in pairs - first the non-latin character in UTF-8,
// the second is the latin character(s) to appear in identifiers.
static const char* string2Identifier_table[] = {
/* 1. Polish characters */
"Ą", "A",  "Ć", "C",  "Ę", "E",
"Ł", "L",  "Ń", "N",  "Ó", "O",
"Ś", "S",  "Ź", "Z",  "Ż", "Z",
"ą", "a",  "ć", "c",  "ę", "e",
"ł", "l",  "ń", "n",  "ó", "o",
"ś", "s",  "ź", "z",  "ż", "z",

/* 2. The mappings of the german "umlauts" to their 2-letter equivalents:
  (Michael Drüing <michael at drueing.de>)

 Note that ß->ss is AFAIK not always correct transliteration, for example
 "Maße" and "Masse" is different, the first meaning "measurements" (as
 plural of "Maß" meaning "measurement"), the second meaning "(physical)
 mass". They're also pronounced dirrefently, the first one is longer, the
 second one short. */
/** @todo the above three only appear at the beginning of a word. if the word is in
 all caps - like in a caption - then the 2-letter equivalents should also be
 in all caps */
"Ä", "Ae",
"Ö", "Oe",
"Ü", "Ue",
"ä", "ae",
"ö", "oe",
"ü", "ue",
"ß", "ss",
 
/* 3. The part of Serbian Cyrillic which is shared with other Cyrillics but 
 that doesn't mean I am sure that eg. Russians or Bulgarians would do the 
 same. (Chusslove Illich <caslav.ilic at gmx.net>) */
"а", "a",
"б", "b",
"в", "v",
"г", "g",
"д", "d",
"е", "e",
"ж", "z",
"з", "z",
"и", "i",
"к", "k",
"л", "l",
"м", "m",
"н", "n",
"о", "o",
"п", "p",
"р", "r",
"с", "s",
"т", "t",
"у", "u",
"ф", "f",
"х", "h",
"ц", "c",
"ч", "c",
"ш", "s",
"А", "A",
"Б", "B",
"В", "V",
"Г", "G",
"Д", "D",
"Е", "E",
"Ж", "Z",
"З", "Z",
"И", "I",
"К", "K",
"Л", "L",
"М", "M",
"Н", "N",
"О", "O",
"П", "P",
"Р", "R",
"С", "S",
"Т", "T",
"У", "U",
"Ф", "F",
"Х", "H",
"Ц", "C",
"Ч", "C",
"Ш", "S",
// 3.1. The Serbian-specific Cyrillic characters:
"ђ", "dj",
"ј", "j",
"љ", "lj",
"њ", "nj",
"ћ", "c",
"џ", "dz",
"Ђ", "Dj",
"Ј", "J",
"Љ", "Lj",
"Њ", "Nj",
"Ћ", "C",
"Џ", "Dz",
// 3.2. The non-ASCII Serbian Latin characters:
"đ", "dj",
"ž", "z",
"ć", "c",
"č", "c",
"š", "s",
"Đ", "Dj",
"Ž", "Z",
"Ć", "C",
"Č", "C",
"Š", "S",
// 4. Czech characters (cs_CZ, Michal Svec)
 "Á", "A",
 "Č", "C",
 "Ď", "D",
 "É", "E",
 "Ě", "E",
 "Í", "I",
 "Ň", "N",
 "Ó", "O",
 "Ř", "R",
 "Š", "S",
 "Ť", "T",
 "Ú", "U",
 "Ů", "U",
 "Ý", "Y",
 "Ž", "Z",
 "á", "a",
 "č", "c",
 "ď", "d",
 "é", "e",
 "ě", "e",
 "í", "i",
 "ň", "n",
 "ó", "o",
 "ř", "r",
 "š", "s",
 "ť", "t",
 "ú", "u",
 "ů", "u",
 "ý", "y",
 "ž", "z",
// END.
0
};

//! used for O(1) character transformations in char2Identifier()
static KStaticDeleter< Q3Dict<Q3CString> > string2Identifier_deleter;
static Q3Dict<Q3CString>* string2Identifier_dict = 0;

inline QString char2Identifier(const QChar& c)
{
	if ((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_')
		return QString(c);
	else {
		if (!string2Identifier_dict) {
			//build dictionary for later use
			string2Identifier_deleter.setObject( string2Identifier_dict, new Q3Dict<Q3CString>(1009) );
			string2Identifier_dict->setAutoDelete(true);
			for (const char **p = string2Identifier_table; *p; p+=2) {
				string2Identifier_dict->replace( /* replace, not insert because there may be duplicates */
					QString::fromUtf8(*p), new Q3CString(*(p+1)) );
			}
		}
		const Q3CString *fixedChar = string2Identifier_dict->find(c);
		if (fixedChar)
			return *fixedChar;
	}
	return QString(QChar('_'));
}

QString KexiUtils::string2Identifier(const QString &s)
{
	QString r;
	QString id( s.simplified() );
	if (id.isEmpty())
		return id;
	r.reserve(id.length());
//		return "_";
	id.replace(' ',"_");
	QChar c = id[0];

	if (c>='0' && c<='9') {
		r+='_';
		r+=c;
	} else
		r+=char2Identifier(c);

	const uint idLength = id.length();
	for (uint i=1; i<idLength; i++)
		r+=char2Identifier(id.at(i));
	return r;
}

//--------------------------------------------------------------------------------

QString KexiUtils::identifierExpectedMessage(const QString &valueName, const QVariant& v)
{
	return "<p>"+i18n("Value of \"%1\" column must be an identifier.", valueName)
		+"</p><p>"+i18n("\"%1\" is not a valid identifier.", v.toString())+"</p>";
}

//--------------------------------------------------------------------------------

IdentifierValidator::IdentifierValidator(QObject * parent)
: Validator(parent)
{
}

IdentifierValidator::~IdentifierValidator()
{
}

QValidator::State IdentifierValidator::validate( QString& input, int& pos ) const
{
	uint i;
	for (i=0; (int)i<input.length() && input.at(i)==' '; i++)
		;
	pos -= i; //i chars will be removed from beginning
	if ((int)i<input.length() && input.at(i)>='0' && input.at(i)<='9')
		pos++; //_ will be added at the beginning
	bool addspace = (input.right(1)==" ");
	input = string2Identifier(input);
	if (addspace)
		input += "_";
	if(pos > input.length())
		pos=input.length();
	return input.isEmpty() ? QValidator::Intermediate : Acceptable;
}

Validator::Result IdentifierValidator::internalCheck(
	const QString &valueName, const QVariant& v, 
	QString &message, QString & /*details*/)
{
	if (isIdentifier(v.toString()))
		return Validator::Ok;
	message = identifierExpectedMessage(valueName, v);
	return Validator::Error;
}

