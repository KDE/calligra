#ifndef HANDLER_H
#define HANDLER_H

#include <qobject.h>

#include "koDocumentChild.h"

class QWMatrix;

class KoView;

class EventHandler : public QObject
{
    Q_OBJECT
public:
    EventHandler( QObject* target );
    ~EventHandler();

    QObject* target();

private:
    QObject* m_target;
};

class PartResizeHandler : public EventHandler
{
    Q_OBJECT
public:
    PartResizeHandler( QWidget* widget, const QWMatrix& matrix, KoDocumentChild* child,
		       KoDocumentChild::Gadget gadget, const QPoint& point );
    ~PartResizeHandler();

protected:
    bool eventFilter( QObject*, QEvent* );

private:
    KoDocumentChild::Gadget m_gadget;
    QPoint m_mouseStart;
    QRect m_geometryStart;
    KoDocumentChild* m_child;
    QWMatrix m_invert;
    QWMatrix m_matrix;
    QWMatrix m_parentMatrix;
    QWMatrix m_invertParentMatrix;
};

class PartMoveHandler : public EventHandler
{
    Q_OBJECT
public:
    PartMoveHandler( QWidget* widget, const QWMatrix& matrix, KoDocumentChild* child,
		     const QPoint& point );
    ~PartMoveHandler();

protected:
    bool eventFilter( QObject*, QEvent* );

private:
    KoDocumentChild* m_dragChild;
    QPoint m_mouseDragStart;
    QRect m_geometryDragStart;
    QPoint m_rotationDragStart;
    QWMatrix m_invertParentMatrix;
    QWMatrix m_parentMatrix;
};

class ContainerHandler : public EventHandler
{
    Q_OBJECT
public:
    ContainerHandler( KoView* view, QWidget* widget );
    ~ContainerHandler();

protected:
    bool eventFilter( QObject*, QEvent* );

private:
    KoView* m_view;
};

#endif
