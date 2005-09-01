#ifndef KSPREAD_HANDLER_H
#define KSPREAD_HANDLER_H

#include <handler.h>

#include <qpoint.h>

#include <koQueryTrader.h>

class KSpreadView;

class QWidget;

/**
 * This event handler is used to insert a new part. The event handler
 * takes care of selecting the rectangle at which the new
 * part will appear.
 */
class KSpreadInsertHandler : public EventHandler
{
    Q_OBJECT
public:
    KSpreadInsertHandler( KSpreadView* view, QWidget* widget, const KoDocumentEntry& entry, bool ischart = false );
    ~KSpreadInsertHandler();

protected:
    bool eventFilter( QObject*, QEvent* );

private:
    KSpreadView* m_view;
    QPoint m_geometryStart;
    QPoint m_geometryEnd;
    bool m_started;
    bool m_clicked;
    KoDocumentEntry m_entry;
    bool m_isChart;
};
			
#endif
