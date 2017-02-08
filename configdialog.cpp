/*  QWeave is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Copyright  Jan Sundermeyer    2017
    */

#include <QtWidgets>

#include "configdialog.h"

configDialog::configDialog(QWidget *parent):QDialog(parent)
{
    QLabel *lblCols=new QLabel(tr("Columns"));
    sbCols=new QSpinBox();
    sbCols->setMinimum(2);
    sbCols->setMaximum(1000);
    lblCols->setBuddy(sbCols);

    QLabel *lblLines=new QLabel(("Lines"));
    sbLines=new QSpinBox();
    sbLines->setMinimum(2);
    sbLines->setMaximum(1000);
    lblLines->setBuddy(sbLines);

    QLabel *lblShafts=new QLabel(("Shafts"));
    sbShafts=new QSpinBox();
    sbShafts->setMinimum(2);
    sbShafts->setMaximum(100);
    lblShafts->setBuddy(sbShafts);

    QLabel *lblPositions=new QLabel(("Positions"));
    sbPositions=new QSpinBox();
    sbPositions->setMinimum(2);
    sbPositions->setMaximum(100);
    lblPositions->setBuddy(sbPositions);

    QPushButton *btUp=new QPushButton("up");
    QPushButton *btDown=new QPushButton("down");

    connect(btUp,SIGNAL(clicked()),this,SLOT(chooseColorUp()));
    connect(btDown,SIGNAL(clicked()),this,SLOT(chooseColorDown()));

    QPushButton *btOK=new QPushButton(("OK"));
    QPushButton *btCancel=new QPushButton(("Cancel"));

    connect(btOK,SIGNAL(clicked(bool)),this,SLOT(accept()));
    connect(btCancel,SIGNAL(clicked(bool)),this,SLOT(reject()));

    QGridLayout *layGrid=new QGridLayout;
    layGrid->addWidget(lblCols,0,0,1,1);
    layGrid->addWidget(sbCols,0,1,1,1);
    layGrid->addWidget(lblLines,1,0,1,1);
    layGrid->addWidget(sbLines,1,1,1,1);
    layGrid->addWidget(lblShafts,2,0,1,1);
    layGrid->addWidget(sbShafts,2,1,1,1);
    layGrid->addWidget(lblPositions,3,0,1,1);
    layGrid->addWidget(sbPositions,3,1,1,1);
    layGrid->addWidget(btUp,4,0,1,1);
    layGrid->addWidget(btDown,4,1,1,1);

    QHBoxLayout *hlayout= new QHBoxLayout;
    hlayout->addStretch();
    hlayout->addWidget(btOK);
    hlayout->addWidget(btCancel);

    QVBoxLayout *vlayout= new QVBoxLayout;
    vlayout->addLayout(layGrid);
    vlayout->addStretch();
    vlayout->addLayout(hlayout);

    setLayout(vlayout);
}

int configDialog::getCols()
{
    return sbCols->value();
}

void configDialog::setCols(int cols)
{
    sbCols->setValue(cols);
}

int configDialog::getLines()
{
    return sbLines->value();
}

void configDialog::setLines(int lines)
{
    sbLines->setValue(lines);
}

int configDialog::getShafts()
{
    return sbShafts->value();
}

void configDialog::setShafts(int shafts)
{
    sbShafts->setValue(shafts);
}

int configDialog::getPositions()
{
    return sbPositions->value();
}

void configDialog::setPositions(int positions)
{
    sbPositions->setValue(positions);
}

void configDialog::setColors(QColor up, QColor down)
{
    m_up=up;
    m_down=down;
}

void configDialog::getColors(QColor &up, QColor &down)
{
    up=m_up;
    down=m_down;
}

void configDialog::chooseColorUp()
{
    QColorDialog dlg;
    dlg.setCurrentColor(m_up);
    if(dlg.exec()){
        m_up=dlg.currentColor();
    }
}

void configDialog::chooseColorDown()
{
    QColorDialog dlg;
    dlg.setCurrentColor(m_down);
    if(dlg.exec()){
        m_down=dlg.currentColor();
    }
}
