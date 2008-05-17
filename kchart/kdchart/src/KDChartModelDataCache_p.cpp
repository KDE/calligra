#include "KDChartModelDataCache_p.h"

#include <limits>

using namespace KDChart::ModelDataCachePrivate;

ModelSignalMapperConnector::ModelSignalMapperConnector( ModelSignalMapper& mapper )
    : QObject( 0 ),
      m_mapper( mapper )
{
}

ModelSignalMapperConnector::~ModelSignalMapperConnector()
{
}

void ModelSignalMapperConnector::connectSignals( QAbstractItemModel* model )
{
    connect( model, SIGNAL( destroyed() ),                              this, SLOT( resetModel() ) );
    connect( model, SIGNAL( columnsInserted( QModelIndex, int, int ) ), this, SLOT( columnsInserted( QModelIndex, int, int ) ) );
    connect( model, SIGNAL( columnsRemoved( QModelIndex, int, int ) ),  this, SLOT( columnsRemoved( QModelIndex, int, int ) ) );
    connect( model, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),  this, SLOT( dataChanged( QModelIndex, QModelIndex ) ) );
    connect( model, SIGNAL( layoutChanged() ),                          this, SLOT( layoutChanged() ) );
    connect( model, SIGNAL( modelReset() ),                             this, SLOT( modelReset() ) );
    connect( model, SIGNAL( rowsInserted( QModelIndex, int, int ) ),    this, SLOT( rowsInserted( QModelIndex, int, int )) );
    connect( model, SIGNAL( rowsRemoved( QModelIndex, int, int ) ),     this, SLOT( rowsRemoved( QModelIndex, int, int ) ) );
}

void ModelSignalMapperConnector::disconnectSignals( QAbstractItemModel* model )
{
    disconnect( model, SIGNAL( destroyed() ),                              this, SLOT( resetModel() ) );
    disconnect( model, SIGNAL( columnsInserted( QModelIndex, int, int ) ), this, SLOT( columnsInserted( QModelIndex, int, int ) ) );
    disconnect( model, SIGNAL( columnsRemoved( QModelIndex, int, int ) ),  this, SLOT( columnsRemoved( QModelIndex, int, int ) ) );
    disconnect( model, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),  this, SLOT( dataChanged( QModelIndex, QModelIndex ) ) );
    disconnect( model, SIGNAL( layoutChanged() ),                          this, SLOT( layoutChanged() ) );
    disconnect( model, SIGNAL( modelReset() ),                             this, SLOT( modelReset() ) );
    disconnect( model, SIGNAL( rowsInserted( QModelIndex, int, int ) ),    this, SLOT( rowsInserted( QModelIndex, int, int )) );
    disconnect( model, SIGNAL( rowsRemoved( QModelIndex, int, int ) ),     this, SLOT( rowsRemoved( QModelIndex, int, int ) ) );
}

void ModelSignalMapperConnector::resetModel()
{
    m_mapper.resetModel();
}

void ModelSignalMapperConnector::columnsInserted( const QModelIndex& parent, int start, int end )
{
    m_mapper.columnsInserted( parent, start, end );
}

void ModelSignalMapperConnector::columnsRemoved( const QModelIndex& parent, int start, int end )
{
    m_mapper.columnsRemoved( parent, start, end );
}

void ModelSignalMapperConnector::dataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight )
{
    m_mapper.dataChanged( topLeft, bottomRight );
}

void ModelSignalMapperConnector::layoutChanged()
{
    m_mapper.layoutChanged();
}

void ModelSignalMapperConnector::modelReset()
{
    m_mapper.modelReset();
}

void ModelSignalMapperConnector::rowsInserted( const QModelIndex& parent, int start, int end )
{
    m_mapper.rowsInserted( parent, start, end );
}

void ModelSignalMapperConnector::rowsRemoved( const QModelIndex& parent, int start, int end )
{
    m_mapper.rowsRemoved( parent, start, end );
}
