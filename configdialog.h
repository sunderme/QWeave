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

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H
#include <QDialog>

class QSpinBox;

class configDialog : public QDialog
{

public:
    configDialog(QWidget *parent=0);
    int getCols();
    void setCols(int cols);
    int getLines();
    void setLines(int lines);
    int getShafts();
    void setShafts(int shafts);
    int getPositions();
    void setPositions(int positions);
private:
    QSpinBox *sbCols,*sbLines,*sbShafts,*sbPositions;
};

#endif // CONFIGDIALOG_H
