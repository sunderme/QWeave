#include "modifydlg.h"

Modifydlg::Modifydlg(QWidget *parent) : QDialog(parent)
{
    QPushButton *btFlipHorz=new QPushButton(tr("mirror horizontal"));
    connect(btFlipHorz,SIGNAL(clicked()),this,SIGNAL(flipHorz()));
    QPushButton *btFlipVert=new QPushButton(tr("mirror vertical"));
    connect(btFlipVert,SIGNAL(clicked()),this,SIGNAL(flipVert()));
    QToolButton *tbUp=new QToolButton();
    tbUp->setObjectName("up");
    tbUp->setIcon(QIcon(":/up.svgz"));
    connect(tbUp,SIGNAL(clicked()),this,SLOT(movePressed()));
    QToolButton *tbDown=new QToolButton();
    tbDown->setObjectName("down");
    tbDown->setIcon(QIcon(":/down.svgz"));
    connect(tbDown,SIGNAL(clicked()),this,SLOT(movePressed()));
    QToolButton *tbRight=new QToolButton();
    tbRight->setObjectName("right");
    tbRight->setIcon(QIcon(":/right.svgz"));
    connect(tbRight,SIGNAL(clicked()),this,SLOT(movePressed()));
    QToolButton *tbLeft=new QToolButton();
    tbLeft->setObjectName("left");
    tbLeft->setIcon(QIcon(":/left.svgz"));
    connect(tbLeft,SIGNAL(clicked()),this,SLOT(movePressed()));

    QGridLayout *layGrid=new QGridLayout(this);
    layGrid->addWidget(btFlipHorz,0,0,1,3);
    layGrid->addWidget(btFlipVert,1,0,1,3);
    layGrid->addWidget(tbUp,2,1);
    layGrid->addWidget(tbLeft,3,0);
    layGrid->addWidget(tbRight,3,2);
    layGrid->addWidget(tbDown,4,1);

}

void Modifydlg::movePressed()
{
    QToolButton *tb=qobject_cast<QToolButton*>(sender());
    if(tb){
        // get direction 0 up,1 right, 2 down, 3 left
        int direction=0;
        QString name=tb->objectName();
        if(name=="right")
            direction=1;
        if(name=="down")
            direction=2;
        if(name=="left")
            direction=3;
        emit move(direction);
    }
}
