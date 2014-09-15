#include "endlivel.h"
#include "ui_endlivel.h"
#include <QSound>

QSound  *fanfare;           // звук конца уровня

EndLivel::EndLivel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EndLivel)
{
    ui->setupUi(this);
    this->setWindowTitle("Chain");
    fanfare=new QSound(QString(":/new/prefix1/fanfare"));
}

EndLivel::~EndLivel()
{
    delete ui;
}

void EndLivel::on_pushButton_clicked()
{
    emit goNow();
    this->close();
}

void EndLivel::sound()
{
    fanfare->play();
}

void EndLivel::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    fanfare->play();
}

void EndLivel::on_pushButton_2_clicked()
{
    emit toHome();
    this->close();
}
