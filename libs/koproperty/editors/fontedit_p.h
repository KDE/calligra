#ifndef KPROPERTY_FONTEDITREQUESTER_H
#define KPROPERTY_FONTEDITREQUESTER_H

#include <QWidget>

class FontEditRequester : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QFont value READ value WRITE setValue USER true)
public:
    FontEditRequester(QWidget* parent);

    QFont value() const
    {
        return m_font;
    }

public slots:
    void setValue(const QFont& value) {
        m_font = value;
    }

signals:
    void commitData( QWidget * editor );

protected slots:
    void slotSelectFontClicked();
    
protected:
    virtual bool event( QEvent * event ) {
        return QWidget::event(event);
    }

    QPushButton *m_button;
    QFont m_font;
    bool m_paletteChangedEnabled;
};

#endif