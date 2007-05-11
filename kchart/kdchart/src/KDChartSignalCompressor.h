#ifndef KDCHARTSIGNALCOMPRESSOR_H
#define KDCHARTSIGNALCOMPRESSOR_H

#include <QObject>
#include <QTimer>

namespace KDChart {

    /** SignalCompressor compresses signals where the same signal
        needs to be emitted by several pieces of the code, but only
        one of the signals should be received at the end.
        Usage:
        * create a object of SignalCompressor, and give it the name
          and object of the signal it is supposed to manage
        * instead of emitting the signal, call emitSignal() on the compressor
        * the signal will only be emitted once, and that is after the
          current call stack ends and returns to the event loop

        With the current implementation, the class changes the
        sematics of signals to be a queued connection. If that is not
        wanted, another compression algorithm needs to be
        implemented.
        Also, at the moment, only nullary signals are supported, as
        parameters could not be compressed.
        A typical use of the class is to compress update
        notifications.
        This class is not part of the published KDChart API.
    */
    class SignalCompressor : public QObject
    {
        Q_OBJECT

    public:
        SignalCompressor( QObject* receiver, const char* signal,
                          QObject* parent = 0 );

    Q_SIGNALS:
        void finallyEmit();

    public Q_SLOTS:
        void emitSignal(); // emit() won't work, because of stupid defines

    private Q_SLOTS:
        void nowGoAlready();

    private:
        QTimer m_timer;
    };

}

#endif
