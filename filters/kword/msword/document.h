#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <lldocument.h>
#include <qstring.h>
#include <qdom.h>

class Document : public wvWare::LLDocument
{
public:
    Document( const std::string& fileName, QDomDocument& mainDocument, QDomElement& mainFramesetElement );
    virtual ~Document();

    virtual void paragraphStart( wvWare::SharedPtr<const wvWare::Word97::PAP> pap );
    virtual void paragraphEnd();
    virtual void runOfText( const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp );

private:
    void writeOutParagraph( const QString& name, const QString& text );

    QString m_paragraph;
    QDomDocument& m_mainDocument;
    QDomElement& m_mainFramesetElement;
    QDomElement m_formats;
    int m_index;
    wvWare::SharedPtr<const wvWare::Word97::PAP> m_pap;
};

#endif // DOCUMENT_H
