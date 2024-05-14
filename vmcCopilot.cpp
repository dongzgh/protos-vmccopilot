#include "vmcCopilot.h"
#include "ui_vmcCopilot.h"

vmcCopilot::vmcCopilot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::vmcCopilot)
{
    ui->setupUi(this);
}

vmcCopilot::~vmcCopilot()
{
    delete ui;
}