#include "genpatterndlg.h"

GenPatternDlg::GenPatternDlg(QWidget *parent):QDialog(parent)
{
    setWindowTitle(tr("Generate Colour Pattern"));
    QLabel *lblPattern=new QLabel(tr("Pattern"));
    cbPattern=new QComboBox();
    cbPattern->addItems(QStringList()<<"A"<<"B"<<"AB"<<"BA"<<"ABBA"<<"BAAB"<<"ABCD");
    cbPattern->setEditable(true);

    auto *layGrid=new QGridLayout(this);
    layGrid->addWidget(lblPattern,0,0);
    layGrid->addWidget(cbPattern,0,1);

    QLabel *lblA=new QLabel(tr("Colour A"));
    QLabel *lblB=new QLabel(tr("Colour B"));
    QLabel *lblC=new QLabel(tr("Colour C"));
    QLabel *lblD=new QLabel(tr("Colour D"));

    layGrid->addWidget(lblA,1,0);
    layGrid->addWidget(lblB,1,1);
    layGrid->addWidget(lblC,1,2);
    layGrid->addWidget(lblD,1,3);

    auto *tbA=new QToolButton();
    tbA->setProperty("nr",0);
    tbA->setObjectName("0");
    auto *tbB=new QToolButton();
    tbB->setProperty("nr",1);
    tbB->setObjectName("1");
    auto *tbC=new QToolButton();
    tbC->setProperty("nr",2);
    tbC->setObjectName("2");
    auto *tbD=new QToolButton();
    tbD->setProperty("nr",3);
    tbD->setObjectName("3");
    connect(tbA,SIGNAL(clicked()),this,SLOT(chooseColor()));
    connect(tbB,SIGNAL(clicked()),this,SLOT(chooseColor()));
    connect(tbC,SIGNAL(clicked()),this,SLOT(chooseColor()));
    connect(tbD,SIGNAL(clicked()),this,SLOT(chooseColor()));

    layGrid->addWidget(tbA,2,0);
    layGrid->addWidget(tbB,2,1);
    layGrid->addWidget(tbC,2,2);
    layGrid->addWidget(tbD,2,3);

    QPushButton *pbShafts=new QPushButton(tr("Pattern shaft colours"));
    QPushButton *pbLines=new QPushButton(tr("Pattern line colours"));
    connect(pbShafts,SIGNAL(clicked()),this,SLOT(selShafts()));
    connect(pbLines,SIGNAL(clicked()),this,SLOT(selLines()));

    layGrid->addWidget(pbShafts,3,0);
    layGrid->addWidget(pbLines,3,1);

    // initialize ColorList
    setColours(Qt::white,Qt::black,Qt::red,Qt::blue);
}

void GenPatternDlg::setColours(const QColor& A, const QColor& B, const QColor& C, const QColor& D)
{
    lstColors.clear();
    lstColors<<A<<B<<C<<D;
    for(int j=0;j<lstColors.length();j++){
        QPixmap px(20, 20);
        px.fill(lstColors.at(j));
        auto *tb=findChild<QToolButton*>(QString("%1").arg(j));
        if(tb){
            tb->setIcon(px);
        }
    }
}

QList<QColor> GenPatternDlg::getColours()
{
    return lstColors;
}

QList<int> GenPatternDlg::getPattern()
{
    QList<int> result;
    QString text=cbPattern->currentText();
    for(auto ch : text){
        int d=ch.toUpper().toLatin1()-QChar('A').toLatin1();
        if(d>=0 && d<lstColors.length()){
            result<<d;
        }
    }
    return result;
}

void GenPatternDlg::selShafts()
{
    emit generateColourPattern(lstColors,getPattern(),1);
}

void GenPatternDlg::selLines()
{
    emit generateColourPattern(lstColors,getPattern(),2);
}

void GenPatternDlg::chooseColor()
{
    auto *tb=qobject_cast<QToolButton*>(sender());
    if(!tb)
        return;
    int p=tb->property("nr").toInt();
    QColorDialog dlg;
    dlg.setCurrentColor(lstColors.at(p));
    if(dlg.exec()){
        lstColors[p]=dlg.currentColor();
        QPixmap px(20, 20);
        px.fill(dlg.currentColor());
        tb->setIcon(px);
    }
}
