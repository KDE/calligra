#ifndef KOHYPHEN_H
#define KOHYPHEN_H

#include <map>
#include <qstring.h>
#include "hyphen.h"

class KoTextString;

class KoHyphenatorException{
public:
        KoHyphenatorException(QString MessageText): messageText(MessageText) {}
        QString message() { return messageText; }
private:
        QString messageText;
};

/**
@short The KoHyphenator class provides an interface to the libhnj hyphenation library
@author Alexander Dymo (cloudtemple@mksat.net)

The KoHyphenator class provides an interface to the libhnj hyphenation library.

Libhnj library was written for TeX and adopted to use with OpenOffice.
This library tries to adopt it for KOffice.

It handles the hyphenation dictionary loading for the specified language.
KoTextString/QString -> char* conversion is done by using of settings in
dicts.xml file in datadir/koffice/hyphdicts (dictionaries are also located
there).

The @ref hyphenate() functions returns QString containing hyphenation
chars (0xad) or char* in format of hnj_hyphen_hyphenate() function from
libhnj library.
*/
class KoHyphenator{
public:
        KoHyphenator();
        ~KoHyphenator();

        /**
        Checks if the string was hyphenated before and no hyphenation nessesary.
        Currently used with kotext library - things may change ;)
        */
        static bool isHyphenated(KoTextString *string, int pos, int lastBreak);

        /**
        Checks if the letter in position pos is placed before the hyphen.

        Should be used in kotext library to check if the line break at given position
        should be forced and automatic hyphen added.
        */
        bool checkHyphenPos(QString str, int pos, QString lang);

        /**
        Returns the pointer to the string in hnj_hyphen_hyphenate() format
        (that is hyphenation function from underligning libhnj library).

        The string is array of integer numbers. Each odd number marks
        that hyphen can be added after the character in the position
        of that number.

        For example, for the string "example" the returning value is "01224400".

        @param str String to be hyphenated.

        @param lang Language for the hyphenation dictionary to be loaded.
        Language: two chars containing the ISO 639-1 code
        (for example "en", "uk", etc.) (could be lang_COUNTRY as well).
        */
        char *hyphens(QString str, QString lang);
        /**
        Hyphenates the string str and returns the string with
        hyphenation marks in it.

        @param str String to be hyphenated.

        @param lang Language for the hyphenation dictionary to be loaded.
        Language: two chars containing the ISO 639-1 code
        (for example "en", "uk", etc.) (could be lang_COUNTRY as well).
        */
        QString hyphenate(QString str, QString lang);

        /**
        Returns the encoding of dictionaty for the language lang.
        */
        QString encodingForLang(QString lang);
private:
        HyphenDict *dict(QString &lang);
        std::map<QString, HyphenDict*> dicts;
        std::map<QString, QString> encodings;
};

#endif
