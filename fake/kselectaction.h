#ifndef FAKE_KSELECTACTION_H
#define FAKE_KSELECTACTION_H

#include <kaction.h>
#include <QActionGroup>
#include <QMenu>
#include "kofake_export.h"
class KOFAKE_EXPORT KSelectAction : public KAction
{
    Q_OBJECT
public:
    KSelectAction(QObject *parent = 0) : KAction(parent) { init(); }
    KSelectAction(const QString &text, QObject *parent) : KAction(text, parent) { init(); }
    KSelectAction(const KIcon &icon, const QString &text, QObject *parent) : KAction(icon, text, parent) { init(); }
    virtual ~KSelectAction() { delete m_menu; }

    void clear() {
        while(!m_group->actions().isEmpty())
            removeAction(m_group->actions().first())->deleteLater();
    }
    QAction* removeAction(QAction *a) {
        menu()->removeAction(a);
        m_group->removeAction(a);
        return a;
    }

    void addAction(QAction *a) { menu()->addAction(a); m_group->addAction(a); }
    KAction* addAction(const QString &t) {
        KAction *a = new KAction(t, this);
        a->setCheckable(true);
        addAction(a);
        return a;
    }

    QAction * action(int i) const { return menu()->actions().at(i); }
    QAction* action(const QString& text, Qt::CaseSensitivity cs = Qt::CaseSensitive) {
        QString compare = cs == Qt::CaseSensitive ? text : text.toLower();
        foreach(QAction* a, menu()->actions()) {
            if (cs == Qt::CaseSensitive) {
                if (a->text() == compare)
                    return a;
            } else if (cs == Qt::CaseInsensitive) {
                if (a->text().toLower() == compare)
                    return a;
            }
        }
        return 0;
    }
    QList<QAction*> actions() const { return menu()->actions(); }

    QAction* currentAction() const { return m_group->checkedAction(); }
    QString currentText() const { return currentAction() ? currentAction()->text() : QString(); }

    void setCurrentAction(QAction *a) {
        if (a)
            a->setChecked(true);
        else if (currentAction())
            currentAction()->setChecked(false);
    }
    void setCurrentItem(int i) { setCurrentAction(i > -1 && i < actions().count() ? actions().at(i) : 0); }
    void setCurrentAction(const QString &text) { setCurrentItem(items().indexOf(text)); }

    QStringList items() const {
        QStringList r;
        foreach(QAction* action, menu()->actions())
            r.append(action->text());
        return r;
    }
    void setItems(const QStringList &l) {
        foreach(const QString& string, l)
            if (!string.isEmpty())
                addAction(string);
        setEnabled(!l.isEmpty() || m_isEditable);
    }

    bool isEditable() const { return m_isEditable; }
    void setEditable(bool e) { m_isEditable = e; }

    int comboWidth() const { return 100; }
    void setComboWidth( int width ) {}

    void setMaxComboViewCount(int) {}

Q_SIGNALS:
    void triggered( QAction* action );
    void triggered( int index );
    void triggered( const QString& text );

protected Q_SLOTS:
    virtual void actionTriggered(QAction* action) {
        if (isCheckable()) trigger();
        emit triggered(action);
        emit triggered(m_group->actions().indexOf(action));
        emit triggered(action->text());
    }
    void slotToggled(bool checked) {
        if (!checked && currentAction())
            currentAction()->setChecked(false);
    }

private:
    QActionGroup *m_group;
    bool m_isEditable;
    QMenu *m_menu;

    void init() {
        m_group = new QActionGroup(this);
        m_group->setExclusive(true);
        m_isEditable = false;
        m_menu = new QMenu();
        setMenu(m_menu);
        QObject::connect(m_group, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
        QObject::connect(this, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
    }
};

#endif
