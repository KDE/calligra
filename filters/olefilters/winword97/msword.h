/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

DESCRIPTION

    This file implements an abstraction of the on-disk format of Microsoft
    Word documents. The public interface implements iterators that allow
    convenient traversal of the types of data in a document.

    We also add hand-generated structures and routines to complement all
    the machine-generated ones in our superclass.
*/

#ifndef MSWORD_H
#define MSWORD_H

#include <kdebug.h>
#include <mswordgenerated.h>
#include <qarray.h>

class Properties;

class MsWord: public MsWordGenerated
{
public:

    // Construction. Invoke with the OLE streams that comprise the Word document.

    MsWord(
        const U8 *mainStream,
        const U8 *table0Stream,
        const U8 *table1Stream,
        const U8 *dataStream);
    virtual ~MsWord();

    // Metadata.

    QString m_title;
    QString m_subject;
    QString m_author;
    QString m_lastRevisedBy;

    // Call the parse() function to process the document. The callbacks return
    // the text along with any relevant attributes.

    void parse();

// TBD: this will be not remain public once I figure out how the nested classes
// can be made to work (as friends?).
public:

    // Hand-generated structure definitions and reader prototypes. Reader
    // prototypes are also declared for inherited structure definitions
    // which are OK, but where the inherited reader must be overridden.

    // Strings and Pascal strings.

    static unsigned read(
        U16 lid,
        const U8 *in,
        QString *out,
        unsigned length,
        bool unicode);
    static unsigned read(
        U16 lid,
        const U8 *in,
        QString *out,
        bool unicode);

    typedef struct CHPXFKP
    {
        U8 count;
        U8 *ptr;
    } CHPXFKP;
    unsigned read(const U8 *in, CHPXFKP *out);

    typedef struct PAPXFKP
    {
        U16 istd;
        U16 count;
        U8 *ptr;
    } PAPXFKP;
    unsigned read(const U8 *in, PAPXFKP *out);

    // STyle Definition (STD)
    typedef struct STD
    {

        // invariant style identifier
        U16 sti:12;

        // spare field for any temporary use, always reset back to zero!
        U16 fScratch:1;

        // PHEs of all text with this style are wrong
        U16 fInvalHeight:1;

        // UPEs have been generated
        U16 fHasUpe:1;

        // std has been mass-copied; if unused at save time, style should be
        // deleted
        U16 fMassCopy:1;

        // style type code
        U16 sgc:4;

        // base style
        U16 istdBase:12;

        // # of UPXs (and UPEs)
        U16 cupx:4;

        // next style
        U16 istdNext:12;

        // offset to end of upx's, start of upe's
        U16 bchUpe;

        // auto redefine style when appropriate
        U16 fAutoRedef:1;

        // hidden from UI?
        U16 fHidden:1;

        // unused bits
        U16 unused8_3:14;

        // sub-names are separated by chDelimStyle
        QString xstzName;

        //
        const U8 *grupx;
    } STD;
    unsigned read(const U8 *in, unsigned baseInFile, STD *out);

    // STTBF (STring TaBle stored in File)
    typedef struct STTBF
    {
        U16 stringCount;
        U16 extraDataLength;
        QString *strings;
        const U8 **extraData;
        STTBF();
        ~STTBF();
    } STTBF;
    unsigned read(const U8 *in, STTBF *out);

    static unsigned read(const U8 *in, FIB *out);
    unsigned read(const U8 *in, BTE *out);
    unsigned read(const U8 *in, PCD *out);
    unsigned read(const U8 *in, PHE *out);

    // Some fundamental data structures. We keep pointers to our streams,
    // and a copy of the FIB.

    const U8 *m_mainStream;
    const U8 *m_tableStream;
    const U8 *m_dataStream;
    FIB m_fib;

    // Cache for styles in stylesheet. This is an array of fully "decoded"
    // PAPs - that will help performance with lots of paragraphs.

    Properties **m_styles;

    // Cache for list styles. This is an array of LVLF pointersfully "decoded"
    // PAPs - that will help performance with lots of paragraphs.

    LVLF ***m_listStyles;

    // This class allows standardised treatment of plexes of different types.
    // It is designed to be instantiated locally, once for each plex we are
    // interested in.

    template <class T, int word6Size>
    class Plex
    {
    public:

        // We would like to define the constructor in terms of a callback to
        // take the data within the plex. Unfortunately, egcs-2.91-66 does not
        // support taking the address of bound pointer-to-member.
        //
        // Plex(bool (*callback)(unsigned start, unsigned end, const <T>&

        Plex(MsWord *document);

        // We would like to define the iterator as a proper friend class with a
        // constructor like this:
        //
        // template<class T>
        // PlexIterator<T>(Plex<T> plex);
        //
        // but that results in a compiler error.

        void startIteration(const U8 *plex, const U32 byteCount);
        bool getNext(U32 *startFc, U32 *endFc, T *data);
    protected:
        MsWord *m_document;
        const U8 *m_plex;
        U32 m_byteCount;
        unsigned m_crun;
        const U8 *m_fcNext;
        const U8 *m_dataNext;
        unsigned m_i;
    };

    // This class allows standardised treatment of FKPs of different types.
    // It is designed to be instantiated locally, once for each FKP we are
    // interested in.

    template <class T1, class T2>
    class Fkp
    {
    public:

        // We would like to define the constructor in terms of a callback to
        // take the data within the plex. Unfortunately, egcs-2.91-66 does not
        // support taking the address of bound pointer-to-member.
        //
        // Fkp(bool (*callback)(unsigned start, unsigned end, const <T>&

        Fkp(MsWord *document);

        // We would like to define the iterator as a proper friend class with a
        // constructor like this:
        //
        // template<class T>
        // FkpIterator<T>(Fkp<T> fkp);
        //
        // but that results in a compiler error.

        void startIteration(const U8 *fkp);
        bool getNext(U32 *startFc, U32 *endFc, U8 *rgb, T1 *data1, T2 *data2);
    protected:
        MsWord *m_document;
        const U8 *m_fkp;
        U8 m_crun;
        const U8 *m_fcNext;
        const U8 *m_dataNext;
        U8 m_i;
    };

protected:
    // Character property handling.

    typedef struct
    {
        U32 startFc;
        U32 endFc;
        CHPXFKP data;
    } CHPX;

    typedef QArray<CHPX> CHPXarray;

    virtual void gotParagraph(
        const QString &text,
        const PAP &pap,
        const CHPXarray &chpxs) = 0;
    virtual void gotHeadingParagraph(
        const QString &text,
        const PAP &pap,
        const CHPXarray &chpxs) = 0;
    virtual void gotListParagraph(
        const QString &text,
        const PAP &pap,
        const CHPXarray &chpxs) = 0;
    virtual void gotTableBegin() = 0;
    virtual void gotTableEnd() = 0;
    virtual void gotTableRow(
        const QString texts[],
        const PAP styles[],
        TAP &row) = 0;

private:
    friend class Properties;

    // Error handling and reporting support.

    static const int s_area = 30513;
    QString m_constructionError;
    void constructionError(unsigned line, const char *reason);
    static const unsigned s_minWordVersion = 100;
    static const unsigned s_maxWord6Version = 105;

    // Character property handling.

    void getChpxs(U32 startFc, U32 endFc, CHPXarray &result);
    void getChpxs(const U8 *fkp, U32 startFc, U32 endFc, CHPXarray &result);

    // The text stream contains body text, header, footers, endnotes, footnotes
    // and so on. These routines help walk the text stream.

    void getParagraphsFromBtes(
        U32 startFc,
        U32 endFc,
        bool unicode);
    void getParagraphsFromPapxs(
        const U8 *fkp,
        U32 startFc,
        U32 endFc,
        bool unicode);
    struct
    {
        QString text;
        CHPXarray chpxs;
    } m_partialParagraph;

    // Convert a char into a unicode character.

    static QString char2unicode(unsigned lid, char c);
    static const char *lid2codepage(U16 lid);

    // Decode a paragraph into the various types for which we have callbacks.

    void decodeParagraph(const QString &text, PHE &layout, PAPXFKP &style, CHPXarray &chpxs);

    // Table variables. We store up the paragraphs in a row,
    // and then return the whole lot in one go.

    bool m_wasInTable;
    unsigned m_tableColumn;
    QString m_tableText[64];
    PAP m_tableStyle[64];

    // Get the metadata for the file.

    void getAssociatedStrings();

    // Fetch the styles in the style sheet.

    void getStyles();

    // Fetch the list styles.

    void getListStyles();
};
#endif
