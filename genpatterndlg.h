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

#ifndef GENPATTERNDLG_H
#define GENPATTERNDLG_H

#include <QDialog>
#include <QtWidgets>

class GenPatternDlg: public QDialog
{
Q_OBJECT

public:
    GenPatternDlg(QWidget *parent);
    void setColours(const QColor& A, const QColor& B, const QColor& C, const QColor& D);
    QList<QColor> getColours();
    QList<int> getPattern();

signals:
    void generateColourPattern(QList<QColor> colors,QList<int> pattern,int side);

private slots:
    void selShafts();
    void selLines();
    void chooseColor();

protected:
    QList<QColor> lstColors;

    QComboBox *cbPattern;
};

#endif // GENPATTERNDLG_H
