#ifndef HELPWND_H
#define HELPWND_H

#include <QDialog>

namespace Ui {
class HelpWnd;
}

class HelpWnd : public QDialog
{
    Q_OBJECT

public:
    explicit HelpWnd(QWidget *parent = 0);
    ~HelpWnd();

private:
    Ui::HelpWnd *ui;

signals:
    void clsw();      // закрытие окна

private slots:
    void on_pushButton_clicked();
};

#endif // HELPWND_H
