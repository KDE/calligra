#ifndef KSPREAD_DOC_IFACE_H
#define KSPREAD_DOC_IFACE_H

#include <dcopref.h>
#include <KoDocumentIface.h>

#include <qstring.h>
#include <qcolor.h>
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
    virtual float paperHeight() const;
    virtual float paperWidth() const ;
    virtual float leftBorder() const;
    virtual float rightBorder() const;
    virtual float topBorder() const;
    virtual float bottomBorder() const;
    QString paperFormatString() const;

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

    QString headLeft()const;
    QString headMid()const;
    QString headRight()const;
    QString footLeft()const;
    QString footMid()const;
    QString footRight()const;
private:
    KSpreadDoc* doc;
};

#endif
