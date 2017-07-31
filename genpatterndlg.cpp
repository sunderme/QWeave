#include "genpatterndlg.h"

GenPatternDlg::GenPatternDlg(QWidget *parent):QDialog(parent)
{
    setWindowTitle(tr("Generate Colour Pattern"));
    QLabel *lblPattern=new QLabel(tr("Pattern"));
    cbPattern=new QComboBox();
    cbPattern->addItems(QStringList()<<"A"<<"B"<<"AB"<<"BA"<<"ABBA"<<"BAAB");
    cbPattern->setEditable(true);

    QGridLayout *layGrid=new QGridLayout(this);
    layGrid->addWidget(lblPattern,0,0);
    layGrid->addWidget(cbPattern,0,1);

    QLabel *lblA=new QLabel(tr("Colour A"));
    QLabel *lblB=new QLabel(tr("Colour B"));

    layGrid->addWidget(lblA,1,0);
    layGrid->addWidget(lblB,1,1);

    QToolButton *tbA=new QToolButton();
    tbA->setProperty("nr",0);
    tbA->setObjectName("0");
    QToolButton *tbB=new QToolButton();
    tbB->setProperty("nr",1);
    tbB->setObjectName("1");
    connect(tbA,SIGNAL(clicked()),this,SLOT(chooseColor()));
    connect(tbB,SIGNAL(clicked()),this,SLOT(chooseColor()));

    layGrid->addWidget(tbA,2,0);
    layGrid->addWidget(tbB,2,1);

    QPushButton *pbShafts=new QPushButton(tr("Pattern shaft colours"));
    QPushButton *pbLines=new QPushButton(tr("Pattern line colours"));
    connect(pbShafts,SIGNAL(clicked()),this,SLOT(selShafts()));
    connect(pbLines,SIGNAL(clicked()),this,SLOT(selLines()));

    layGrid->addWidget(pbShafts,3,0);
    layGrid->addWidget(pbLines,3,1);

    // initialize ColorList
    setColours(Qt::white,Qt::black);
}

void GenPatternDlg::setColours(QColor A, QColor B)
{
    lstColors.clear();
    lstColors<<A<<B;
    for(int j=0;j<lstColors.length();j++){
        QPixmap px(20, 20);
        px.fill(lstColors.at(j));
        QToolButton *tb=findChild<QToolButton*>(QString("%1").arg(j));
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
    for(int k=0;k<text.length();k++){
        int d=text.at(k).toUpper().toLatin1()-QChar('A').toLatin1();
        if(d>=0 && d<lstColors.length()){
            result<<d;
        }
    }
    return result;
}

void GenPatternDlg::selShafts()
{
    done(1);
}

void GenPatternDlg::selLines()
{
    done(2);
}

void GenPatternDlg::chooseColor()
{
    QToolButton *tb=qobject_cast<QToolButton*>(sender());
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
