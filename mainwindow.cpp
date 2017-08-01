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

#include "mainwindow.h"
#include <QMenu>
#include <QMenuBar>

#include <QSettings>
#include <QFileDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <QPainter>

#include <QScrollArea>

#include <QShortcut>




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),dlgPattern(0),dlgModify(0)
{
   QScrollArea *sc=new QScrollArea;
   wv=new Weave(this);
   sc->setWidget(wv);
   setCentralWidget(sc);
   setupMenu();
   readSettings();
   setStatusBar(new QStatusBar());
   connect(wv,SIGNAL(currentPosition(int,int,int)),this,SLOT(updateStatus(int,int,int)));
}

MainWindow::~MainWindow()
{
    writeSettings();
}

void MainWindow::setupMenu()
{
    QMenu *menu=menuBar()->addMenu(tr("&File"));
    menu->addAction(tr("&Open"),this,SLOT(open()));
    menu->addAction(tr("&Save"),this,SLOT(save()));
    menu->addAction(tr("Save &as"),this,SLOT(saveas()));
    menu->addAction(tr("&Print"),this,SLOT(print()));
    menu->addAction(tr("&Quit"),this,SLOT(close()));
    menu=menuBar()->addMenu(tr("&Edit"));
    mUndoAction=menu->addAction(tr("&Undo"),wv,SLOT(undo()),QKeySequence(Qt::ControlModifier+Qt::Key_Z));
    mRedoAction=menu->addAction(tr("&Redo"),wv,SLOT(redo()),QKeySequence(Qt::ControlModifier+Qt::ShiftModifier+Qt::Key_Z));
    menu->addAction(tr("&Move"),wv,SLOT(move()),QKeySequence(Qt::Key_M));
    menu->addAction(tr("&Copy"),wv,SLOT(copy()),QKeySequence(Qt::Key_C));
    menu->addAction(tr("C&lear"),wv,SLOT(clear()),QKeySequence(Qt::Key_Delete));
    menu->addAction(tr("Mirror &X"),wv,SLOT(mirror_x()),QKeySequence(Qt::Key_X));
    menu->addAction(tr("Mirror &Y"),wv,SLOT(mirror_y()),QKeySequence(Qt::Key_Y));
    menu->addAction(tr("Generate colour &pattern"),this,SLOT(generateColourPattern()));
    menu->addAction(tr("M&odify pattern"),this,SLOT(modifySelected()));
    menu->addAction(tr("&Duplicate pattern"),this,SLOT(duplicatePattern()));
    menu->addSeparator();
    menu->addAction(tr("&Analyze pattern"),wv,SLOT(analyzePattern()));
    menu=menuBar()->addMenu(tr("&Options"));
    menu->addAction(tr("&Config"),this,SLOT(config()));
    menu->addAction(tr("&Reset Colours"),this,SLOT(resetColour()));
}

void MainWindow::readSettings()
{
    QSettings settings("sdm","QWeave");
    int nrLines=settings.value("nrLines",4).toInt();
    int nrCols=settings.value("nrCols",4).toInt();
    int nrShafts=settings.value("nrShafts",4).toInt();
    int nrPositions=settings.value("nrPositions",4).toInt();
    wv->resizeWeave(nrLines,nrCols,nrShafts,nrPositions);
    wv->clrUp=settings.value("Colour_up",QColor(Qt::blue)).value<QColor>();
    wv->clrDown=settings.value("Colour_down",QColor(Qt::green)).value<QColor>();
    resetColour();
}

void MainWindow::writeSettings()
{
    QSettings settings("sdm","QWeave");
    settings.setValue("nrLines",wv->nrLines);
    settings.setValue("nrCols",wv->nrCols);
    settings.setValue("nrShafts",wv->nrShafts);
    settings.setValue("nrPositions",wv->nrPositions);
    settings.setValue("Colour_up",wv->clrUp);
    settings.setValue("Colour_down",wv->clrDown);
}

void MainWindow::config()
{
    configDialog *dlg=new configDialog(this);
    dlg->setLines(wv->nrLines);
    dlg->setCols(wv->nrCols);
    dlg->setShafts(wv->nrShafts);
    dlg->setPositions(wv->nrPositions);
    dlg->setColors(wv->clrUp,wv->clrDown);
    dlg->setMultiShaft(wv->exclusiveShaft);
    dlg->setMultiLines(wv->exclusiveLines);
    if(dlg->exec()){
        wv->resizeWeave(dlg->getLines(),dlg->getCols(),dlg->getShafts(),dlg->getPositions());
        dlg->getColors(wv->clrUp,wv->clrDown);
        wv->exclusiveShaft=dlg->getMultiShaft();
        wv->exclusiveLines=dlg->getMultiLines();
    }
    delete dlg;
}

void MainWindow::save()
{
    if(fileName.isEmpty()){
        saveas();
        return;
    }
    wv->save(fileName);
}

void MainWindow::saveas()
{
    fileName = QFileDialog::getSaveFileName(this,
          tr("Open Weave"), "", tr("QWeave Files (*.weave)"));
    if(!fileName.isEmpty())
        save();
}

void MainWindow::open()
{
    fileName = QFileDialog::getOpenFileName(this,
          tr("Open Weave"), "", tr("QWeave Files (*.weave)"));
    if(!fileName.isEmpty()){
        wv->open(fileName);
    }
}

void MainWindow::print()
{
    QPrinter *printer=new QPrinter(QPrinter::HighResolution);
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        // print ...
        QPainter painter;
        painter.begin(printer);
        wv->paint(painter,60);
        painter.end();
    }
}

void MainWindow::resetColour()
{
    wv->colColors.fill(wv->clrUp);
    wv->lineColors.fill(wv->clrDown);
    wv->update();
}

void MainWindow::generateColourPattern()
{
    if(!dlgPattern){
        dlgPattern=new GenPatternDlg(this);
        connect(dlgPattern,SIGNAL(generateColourPattern(QList<QColor>,QList<int>,int)),wv,SLOT(generateColourPattern(QList<QColor>,QList<int>,int)));
    }
    dlgPattern->show();
    dlgPattern->raise();
    dlgPattern->activateWindow();
}

void MainWindow::modifySelected()
{
    if(!dlgModify){
        dlgModify=new Modifydlg(this);
        connect(dlgModify,SIGNAL(flipHorz()),wv,SLOT(mirror_x()));
        connect(dlgModify,SIGNAL(flipVert()),wv,SLOT(mirror_y()));
        connect(dlgModify,SIGNAL(move(int)),wv,SLOT(modifySelected(int)));
    }
    dlgModify->show();
    dlgModify->raise();
    dlgModify->activateWindow();
}

void MainWindow::duplicatePattern()
{
    DuplicatPatternDlg *duplicatePatternDlg=new DuplicatPatternDlg(this);
    if(duplicatePatternDlg->exec()){
        int shift=duplicatePatternDlg->sp->value();
        wv->duplicatePattern(-shift);
    }
    delete duplicatePatternDlg;
}

void MainWindow::updateStatus(int x, int y, int zone)
{
    QString text;
    if(zone>=0){
        text=QString("Position: %1 %2").arg(y).arg(x);
    }
    statusBar()->showMessage(text);
}
