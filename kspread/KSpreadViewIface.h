
#ifndef KSPREAD_VIEW_IFACE_H
#define KSPREAD_VIEW_IFACE_H

#include <KoViewIface.h>

#include <qstring.h>
#include <qrect.h>
#include <qcolor.h>

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

    virtual void setSelection(QRect selection);
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

  virtual void setSelectionMoneyFormat( bool b );
  virtual void setSelectionPrecision( int delta );
  virtual void setSelectionPercent( bool b );
  virtual void setSelectionMultiRow( bool enable );
  virtual void setSelectionSize(int size);
  virtual void setSelectionUpper();
  virtual void setSelectionLower();
  virtual void setSelectionFirstLetterUpper();
  virtual void setSelectionVerticalText(bool enable);
  virtual void setSelectionComment(QString comment);
  virtual void setSelectionAngle(int value);
  virtual void setSelectionTextColor(QColor txtColor );
  virtual void setSelectionBgColor(QColor bgColor );
  virtual void setSelectionBorderColor(QColor bdColor );

  virtual void deleteSelection();
  virtual void copySelection();
  virtual void cutSelection();

  virtual void setLeftBorderColor(QColor color);
  virtual void setTopBorderColor(QColor color);
  virtual void setRightBorderColor(QColor color);
  virtual void setBottomBorderColor(QColor color);
  virtual void setAllBorderColor(QColor color);
  virtual void setOutlineBorderColor(QColor color);
  virtual void removeBorder();

  virtual void increaseIndent();
  virtual void decreaseIndent();

private:
    KSpreadView* m_view;
    KSpreadCellProxy* m_proxy;
};

#endif
