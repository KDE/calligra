#ifndef KPRSELECTIONTOGGLE_H
#define KPRSELECTIONTOGGLE_H

#include <QAbstractButton>
#include <QPixmap>

class KoPAPageBase;


class QTimeLine;

/**
 * @brief Toggle button for changing the selection of an hovered item.
 *
 * The toggle button is visually invisible until it is displayed at least
 * for one second.
 *
 * @see SelectionManager
 */
class KPrSelectionToggle : public QAbstractButton
{
    Q_OBJECT

public:
    explicit KPrSelectionToggle(QWidget* parent);
    virtual ~KPrSelectionToggle();
    virtual QSize sizeHint() const;

    /**
     * Resets the selection toggle so that it is hidden and stays
     * visually invisible for at least one second after it is shown again.
     */
    void reset();

    void setPage(KoPAPageBase* page);
    KoPAPageBase* page() const;

    /**
     * Sets the margin around the selection-icon in pixels. Per default
     * the value is 0.
     */
    void setMargin(int margin);
    int margin() const;

public slots:
    virtual void setVisible(bool visible);

protected:
    virtual bool eventFilter(QObject* obj, QEvent* event);
    virtual void enterEvent(QEvent* event);
    virtual void leaveEvent(QEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void paintEvent(QPaintEvent* event);

private slots:
    /**
     * Sets the alpha value for the fading animation and is
     * connected with m_fadingTimeLine.
     */
    void setFadingValue(int value);

    void setIconOverlay(bool checked);
    void refreshIcon();

private:
    void startFading();
    void stopFading();

private:
    bool m_isHovered;
    bool m_leftMouseButtonPressed;
    int m_fadingValue;
    int m_margin;
    QPixmap m_icon;
    QTimeLine* m_fadingTimeLine;
    KoPAPageBase* m_page;
};

#endif // KPRSELECTIONTOGGLE_H
