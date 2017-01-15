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

#ifndef WEAVE_H
#define WEAVE_H

#include <QWidget>
#include <QPaintEvent>
#include <QBitArray>
typedef QList<QBitArray> bitField;

class Weave : public QWidget
{
    Q_OBJECT
public:
    explicit Weave(QWidget *parent = 0);

    void init();
    bitField lines,translation,shafts,positions;
    int nrLines,nrCols;
    int scale,offset;
    int nrShafts,nrPositions;
    int xDist,yDist;
    bool exclusiveShaft;

    void resizeWeave(int newLines, int newCols, int newShafts, int newPos);
    void generateWeave();
    void generatePattern();
    void save(QString fileName);
    void open(QString fileName);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

    QString bitToString(QBitArray bits);
    QBitArray stringToBit(QString txt);

    void paint(QPainter &paint);
};

#endif // WEAVE_H
