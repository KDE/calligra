#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <handlers.h>
#include <qstring.h>
#include <qcolor.h>
#include <qdom.h>

#include <string>

namespace wvWare {
    class Parser;
}

class KWordCharacterHandler : public wvWare::SpecialCharHandler
{
public:
    virtual wvWare::U8 hardLineBreak();
    virtual wvWare::U8 nonBreakingHyphen();
    virtual wvWare::U8 nonRequiredHyphen();
};

class Document : public wvWare::BodyTextHandler
{
public:
    Document( const std::string& fileName, QDomDocument& mainDocument, QDomElement& mainFramesetElement );
    virtual ~Document();

    bool parse();

    virtual void paragraphStart( wvWare::SharedPtr<const wvWare::Word97::PAP> pap );
    virtual void paragraphEnd();
    virtual void runOfText( const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp );

    virtual void pageBreak();

private:
    QString getFont(unsigned fc) const;
    void writeOutParagraph( const QString& styleName, const QString& text );
    QColor colorForNumber(int number, int defaultcolor, bool defaultWhite = false);

    QString m_paragraph;
    QDomDocument& m_mainDocument;
    QDomElement& m_mainFramesetElement;
    QDomElement m_formats;
    QDomElement m_oldLayout;
    int m_index;
    wvWare::SharedPtr<const wvWare::Word97::PAP> m_pap;
    KWordCharacterHandler* m_charHandler;
    wvWare::SharedPtr<wvWare::Parser> m_parser;
};

#endif // DOCUMENT_H
