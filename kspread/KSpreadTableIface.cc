#include "KSpreadTableIface.h"

#include "kspread_table.h"

KSpreadTableIface::KSpreadTableIface( KSpreadTable* t )
    : DCOPObject( t )
{
    m_table = t;
}

DCOPRef KSpreadTableIface::cell( int x, int y )
{
}

DCOPRef KSpreadTableIface::cell( const QString& name )
{
}

QRect KSpreadTableIface::selection() const
{
    return m_table->selectionRect();
}

void KSpreadTableIface::setSelection( const QRect& selection )
{
    m_table->setSelection( selection );
}

QString KSpreadTableIface::name() const
{
    return m_table->tableName();
}
