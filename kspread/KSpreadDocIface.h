#ifndef KSPREAD_DOC_IFACE_H
#define KSPREAD_DOC_IFACE_H

#include <dcopref.h>
#include <KoDocumentIface.h>

#include <qstring.h>
#include <qcolor.h>
#include <qstringlist.h>
class KSpreadDoc;

class KSpreadDocIface : virtual public KoDocumentIface
{
    K_DCOP
public:
    KSpreadDocIface( KSpreadDoc* );

k_dcop:
    virtual DCOPRef map();
    virtual bool save();
    virtual bool saveAs( const QString& url );

    bool showColHeader()const;
    bool showRowHeader()const;
    int indentValue()const;
    void setIndentValue(int _val);
    bool showTabBar()const;

    void setShowVerticalScrollBar(bool _show);
    void setShowHorizontalScrollBar(bool _show);
    void setShowColHeader(bool _show);
    void setShowRowHeader(bool _show);
    void setShowTabBar(bool _show);
    void setShowCommentIndicator(bool _show);

    void changeDefaultGridPenColor( const QColor &_col);
    bool showCommentIndicator()const;
    bool showFormulaBar()const;
    bool showStatusBar()const;

    void setShowMessageError(bool _show);
    bool showMessageError()const;

    QColor pageBorderColor() const;
    void changePageBorderColor( const QColor & _color);
    void addIgnoreWordAll( const QString &word);
    void clearIgnoreWordAll( );
    QStringList spellListIgnoreAll() const;

    void addStringCompletion(const QString & stringCompletion);
    int zoom() const;

private:
    KSpreadDoc* doc;
};

#endif
