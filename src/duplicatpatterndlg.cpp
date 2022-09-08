#include "duplicatpatterndlg.h"

DuplicatPatternDlg::DuplicatPatternDlg(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Duplicate Pattern"));
    QLabel *lbl=new QLabel(tr("shift by:"));
    QLabel *lbl2=new QLabel(tr("times:"));
    sp=new QSpinBox();
    sp->setMaximum(20);
    sp->setMinimum(-20);
    sp2=new QSpinBox();
    sp2->setMaximum(20);
    sp2->setMinimum(1);
    auto *hl=new QGridLayout(this);
    hl->addWidget(lbl,0,0);
    hl->addWidget(sp,0,1);
    hl->addWidget(lbl2,1,0);
    hl->addWidget(sp2,1,1);
    QPushButton *pbOK=new QPushButton(tr("OK"));
    connect(pbOK,SIGNAL(clicked()),this,SLOT(accept()));
    QPushButton *pbCancel=new QPushButton(tr("Cancel"));
    connect(pbCancel,SIGNAL(clicked()),this,SLOT(reject()));
    hl->addWidget(pbOK,2,0);
    hl->addWidget(pbCancel,2,1);
}
