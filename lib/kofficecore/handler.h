#ifndef HANDLER_H
#define HANDLER_H

#include <qobject.h>
#include <koDocumentChild.h>

class QWMatrix;

class KoView;
class PartResizeHandlerPrivate;
class PartMoveHandlerPrivate;

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
    PartResizeHandler( QWidget* widget, const QWMatrix& matrix, KoChild* child,
		       KoChild::Gadget gadget, const QPoint& point );
    ~PartResizeHandler();

protected:
    bool eventFilter( QObject*, QEvent* );

private:
    PartResizeHandlerPrivate *d;
};

class PartMoveHandler : public EventHandler
{
    Q_OBJECT
public:
    PartMoveHandler( QWidget* widget, const QWMatrix& matrix, KoChild* child,
		     const QPoint& point );
    ~PartMoveHandler();

protected:
    bool eventFilter( QObject*, QEvent* );

private:
    PartMoveHandlerPrivate *d;
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
