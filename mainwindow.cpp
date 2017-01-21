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
    : QMainWindow(parent)
{
   QScrollArea *sc=new QScrollArea;
   wv=new Weave(this);
   sc->setWidget(wv);
   setCentralWidget(sc);
   setupMenu();
   readSettings();
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
    menu=menuBar()->addMenu("&Edit");
    menu->addAction("&Move",wv,SLOT(move()),QKeySequence(Qt::Key_M));
    menu->addAction("&Copy",wv,SLOT(copy()),QKeySequence(Qt::Key_C));
    menu->addAction("C&lear",wv,SLOT(clear()),QKeySequence(Qt::Key_Delete));
    menu->addAction("Mirror &X",wv,SLOT(mirror_x()),QKeySequence(Qt::Key_X));
    menu->addAction("Mirror &Y",wv,SLOT(mirror_y()),QKeySequence(Qt::Key_Y));
    menu=menuBar()->addMenu("&Options");
    menu->addAction("&Config",this,SLOT(config()));

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
    if(dlg->exec()){
        wv->resizeWeave(dlg->getLines(),dlg->getCols(),dlg->getShafts(),dlg->getPositions());
        dlg->getColors(wv->clrUp,wv->clrDown);
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
