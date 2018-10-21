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
class QCheckBox;

class configDialog : public QDialog
{

Q_OBJECT

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
    void setColors(const QColor &up, const QColor &down);
    void getColors(QColor &up,QColor &down);
    void setMultiShaft(bool multiShaft);
    bool getMultiShaft();
    void setMultiLines(bool multiShaft);
    bool getMultiLines();

public slots:
    void chooseColorUp();
    void chooseColorDown();
private:
    QSpinBox *sbCols,*sbLines,*sbShafts,*sbPositions;
    QCheckBox *cbMultiShafts,*cbMultiLines;
    QColor m_up,m_down;
};

#endif // CONFIGDIALOG_H
