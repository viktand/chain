#include "helpwnd.h"
#include "ui_helpwnd.h"

HelpWnd::HelpWnd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpWnd)
{
    ui->setupUi(this);
}

HelpWnd::~HelpWnd()
{
    delete ui;
}

void HelpWnd::on_pushButton_clicked()
{
    emit clsw();
    this->close();
}
