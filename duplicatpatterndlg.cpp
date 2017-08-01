#include "duplicatpatterndlg.h"

DuplicatPatternDlg::DuplicatPatternDlg(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Duplicate Pattern"));
    QLabel *lbl=new QLabel(tr("shift by:"));
    sp=new QSpinBox();
    sp->setMaximum(20);
    sp->setMinimum(-20);
    QGridLayout *hl=new QGridLayout(this);
    hl->addWidget(lbl,0,0);
    hl->addWidget(sp,0,1);
    QPushButton *pbOK=new QPushButton(tr("OK"));
    connect(pbOK,SIGNAL(clicked()),this,SLOT(accept()));
    QPushButton *pbCancel=new QPushButton(tr("Cancel"));
    connect(pbCancel,SIGNAL(clicked()),this,SLOT(reject()));
    hl->addWidget(pbOK,1,0);
    hl->addWidget(pbCancel,1,1);
}
