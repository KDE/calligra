#ifndef KSPREAD_HANDLER_H
#define KSPREAD_HANDLER_H

#include <KoContainerHandler.h>

#include <qpoint.h>

#include <KoQueryTrader.h>

class QWidget;

enum HandlerType
{
  Part,
  Chart,
  Picture
};

namespace KSpread
{
class View;

/**
 * This event handler is used to insert a new part. The event handler
 * takes care of selecting the rectangle at which the new
 * part will appear.
 */
class InsertHandler : public KoEventHandler
{
    Q_OBJECT
public:
    InsertHandler( View* view, QWidget* widget );
    ~InsertHandler();

    virtual HandlerType getType() = 0;

protected:
    bool eventFilter( QObject*, QEvent* );
    virtual void insertObject( QRect ) = 0;

    View* m_view;
    QPoint m_geometryStart;
    QPoint m_geometryEnd;
    bool m_started;
    bool m_clicked;
};

class InsertPartHandler : public InsertHandler
{
  public:
    InsertPartHandler( View* view, QWidget* widget, const KoDocumentEntry& entry );
    virtual ~InsertPartHandler();
    HandlerType getType() { return Part; }

  private:
    void insertObject( QRect );

    KoDocumentEntry m_entry;
};


class InsertChartHandler : public InsertHandler
{
  public:
    InsertChartHandler( View* view, QWidget* widget, const KoDocumentEntry& entry );
    virtual ~InsertChartHandler();
    HandlerType getType() { return Chart; }

  private:
    void insertObject( QRect );

    KoDocumentEntry m_entry;
};

class InsertPictureHandler : public InsertHandler
{
  public:
    InsertPictureHandler( View* view, QWidget* widget, const KURL &);
    virtual ~InsertPictureHandler();
    HandlerType getType() { return Picture; }

  private:
    void insertObject( QRect );

    KURL m_file;
};

} // namespace KSpread

#endif
