#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QtCore/QTextCodec>

#include <QDebug>

#include "kcodecactiontest.h"

#include <kcodecaction.h>

int main( int argc, char **argv )
{
    QApplication::setApplicationName("kcodecactiontest");
    QApplication app(argc, argv);

    CodecActionTest* test = new CodecActionTest;
    test->show();

    return app.exec();
}

CodecActionTest::CodecActionTest(QWidget *parent)
    : QMainWindow(parent)
    , m_comboCodec(new KCodecAction("Combo Codec Action", this))
    , m_buttonCodec(new KCodecAction("Button Codec Action", this))
{
    m_comboCodec->setToolBarMode(KCodecAction::ComboBoxMode);
    connect(m_comboCodec, SIGNAL(triggered(QAction*)), SLOT(triggered(QAction*)));
    connect(m_comboCodec, SIGNAL(triggered(int)), SLOT(triggered(int)));
    connect(m_comboCodec, SIGNAL(triggered(QString)), SLOT(triggered(QString)));
    connect(m_comboCodec, SIGNAL(triggered(QTextCodec*)), SLOT(triggered(QTextCodec*)));

    m_buttonCodec->setToolBarMode(KCodecAction::MenuMode);
    connect(m_buttonCodec, SIGNAL(triggered(QAction*)), SLOT(triggered(QAction*)));
    connect(m_buttonCodec, SIGNAL(triggered(int)), SLOT(triggered(int)));
    connect(m_buttonCodec, SIGNAL(triggered(QString)), SLOT(triggered(QString)));
    connect(m_buttonCodec, SIGNAL(triggered(QTextCodec*)), SLOT(triggered(QTextCodec*)));

    menuBar()->addAction(m_comboCodec);
    menuBar()->addAction(m_buttonCodec);

    QToolBar* toolBar = addToolBar("Test");
    toolBar->addAction(m_comboCodec);
    toolBar->addAction(m_buttonCodec);
}

void CodecActionTest::triggered(QAction* action)
{
  qDebug() << action;
}

void CodecActionTest::triggered(int index)
{
  qDebug() << index;
}

void CodecActionTest::triggered(const QString& text)
{
  qDebug() << '"' << text << '"';
}

void CodecActionTest::triggered(QTextCodec *codec)
{
  qDebug() << codec->name() << ':' << codec->mibEnum();
}

void CodecActionTest::slotActionTriggered(bool state)
{
  qDebug() << sender() << " state " << state;
}


