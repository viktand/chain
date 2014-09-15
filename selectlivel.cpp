#include "selectlivel.h"
#include "ui_selectlivel.h"

SelectLivel::SelectLivel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectLivel)
{
    ui->setupUi(this);
}

SelectLivel::~SelectLivel()
{
    delete ui;
}


void SelectLivel::on_pushButton_clicked()
{
    emit select(1);
    this->close();
}

void SelectLivel::on_pushButton_2_clicked()
{
    emit select(2);
    this->close();
}

void SelectLivel::on_pushButton_3_clicked()
{
    emit select(3);
    this->close();
}

void SelectLivel::on_pushButton_4_clicked()
{
    emit select(4);
    this->close();
}

void SelectLivel::on_pushButton_5_clicked()
{
    emit select(5);
    this->close();
}

void SelectLivel::on_pushButton_6_clicked()
{
    emit select(6);
    this->close();
}

void SelectLivel::on_pushButton_7_clicked()
{
    emit select(7);
    this->close();
}

void SelectLivel::on_pushButton_8_clicked()
{
    emit select(8);
    this->close();
}

void SelectLivel::on_pushButton_9_clicked()
{
    emit select(9);
    this->close();
}

void SelectLivel::on_pushButton_10_clicked()
{
    emit select(10);
    this->close();
}
