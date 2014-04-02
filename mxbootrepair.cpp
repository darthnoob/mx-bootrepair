#include "mxbootrepair.h"
#include "ui_mxbootrepair.h"

mxbootrepair::mxbootrepair(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mxbootrepair)
{
    ui->setupUi(this);
}

mxbootrepair::~mxbootrepair()
{
    delete ui;
}
