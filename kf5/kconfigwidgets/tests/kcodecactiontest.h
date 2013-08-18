#ifndef KSELECTACTION_TEST_H
#define KSELECTACTION_TEST_H

#include <QMainWindow>

class KCodecAction;

class CodecActionTest : public QMainWindow
{
    Q_OBJECT

public:
    CodecActionTest(QWidget* parent = 0);

public Q_SLOTS:
    void triggered(QAction* action);
    void triggered(int index);
    void triggered(const QString& text);
    void triggered(QTextCodec *codec);

    void slotActionTriggered(bool state);

private:
    KCodecAction* m_comboCodec;
    KCodecAction* m_buttonCodec;
};

#endif
