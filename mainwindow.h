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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include "weave.h"
#include "configdialog.h"
#include "genpatterndlg.h"
#include "modifydlg.h"
#include "duplicatpatterndlg.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setupMenu();

    Weave *wv;

    void readSettings();
    void writeSettings();
private slots:
    void config();
    void save();
    void saveas();
    void open();
    void print();
    void resetColour();
    void generateColourPattern();
    void modifySelected();
    void duplicatePattern();
    void updateStatus(int x,int y,int zone);
    void updateRecent();
    void openRecent();
private:
    QString fileName;

    GenPatternDlg *dlgPattern;
    Modifydlg *dlgModify;

    QAction *mUndoAction,*mRedoAction;

    QMenu *recentMenu;

    QStringList recentFilesList;
};

#endif // MAINWINDOW_H
