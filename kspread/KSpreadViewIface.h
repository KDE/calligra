#ifndef KSPREAD_VIEW_IFACE_H
#define KSPREAD_VIEW_IFACE_H

#include <KoViewIface.h>

#include <qstring.h>
#include <qrect.h>

class KSpreadView;
class KSpreadCellProxy;

class KSpreadViewIface : public KoViewIface
{
    K_DCOP
public:
    KSpreadViewIface( KSpreadView* );
    ~KSpreadViewIface();

k_dcop:
    virtual DCOPRef doc() const;
    virtual DCOPRef map() const;
    virtual DCOPRef table() const;

    virtual void changeNbOfRecentFiles(int _nb);

    virtual void hide();
    virtual void show();

    virtual void find();
    virtual void replace();
    virtual void conditional();
    virtual void validity();
    virtual void insertSeries();
    virtual void insertHyperlink();
    virtual void gotoCell();
    virtual void changeAngle();
    virtual void preference();
    virtual void nextTable();
    virtual void previousTable();
    virtual void sortList();
    virtual void setAreaName();
    virtual void showAreaName();
    virtual void mergeCell();
    virtual void dissociateCell();
    virtual void consolidate();

    virtual void deleteColumn();
    virtual void insertColumn();
    virtual void deleteRow();
    virtual void insertRow();
    virtual void hideRow();
    virtual void showRow();
    virtual void hideColumn();
    virtual void showColumn();
    virtual void upper();
    virtual void lower();

    virtual void equalizeColumn();
    virtual void equalizeRow();

    virtual void clearTextSelection();
    virtual void clearCommentSelection();
    virtual void clearValiditySelection();
    virtual void clearConditionalSelection();
    virtual void goalSeek();
    virtual void insertFromDatabase();
    virtual void insertFromTextfile();
    virtual void insertFromClipboard();
    virtual void textToColumns();
    virtual void copyAsText();


private:
    KSpreadView* m_view;
    KSpreadCellProxy* m_proxy;
};

#endif
