#ifndef SELECTLIVEL_H
#define SELECTLIVEL_H

#include <QDialog>

namespace Ui {
class SelectLivel;
}

class SelectLivel : public QDialog
{
    Q_OBJECT

public:
    explicit SelectLivel(QWidget *parent = 0);
    ~SelectLivel();

private:
    Ui::SelectLivel *ui;

signals:
    void select(int index);         // выбор уровня

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_10_clicked();
};

#endif // SELECTLIVEL_H
