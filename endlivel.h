#ifndef ENDLIVEL_H
#define ENDLIVEL_H

#include <QDialog>

namespace Ui {
class EndLivel;
}

class EndLivel : public QDialog
{
    Q_OBJECT

public:
    explicit EndLivel(QWidget *parent = 0);
    ~EndLivel();



private:
    Ui::EndLivel *ui;

signals:
    void goNow();    // Дальше
    void toHome();   // на начальный экран

protected:
    virtual void showEvent(QShowEvent *event);


private slots:
    void on_pushButton_clicked();
    void sound();
    void on_pushButton_2_clicked();
};

#endif // ENDLIVEL_H
