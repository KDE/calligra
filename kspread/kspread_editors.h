#ifndef __kspread_editors_h__
#define __kspread_editors_h__

#include <kcompletion.h>
#include <qwidget.h>
class KSpreadCell;
class KSpreadCanvas;


class QFont;
class KLineEdit;
class KSpreadCellEditor : public QWidget
{
    Q_OBJECT
public:
    KSpreadCellEditor( KSpreadCell*, KSpreadCanvas* _parent = 0, const char* _name = 0 );
    ~KSpreadCellEditor();

    KSpreadCell* cell()const { return m_pCell; }

    virtual void handleKeyPressEvent( QKeyEvent* _ev ) = 0;
    virtual void setEditorFont(QFont const & font, bool updateSize) = 0;
    virtual QString text() const = 0;
    virtual void setText(QString text) = 0;
    virtual int cursorPosition() const = 0;
    virtual void setCursorPosition(int pos) = 0;
    // virtual void setFocus() = 0;
    virtual void insertFormulaChar(int c) = 0;
    virtual void cut(){};
    virtual void paste(){};
    virtual void copy(){};
    KSpreadCanvas* canvas()const { return m_pCanvas; }

private:
    KSpreadCell* m_pCell;
    KSpreadCanvas* m_pCanvas;
};

class KSpreadTextEditor : public KSpreadCellEditor
{
    Q_OBJECT
public:
    KSpreadTextEditor( KSpreadCell*, KSpreadCanvas* _parent = 0, const char* _name = 0 );
    ~KSpreadTextEditor();

    virtual void handleKeyPressEvent( QKeyEvent* _ev );
    virtual void setEditorFont(QFont const & font, bool updateSize);
    virtual QString text() const;
    virtual void setText(QString text);
    virtual int cursorPosition() const;
    virtual void setCursorPosition(int pos);
    // virtual void setFocus();
    virtual void insertFormulaChar(int c);
    virtual void cut();
    virtual void paste();
    virtual void copy();
    bool checkChoose();
    void blockCheckChoose( bool b ) { m_blockCheck = b; }
    bool sizeUpdate() const { return m_sizeUpdate; }

private slots:
    void slotTextChanged( const QString& text );
    void  slotCompletionModeChanged(KGlobalSettings::Completion _completion);
protected:
    void resizeEvent( QResizeEvent* );
    /**
     * Steals some key events from the QLineEdit and sends
     * it to the @ref KSpreadCancvas ( its parent ) instead.
     */
    bool eventFilter( QObject* o, QEvent* e );

private:
    //QLineEdit* m_pEdit;
    KLineEdit* m_pEdit;
    bool m_blockCheck;
    bool m_sizeUpdate;
    uint m_length;
    int  m_fontLength;
};


#endif
