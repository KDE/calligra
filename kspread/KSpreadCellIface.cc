#include "KSpreadCellIface.h"

#include "kspread_table.h"
#include "kspread_cell.h"

KSpreadCellIface::KSpreadCellIface()
{
}

void KSpreadCellIface::setCell( KSpreadTable* table, const QPoint& point )
{
    m_table = table;
    m_point = point;
}

QString KSpreadCellIface::text() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->valueString();
}

void KSpreadCellIface::setText( const QString& text )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setText( text );
}

void KSpreadCellIface::setValue( int value )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setValue( (double)value );
}

void KSpreadCellIface::setValue( double value )
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    cell->setValue( value );
}

double KSpreadCellIface::value() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->valueDouble();
}

void KSpreadCellIface::setBgColor(const QString& _c)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(_c);
    cell->setBgColor(c);
}

void KSpreadCellIface::setBgColor(int r,int g,int b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(r,g,b);
    cell->setBgColor(c);
}

QString KSpreadCellIface::bgColor() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->bgColor().name();
}

QString KSpreadCellIface::textColor() const
{
    KSpreadCell* cell = m_table->cellAt( m_point );
    return cell->textColor().name();
}

void KSpreadCellIface::setTextColor(int r,int g,int b)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(r,g,b);
    cell->setTextColor(c);
}

void KSpreadCellIface::setTextColor(const QString& _c)
{
    KSpreadCell* cell = m_table->nonDefaultCell( m_point.x(), m_point.y() );
    QColor c(_c);
    cell->setTextColor(c);
}
