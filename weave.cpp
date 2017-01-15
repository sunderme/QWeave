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

#include "weave.h"
#include <QPainter>
#include <QBitArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Weave::Weave(QWidget *parent) : QWidget(parent)
{
    init();
    generateWeave();
}

void Weave::init()
{
    QBitArray line(4);
    line.setBit(1,true);
    line.setBit(3,true);
    QBitArray line2(4);
    line2.setBit(0,true);
    line2.setBit(2,true);
    lines<<line<<line2<<line<<line2;
    nrCols=4;
    nrLines=lines.count();
    scale=8;

    QBitArray line3(4);
    line3.setBit(0,true);
    positions<<line3<<line3<<line3<<line3;
    nrPositions=4;

    QBitArray line4(4);
    translation<<line3<<line4<<line4<<line4;

    nrShafts=4;
    line3.fill(false,0,3);
    shafts<<line<<line2<<line3<<line3;
    xDist=1;
    yDist=1;
    offset=(nrLines+yDist+nrShafts)*scale;

    exclusiveShaft=true;
}

void Weave::generateWeave()
{
    for(int y=0;y<nrLines;y++){
        QBitArray p;
        p=positions.at(y);
        QBitArray s(nrShafts);
        for(int j=0;j<nrPositions;j++){
            if(p.at(j)){
                for(int k=0;k<nrShafts;k++){
                    if(translation[k].at(j)){
                        s.setBit(k,true);
                    }
                }
            }
        }
        QBitArray line(nrCols);
        for(int k=0;k<nrShafts;k++){
            if(s.at(k)){
                line=line|shafts[k];
            }
        }
        lines[y]=line;
    }
}

void Weave::generatePattern()
{

}

void Weave::save(QString fileName)
{
    QFile saveFile(fileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }
    QJsonDocument jsDoc;
    QJsonObject jsObj;
    jsObj["nrLines"]=nrLines;
    jsObj["nrCols"]=nrCols;
    jsObj["nrShafts"]=nrShafts;
    jsObj["nrPositions"]=nrPositions;
    QJsonArray jsPositions;
    for(int k=0;k<nrLines;k++){
        jsPositions.append(bitToString(positions[k]));
    }
    jsObj["positions"]=jsPositions;
    QJsonArray jsShafts;
    for(int k=0;k<nrShafts;k++){
        jsShafts.append(bitToString(shafts[k]));
    }
    jsObj["shafts"]=jsShafts;
    QJsonArray jsTranslations;
    for(int k=0;k<nrShafts;k++){
        jsTranslations.append(bitToString(translation[k]));
    }
    jsObj["translation"]=jsTranslations;
    jsDoc.setObject(jsObj);
    saveFile.write(jsDoc.toJson());
}

void Weave::open(QString fileName)
{
    QFile loadFile(fileName);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    QJsonObject jsObj=loadDoc.object();
    int newLines=jsObj["nrLines"].toInt();
    int newCols=jsObj["nrCols"].toInt();
    int newShafts=jsObj["nrShafts"].toInt();
    int newPositions=jsObj["nrPositions"].toInt();

    resizeWeave(newLines,newCols,newShafts,newPositions);

    QJsonArray jsPositions=jsObj["positions"].toArray();
    QJsonArray jsShafts=jsObj["shafts"].toArray();
    QJsonArray jsTranslations=jsObj["translation"].toArray();

    translation.clear();
    for(int k=0;k<jsTranslations.size();k++){
        translation.append(stringToBit(jsTranslations.at(k).toString()));
    }

    positions.clear();
    for(int k=0;k<jsPositions.size();k++){
        positions.append(stringToBit(jsPositions.at(k).toString()));
    }

    shafts.clear();
    for(int k=0;k<jsShafts.size();k++){
        shafts.append(stringToBit(jsShafts.at(k).toString()));
    }

    generateWeave();
}

QSize Weave::sizeHint() const
{
    int extension=2;
    int w=scale*(nrCols+xDist+nrPositions)+extension;
    int h=scale*(nrLines+yDist+nrShafts+1)+extension;
    return QSize(w,h);
}

void Weave::resizeWeave(int newLines, int newCols, int newShafts, int newPos)
{
    /*int newCols=8;
    int newLines=8;
    int newShafts=6;
    int newPos=6;*/
    if(newLines>nrLines){
        QBitArray line(newCols);
        QBitArray p(newPos);
        for(int k=0;k<(newLines-nrLines);k++){
            lines<<line;
            positions<<p;
        }
    }
    if(newLines<nrLines){
        for(int k=0;k<(nrLines-newLines);k++){
            lines.removeLast();
            positions.removeLast();
        }
    }
    nrLines=newLines;

    if(newShafts>nrShafts){
        QBitArray line(newCols);
        QBitArray p(newPos);
        for(int k=0;k<(newShafts-nrShafts);k++){
            shafts<<line;
            translation<<p;
        }
    }
    if(newShafts<nrShafts){
        for(int k=0;k<(nrShafts-newShafts);k++){
            shafts.removeLast();
            translation.removeLast();
        }
    }
    nrShafts=newShafts;

    for(int k=0;k<nrLines;k++){
        lines[k].resize(newCols);
        positions[k].resize(newPos);
    }
    for(int k=0;k<nrShafts;k++){
        shafts[k].resize(newCols);
        translation[k].resize(newPos);
    }


    nrPositions=newPos;
    nrCols=newCols;
    offset=(nrLines+yDist+nrShafts)*scale;

    generateWeave();
    adjustSize();
    update();
}


void Weave::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton) {
        QPoint point = event->pos();
        int y=(offset-point.y()+scale);
        int x=point.x();
        if(x>=0 && y>=0){
            x/=scale;
            y/=scale;
            if( x<nrCols && y<nrLines){
                //lines[y].toggleBit(x);
            }else{
                x=x-nrCols-xDist;
                if( x>=0 && x<nrPositions && y<nrLines){
                    if(!positions[y].at(x)){
                        positions[y].fill(false);
                        positions[y].setBit(x,true);
                    }
                }else{
                    if(x<0){
                        x=x+nrCols+xDist;
                        y=y-nrLines-yDist;
                        if( y>=0 && x<nrCols && y<nrShafts){
                            if(exclusiveShaft){
                                if(!shafts[y].at(x)){
                                    for(int k=0;k<nrShafts;k++){
                                        shafts[k].setBit(x,false);
                                    }
                                    shafts[y].setBit(x,true);
                                }
                            }else{
                                shafts[y].toggleBit(x);
                            }
                        }
                    }else{
                        y=y-nrLines-yDist;
                        if( x>=0 && y>=0 && x<nrPositions && y<nrShafts){
                            translation[y].toggleBit(x);
                        }
                    }
                }
            }
        }
        generateWeave();
        update();
    }
}

QString Weave::bitToString(QBitArray bits)
{
    // lsb left
    QString result;
    for(int j=0;j<bits.size();j++){
        result.append(bits.at(j) ? "1" : "0");
    }
    return result;
}

QBitArray Weave::stringToBit(QString txt)
{
    // lsb left
    QBitArray bit(txt.size());
    for(int j=0;j<txt.size();j++){
        if(txt.at(j)==QChar('1'))
            bit.setBit(j,true);
    }
    return bit;
}

void Weave::paint(QPainter &paint)
{
    QBrush up(Qt::blue);
    QBrush down(Qt::green);
    for(int y=0;y<nrLines;y++){
        QBitArray line;
        line=lines.at(y);
        for (int x = 0; x < nrCols; ++x) {
            if(line.at(x)){
                paint.setBrush(up);
            }else{
                paint.setBrush(down);
            }
            paint.drawRect(x*scale,offset-y*scale,scale,scale);
        }
        line=positions.at(y);
        for (int x = 0; x < nrPositions; ++x) {
            if(line.at(x)){
                paint.setBrush(up);
            }else{
                paint.setBrush(down);
            }
            paint.drawRect((x+nrCols+xDist)*scale,offset-y*scale,scale,scale);
        }
    }
    for(int y=0;y<nrShafts;y++){
        QBitArray line;
        line=shafts.at(y);
        for (int x = 0; x < nrCols; ++x) {
            if(line.at(x)){
                paint.setBrush(up);
            }else{
                paint.setBrush(down);
            }
            paint.drawRect(x*scale,offset-(y+nrLines+yDist)*scale,scale,scale);
        }
        line=translation.at(y);
        for (int x = 0; x < nrPositions; ++x) {
            if(line.at(x)){
                paint.setBrush(up);
            }else{
                paint.setBrush(down);
            }
            paint.drawRect((x+nrCols+xDist)*scale,offset-(y+nrLines+yDist)*scale,scale,scale);
        }
    }

}

void Weave::paintEvent(QPaintEvent *)
{
    QPainter painter;
    painter.begin(this);
    paint(painter);
    painter.end();
}
