#ifndef KOHYPHEN_H
#define KOHYPHEN_H

#include <qmap.h>
#include <qstring.h>
#include "hyphen.h"

class KoHyphenatorException{
public:
        KoHyphenatorException(const QString& MessageText): messageText(MessageText) {}
        QString message() const { return messageText; }
private:
        QString messageText;
};

/**
 * @short The KoHyphenator class provides an interface to the libhnj hyphenation library
 * @author Alexander Dymo (cloudtemple@mksat.net)
 * @author Lukas Tinkl (lukas.tinkl@suse.cz)
 *
 * The KoHyphenator class provides an interface to the libhnj hyphenation library.
 *
 * Libhnj library was written for TeX and adopted to use with OpenOffice.
 * This library tries to adopt it for KDE.
 *
 * It handles the hyphenation dictionary loading for the specified language.
 * QString -> char* conversion is done by using of settings in
 * dicts.xml file in datadir/koffice/hyphdicts (dictionaries are also located
 * there).
 *
 * The @ref hyphenate() functions returns QString containing hyphenation
 * chars (0xad) or char* in format of hnj_hyphen_hyphenate() function from
 * libhnj library.
 */
class KoHyphenator{
public:
        /**
         * Returns the single KoHyphenator instance (singleton pattern)
         * Beware that this might throw an exception in case of an installation problem!
         * Catch KoHyphenatorExceptions!
         */
        static KoHyphenator* self();

        ~KoHyphenator();

        /**
         * Checks if the letter in position pos is placed before the hyphen.
         *
         * Can be used to check if the line break at given position
         * should be forced and automatic hyphen added.
         */
        bool checkHyphenPos(const QString& str, int pos, const QString& lang) const;

        /**
         * Returns the pointer to the string in hnj_hyphen_hyphenate() format
         * (that is hyphenation function from underlying libhnj library).
         *
         * The string is array of integer numbers. Each odd number marks
         * that hyphen can be added after the character in the position
         * of that number. The returned string must be deleted with "delete[] x;"
         *
         * For example, for the string "example" the returning value is "01224400".
         *
         * @param str String to be hyphenated.
         *
         * @param lang Language for the hyphenation dictionary to be loaded.
         * Language: two chars containing the ISO 639-1 code
         * (for example "en", "uk", etc.) (could be lang_COUNTRY as well).
        */
        char *hyphens(const QString& str, const QString& lang) const;

        /**
         * Hyphenates the string str and returns the string with
         * hyphenation marks in it.
         *
         * @param str String to be hyphenated.
         *
         * @param lang Language for the hyphenation dictionary to be loaded.
         * Language: two chars containing the ISO 639-1 code
         * (for example "en", "uk", etc.) (could be lang_COUNTRY as well).
         */
        QString hyphenate(const QString& str, const QString& lang) const;

private:
        /**
         * @return the encoding of dictionary for the language @p lang.
         */
        QTextCodec* codecForLang(const QString& lang) const;

        KoHyphenator();
        HyphenDict *dict(const QString &lang) const;

        QMap<QString, HyphenDict*> dicts;
        struct EncodingStruct {
            EncodingStruct() // for QMap
                : encoding(), codec(0L) {}
            EncodingStruct(const QCString& _encoding)
                : encoding(_encoding), codec(0L) {}
            QCString encoding;
            QTextCodec* codec;
        };
        typedef QMap<QString, EncodingStruct> EncodingMap;
        mutable EncodingMap encodings; // key is the language code

        static KoHyphenator* s_self;
};

#endif
