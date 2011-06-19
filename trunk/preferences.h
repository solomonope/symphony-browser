#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>

namespace Ui {
    class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();
    void doLoad();

private slots:
    void on_homePageBtn_clicked();
    void on_restoreTabsBtn_clicked();
    void on_buttonBox_accepted();

private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H
