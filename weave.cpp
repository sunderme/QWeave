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
#include <QColorDialog>
#include <QDebug>
#include <QMessageBox>

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
    exclusiveLines=true;

    mousePressPoint.setX(-1);
    mousePressPoint.setY(-1);
    mouseMovePoint.setX(-1);
    mouseMovePoint.setY(-1);

    lineColors.resize(nrLines);
    lineColors.fill(clrDown);
    colColors.resize(nrCols);
    colColors.fill(clrUp);

    mode=op_none;

    yOff=2;
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
        QBitArray zw(nrShafts);
        for(int k=0;k<nrCols;k++){
            zw=shafts[k]&s;
            if(zw.count(true)){
                line.setBit(k,true);
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
    for(int k=0;k<nrCols;k++){
        jsShafts.append(bitToString(shafts[k]));
    }
    jsObj["shafts"]=jsShafts;
    QJsonArray jsTranslations;
    for(int k=0;k<nrShafts;k++){
        jsTranslations.append(bitToString(translation[k]));
    }
    jsObj["translation"]=jsTranslations;
    QJsonArray jsLineColors;
    for(int k=0;k<nrLines;k++){
        jsLineColors.append(lineColors.at(k).name());
    }
    jsObj["lineColors"]=jsLineColors;
    QJsonArray jsColColors;
    for(int k=0;k<nrCols;k++){
        jsColColors.append(colColors.at(k).name());
    }
    jsObj["colColors"]=jsColColors;
    jsDoc.setObject(jsObj);
    saveFile.write(jsDoc.toJson());
}

void Weave::open(QString fileName)
{
    m_undoStack.clear();
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
    QJsonArray jsColColors=jsObj["colColors"].toArray();
    QJsonArray jsLineColors=jsObj["lineColors"].toArray();

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

    lineColors.resize(newLines);
    lineColors.fill(clrDown);
    for(int k=0;k<jsLineColors.size();k++){
        lineColors[k]=QColor(jsLineColors.at(k).toString());
    }

    colColors.resize(newCols);
    colColors.fill(clrUp);
    for(int k=0;k<jsColColors.size();k++){
        colColors[k]=QColor(jsColColors.at(k).toString());
    }
    // correct data imported from old format
    if(shafts.size()!=nrCols){
        bitField zw;
        for(int k=0;k<nrCols;k++){
            QBitArray ba(nrShafts);
            for(int j=0;j<nrShafts;j++){
                if(shafts[j].at(k)){
                    ba.setBit(j,true);
                }
            }
            zw<<ba;
        }
        shafts=zw;
    }

    generateWeave();
}

void Weave::copy()
{
    mode=op_copy;
}

void Weave::move()
{
    mode=op_move;
}

void Weave::clear()
{
    if(pos==pos_position || pos1==pos_position){
        m_undoStack.beginMacro("clear");
        for(int i=qMin(origin_x0,origin_x1);i<=qMax(origin_x0,origin_x1);i++){
            QBitArray line(nrPositions);
            ChangeArray *cp=new ChangeArray(&positions,i,line);
            m_undoStack.push(cp);
            //positions[i].fill(false);
        }
        m_undoStack.endMacro();
    }
    if(pos==pos_shaft || pos1==pos_position){
        m_undoStack.beginMacro("clear");
        for(int i=qMin(origin_x0,origin_x1);i<=qMax(origin_x0,origin_x1);i++){
            QBitArray line(nrShafts);
            ChangeArray *cp=new ChangeArray(&shafts,i,line);
            m_undoStack.push(cp);
            //shafts[i].fill(false);
        }
        m_undoStack.endMacro();
    }
    if(pos==pos_colColors || pos1==pos_colColors){
        m_undoStack.beginMacro("clear");
        for(int i=qMin(origin_x0,origin_x1);i<=qMax(origin_x0,origin_x1);i++){
            ChangeColorArray *ccp=new ChangeColorArray(&colColors,i,clrUp);
            m_undoStack.push(ccp);
            //shafts[i].fill(false);
        }
        m_undoStack.endMacro();
    }
    if(pos==pos_lineColors || pos1==pos_lineColors){
        m_undoStack.beginMacro("clear");
        for(int i=qMin(origin_x0,origin_x1);i<=qMax(origin_x0,origin_x1);i++){
            ChangeColorArray *ccp=new ChangeColorArray(&lineColors,i,clrDown);
            m_undoStack.push(ccp);
            //shafts[i].fill(false);
        }
        m_undoStack.endMacro();
    }
    generateWeave();
    update();
}

void Weave::mirror_x()
{
    if(pos==pos_position || pos1==pos_position){
        m_undoStack.beginMacro("mirrorX");
        for(int i=qMin(origin_x0,origin_x1);i<=qMax(origin_x0,origin_x1);i++){
            QBitArray line=positions.at(i);
            QBitArray line2(line.size());
            line=positions[i];
            for(int k=0;k<nrPositions;k++){
                line2.setBit(k,line.at(nrPositions-k-1));
            }
            ChangeArray *cp=new ChangeArray(&positions,i,line2);
            m_undoStack.push(cp);
        }
        m_undoStack.endMacro();
    }
    if(pos==pos_shaft || pos1==pos_position){
        m_undoStack.beginMacro("mirrorX");
        bool doColors=(pos==pos_colColors || pos1==pos_colColors);
        int l=qMax(origin_x0,origin_x1)-qMin(origin_x0,origin_x1)+1;
        int start=qMin(origin_x0,origin_x1);
        bitField field=shafts.mid(start,l);
        QVector<QColor> field2=colColors.mid(start,l);
        for(int j=0;j<l;j++){
            ChangeArray *cp=new ChangeArray(&shafts,j+start,field.at(l-j-1));
            m_undoStack.push(cp);
            if(doColors){
                ChangeColorArray *ccp=new ChangeColorArray(&colColors,j+start,field2.at(l-j-1));
                m_undoStack.push(ccp);
            }
        }
        m_undoStack.endMacro();
    }
    generateWeave();
    update();
}

void Weave::mirror_y()
{
    if(pos==pos_position){
        m_undoStack.beginMacro("mirrorY");
        bool doColors=(pos==pos_lineColors || pos1==pos_lineColors);
        int l=qMax(origin_x0,origin_x1)-qMin(origin_x0,origin_x1)+1;
        int start=qMin(origin_x0,origin_x1);
        bitField field=positions.mid(start,l);
        QVector<QColor> field2=lineColors.mid(start,l);
        for(int j=0;j<l;j++){
            ChangeArray *cp=new ChangeArray(&positions,j+start,field.at(l-j-1));
            m_undoStack.push(cp);
            if(doColors){
                ChangeColorArray *ccp=new ChangeColorArray(&lineColors,j+start,field2.at(l-j-1));
                m_undoStack.push(ccp);
            }
        }
        m_undoStack.endMacro();
    }
    if(pos==pos_shaft){
        m_undoStack.beginMacro("mirrorY");
        for(int i=qMin(origin_x0,origin_x1);i<=qMax(origin_x0,origin_x1);i++){
            QBitArray line=shafts.at(i);
            QBitArray line2(line.size());
            for(int k=0;k<nrShafts;k++){
                line2.setBit(k,line.at(nrShafts-k-1));
            }
            ChangeArray *cp=new ChangeArray(&shafts,i,line2);
            m_undoStack.push(cp);
        }
        m_undoStack.endMacro();
    }
    generateWeave();
    update();
}

void Weave::analyzePattern()
{
    // analyze pattern
    QVector<int> sum(nrShafts);
    for(int i=0;i<shafts.length();i++){
        QBitArray ba=shafts.at(i);
        for(int k=0;k<nrShafts;k++){
            if(ba.at(k)){
                sum[i]++;
            }
        }
    }
    // output result
    QString result;
    for(int k=0;k<nrShafts;k++){
        result+=tr("haddle %1 %2 \n").arg(k+1).arg(sum[k]);
    }
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("analyze haddles"));
    msgBox.setText(result);
    msgBox.exec();
}

void Weave::modifySelected()
{

}

void Weave::undo()
{
    m_undoStack.undo();
    generateWeave();
    update();
}

void Weave::redo()
{
    m_undoStack.redo();
    generateWeave();
    update();
}

QSize Weave::sizeHint() const
{
    int extension=2;
    int w=scale*(nrCols+xDist+nrPositions+2)+extension;
    int h=scale*(nrLines+yDist+nrShafts+1+yOff)+extension;
    return QSize(w,h);
}

void Weave::resizeWeave(int newLines, int newCols, int newShafts, int newPos)
{
    /*int newCols=8;
    int newLines=8;
    int newShafts=6;
    int newPos=6;*/
    lineColors.resize(newLines);
    if(newLines>nrLines){
        QBitArray line(newCols);
        QBitArray p(newPos);
        for(int k=0;k<(newLines-nrLines);k++){
            lines<<line;
            positions<<p;
            lineColors[k+nrLines]=clrDown;
        }
    }
    if(newLines<nrLines){
        for(int k=0;k<(nrLines-newLines);k++){
            lines.removeLast();
            positions.removeLast();
        }
    }
    nrLines=newLines;
    nrShafts=newShafts;
    colColors.resize(newCols);
    if(newCols>nrCols){
        QBitArray line(nrShafts);
        for(int k=0;k<(newCols-nrCols);k++){
            shafts<<line;
            colColors[k+nrCols]=clrUp;
        }
    }
    if(newCols<nrCols){
        for(int k=0;k<(nrCols-newCols);k++){
            shafts.removeLast();
        }
    }


    for(int k=0;k<nrLines;k++){
        lines[k].resize(newCols);
        positions[k].resize(newPos);
    }
    for(int k=0;k<nrShafts;k++){
        if(k>=translation.count()){
            QBitArray line(newPos);
            translation<<line;
        }else{
            translation[k].resize(newPos);
        }
    }
    for(int k=0;k<nrCols;k++){
        shafts[k].resize(newShafts);
    }


    nrPositions=newPos;
    nrCols=newCols;
    offset=(nrLines+yDist+nrShafts)*scale;

    generateWeave();
    adjustSize();
    update();
}


void Weave::mousePressEvent(QMouseEvent *event){
    mouseMovePoint.setX(-1);
    mouseMovePoint.setY(-1);
    if (event->button() == Qt::LeftButton) {
        if(mode==op_copy || mode==op_move){
            panePos pos0;
            int x,y;
            determinePos(event->pos(),pos0,x,y);
            if(pos==pos0){
                performCopy(x,y,mode==op_move,false,(pos==pos_position)||(pos==pos_lineColors));
            }else{
                if((pos==pos_position||pos==pos_shaft)&&(pos0==pos_position||pos0==pos_shaft)&&(nrPositions==nrShafts)){
                    performCopy(x,y,mode==op_move,true);
                }
            }

            mode=op_none;
            return;
        }
        mousePressPoint=event->pos();
    }
}

void Weave::clicked(){
    int y=mousePressPoint.y();
    int x=mousePressPoint.x();
    if(x>=0 && y>=0){
        x/=scale;
        y/=scale;
        y-=yOff;
        if(y==-yOff && x<nrCols){
            // change color
            QColorDialog dlg;
            dlg.setCurrentColor(colColors.at(x));
            if(dlg.exec()){
                ChangeColorArray *ccp=new ChangeColorArray(&colColors,x,dlg.currentColor());
                m_undoStack.push(ccp);
                //colColors[x]=dlg.currentColor();
            }
        }
        if( x==nrCols+nrPositions+2 && y>nrShafts){
            // change color
            int yn=y-nrShafts-yDist;
            QColorDialog dlg;
            dlg.setCurrentColor(lineColors.at(yn));
            if(dlg.exec()){
                ChangeColorArray *ccp=new ChangeColorArray(&lineColors,yn,dlg.currentColor());
                m_undoStack.push(ccp);
                //lineColors[yn]=dlg.currentColor();
            }
        }
        if( x<nrCols && y>nrShafts){
            //lines[y].toggleBit(x);
        }else{
            x=x-nrCols-xDist;
            int yn=y-nrShafts-yDist;
            if( x>=0 && yn>=0 && x<nrPositions && yn<nrLines){
                if(exclusiveLines){
                    if(!positions[yn].at(x)){
                        //positions[yn].fill(false);
                        //positions[yn].setBit(x,true);
                        QBitArray newLine(nrPositions);
                        newLine.setBit(x,true);
                        ChangeArray *cp=new ChangeArray(&positions,yn,newLine);
                        m_undoStack.push(cp);
                    }
                }else{
                    QBitArray newLine(nrPositions);
                    newLine=positions[yn];
                    newLine.toggleBit(x);
                    ChangeArray *cp=new ChangeArray(&positions,yn,newLine);
                    m_undoStack.push(cp);
                }
            }else{
                if(x<0){
                    x=x+nrCols+xDist;
                    if( y>=0 && x<nrCols && y<nrShafts){
                        if(exclusiveShaft){
                            if(!shafts[x].at(nrShafts-y-1)){
                                //shafts[x].fill(false);
                                //shafts[x].setBit(nrShafts-y-1,true);
                                QBitArray newLine(nrShafts);
                                newLine.setBit(nrShafts-y-1,true);
                                ChangeArray *cp=new ChangeArray(&shafts,x,newLine);
                                m_undoStack.push(cp);
                            }
                        }else{
                            //shafts[x].toggleBit(nrShafts-y-1);
                            QBitArray newLine(nrShafts);
                            newLine=shafts[x];
                            newLine.toggleBit(nrShafts-y-1);
                            ChangeArray *cp=new ChangeArray(&shafts,x,newLine);
                            m_undoStack.push(cp);
                        }
                    }
                }else{
                    if( x>=0 && y>=0 && x<nrPositions && y<nrShafts){
                        //translation[nrShafts-y-1].toggleBit(x);
                        QBitArray newLine(nrPositions);
                        newLine=translation[nrShafts-y-1];
                        newLine.toggleBit(x);
                        ChangeArray *cp=new ChangeArray(&translation,nrShafts-y-1,newLine);
                        m_undoStack.push(cp);
                    }
                }
            }
        }
    }
    generateWeave();
    update();
}

void Weave::determinePos(QPoint p, Weave::panePos &pos, int &x, int &y)
{
    y=p.y();
    x=p.x();
    pos=pos_none;
    if(x>=0 && y>=0){
        x/=scale;
        y/=scale;
        if( x<nrCols && y==0){
            pos=pos_colColors;
            return;
        }
        y-=yOff;
        if( x<nrCols && y>nrShafts){
            y=y-nrShafts-yDist;
            if(y<nrLines)
                pos=pos_lines;
            return;
            //lines[y].toggleBit(x);
        }else{
            x=x-nrCols-xDist;
            int yn=y-nrShafts-yDist;
            if( x==nrPositions+1 && yn>=0 && yn<nrLines){
                pos=pos_lineColors;
                y=yn;
                return;
            }
            if( x>=0 && yn>=0 && x<nrPositions && yn<nrLines){
                y=yn;
                pos=pos_position;
                return;
            }else{
                if(x<0){
                    x=x+nrCols+xDist;
                    if( y>=0 && x<nrCols && y<nrShafts){
                        pos=pos_shaft;
                        y=nrShafts-y-1;
                        return;
                    }
                }else{
                    if( x>=0 && y>=0 && x<nrPositions && y<nrShafts){
                        pos=pos_translate;
                        return;
                    }
                }
            }
        }
    }
}

void Weave::performCopy(int x, int y,bool clearSel,bool crossCopy,bool newOrignLeft)
{
    bitField *source=&shafts;
    bitField *target=&shafts;
    QVector<QColor> *source2=0;
    QVector<QColor> *target2=0;
    m_undoStack.beginMacro("copy/move");
    if(pos==pos_colColors || pos1==pos_colColors){
        source2=&colColors;
        target2=source2;
    }
    if(crossCopy && (pos==pos_shaft||pos1==pos_shaft)){
        target=&positions;
        if(source2){
            target2=&lineColors;
        }
        x=y;
    }
    if(pos==pos_position || pos1==pos_position){
        source=&positions;
        target=source;
        if(pos==pos_lineColors || pos1==pos_lineColors){
            source2=&lineColors;
            target2=source2;
        }
        if(crossCopy){
            target=&shafts;
            if(source2){
                target2=&colColors;
            }
        }else{
            x=y;
        }
    }
    int l=qMax(origin_x0,origin_x1)-qMin(origin_x0,origin_x1)+1;
    int start=qMin(origin_x0,origin_x1);
    bitField field=source->mid(start,l);
    QVector<QColor> field2;
    if(source2){
        field2=source2->mid(start,l);
    }
    if(clearSel)
        clear();
    if(!newOrignLeft)
        x=x-l+1;
    for(int i=0;i<field.size();i++){
        if(x+i<0 || x+i>=target->size())
            continue;
        ChangeArray *cp=new ChangeArray(target,x+i,field.at(i));
        m_undoStack.push(cp);
        if(target2){
            ChangeColorArray *ccp=new ChangeColorArray(target2,x+i,field2.at(i));
            m_undoStack.push(ccp);
        }
        //(*target)[x+i]=field.at(i);
    }
    m_undoStack.endMacro();
    generateWeave();
    update();
}

void Weave::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint delta=event->pos()-mousePressPoint;
    if(delta.manhattanLength()<=1)
        clicked();
}

void Weave::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePressPoint.x()>=0){
        mouseMovePoint=event->pos();
        QPoint delta=mousePressPoint-mouseMovePoint;
        if(delta.manhattanLength()>1){
            update();
        }
    }
}

void Weave::wheelEvent(QWheelEvent *event)
{
    //QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;
    if(event->modifiers()==Qt::ControlModifier){
        scale+=numDegrees.y()/15;
        if(scale<2)
            scale=2;
        event->accept();
        adjustSize();
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

void Weave::paint(QPainter &paint,int useScale)
{
    if(useScale<1)
        useScale=scale;
    QBrush up(clrUp);
    QBrush down(clrDown);
    // check if mouseDrag
    bool inSelectMode=false;
    pos=pos_none;
    pos1=pos_none;
    if(mouseMovePoint.x()>=0){
        int x0,x1;
        determinePos(mousePressPoint,pos,x0,origin_x0);
        determinePos(mouseMovePoint,pos1,x1,origin_x1);
        if(pos!=pos_none && pos==pos1 && origin_x0>=0 && origin_x1>=0){
            inSelectMode=true;
            if(pos==pos_shaft || pos==pos_colColors){
                origin_x0=x0;
                origin_x1=x1;
            }
        }else{
            if((pos==pos_shaft || pos1==pos_shaft)&&(pos==pos_colColors || pos1==pos_colColors)){
                inSelectMode=true;
                origin_x0=x0;
                origin_x1=x1;
            }
            if((pos==pos_position || pos1==pos_position)&&(pos==pos_lineColors || pos1==pos_lineColors)){
                inSelectMode=true;
            }
            if(!inSelectMode){
                pos=pos_none;
                pos1=pos_none;
            }
        }
    }
    //
    for(int y=0;y<nrLines;y++){
        QBitArray line;
        line=lines.at(y);
        paint.save();
        for (int x = 0; x < nrCols; ++x) {
            if(line.at(x)){
                paint.setBrush(colColors.at(x));
                paint.setPen(Qt::NoPen);
                paint.drawRect(x*useScale,(nrShafts+yDist+y+yOff)*useScale,useScale,useScale);
                paint.setPen(Qt::SolidLine);
                paint.drawLine(x*useScale,(nrShafts+yDist+y+yOff)*useScale,x*useScale,(nrShafts+yDist+y+1+yOff)*useScale);
                paint.drawLine(x*useScale+useScale,(nrShafts+yDist+y+yOff)*useScale,x*useScale+useScale,(nrShafts+yDist+y+1+yOff)*useScale);
            }else{
                paint.setBrush(lineColors.at(y));
                paint.setPen(Qt::NoPen);
                paint.drawRect(x*useScale,(nrShafts+yDist+y+yOff)*useScale,useScale,useScale);
                paint.setPen(Qt::SolidLine);
                paint.drawLine(x*useScale,(nrShafts+yDist+y+yOff)*useScale,x*useScale+useScale,(nrShafts+yDist+y+yOff)*useScale);
                paint.drawLine(x*useScale,(nrShafts+yDist+y+1+yOff)*useScale,x*useScale+useScale,(nrShafts+yDist+y+1+yOff)*useScale);
            }
        }
        paint.restore();
        line=positions.at(y);
        for (int x = 0; x < nrPositions; ++x) {
            if(inSelectMode && (pos==pos_position || pos1==pos_position)){
                if(y>=qMin(origin_x0,origin_x1) && y<=qMax(origin_x0,origin_x1)){
                    paint.setPen(Qt::yellow);
                }else{
                    paint.setPen(Qt::black);
                }
            }
            if(line.at(x)){
                paint.setBrush(up);
            }else{
                paint.setBrush(down);
            }
            paint.drawRect((x+nrCols+xDist)*useScale,(nrShafts+yDist+y+yOff)*useScale,useScale,useScale);
        }
        if(inSelectMode && (pos==pos_position || pos1==pos_position)){
            paint.setPen(Qt::black);
        }
        // paint colour selector
        if(inSelectMode && (pos==pos_lineColors || pos1==pos_lineColors)){
            if(y>=qMin(origin_x0,origin_x1) && y<=qMax(origin_x0,origin_x1)){
                paint.setPen(Qt::yellow);
            }else{
                paint.setPen(Qt::black);
            }
        }
        paint.setBrush(lineColors.at(y));
        paint.drawRect((nrCols+xDist+nrPositions+1)*useScale,(nrShafts+yDist+y+yOff)*useScale,useScale,useScale);
        if(inSelectMode && (pos==pos_lineColors || pos1==pos_lineColors)){
            paint.setPen(Qt::black);
        }
    }
    // colColours
    for (int x = 0; x < nrCols; ++x) {
        if(inSelectMode && (pos==pos_colColors || pos1==pos_colColors)){
            if(x>=qMin(origin_x0,origin_x1) && x<=qMax(origin_x0,origin_x1)){
                paint.setPen(Qt::yellow);
            }else{
                paint.setPen(Qt::black);
            }
        }
        paint.setBrush(colColors.at(x));
        paint.drawRect(x*useScale,(0)*useScale,useScale,useScale);
        if(inSelectMode && (pos==pos_colColors || pos1==pos_colColors)){
            paint.setPen(Qt::black);
        }
    }

    for(int y=0;y<nrShafts;y++){
        QBitArray line;
        for (int x = 0; x < nrCols; ++x) {
            line=shafts.at(x);
            if(inSelectMode && (pos==pos_shaft || pos1==pos_shaft)){
                if(x>=qMin(origin_x0,origin_x1) && x<=qMax(origin_x0,origin_x1)){
                    paint.setPen(Qt::yellow);
                }else{
                    paint.setPen(Qt::black);
                }
            }
            if(line.at(y)){
                paint.setBrush(up);
            }else{
                paint.setBrush(down);
            }
            paint.drawRect(x*useScale,(nrShafts-y-1+yOff)*useScale,useScale,useScale);
        }
        if(inSelectMode && (pos==pos_shaft || pos1==pos_shaft)){
            paint.setPen(Qt::black);
        }
        line=translation.at(y);
        for (int x = 0; x < nrPositions; ++x) {
            if(line.at(x)){
                paint.setBrush(up);
            }else{
                paint.setBrush(down);
            }
            paint.drawRect((x+nrCols+xDist)*useScale,(nrShafts-y-1+yOff)*useScale,useScale,useScale);
        }
    }

}

void Weave::generateColourPattern(QList<QColor> colors, QList<int> pattern, int side)
{
    m_undoStack.beginMacro("Pattern");
    int period=pattern.length();
    QVector<QColor> *target=&lineColors;
    if(side==1){
        target=&colColors;
    }
    // pattern colours
    for(int k=0;k<nrLines;k++){
        int j=k%period;
        j=pattern.at(j);
        if(j>=colors.length())
            j=0;
        if(j<0)
            j=0;
        //lineColors[k]=colors.at(j);
        ChangeColorArray *ccp=new ChangeColorArray(target,k,colors.at(j));
        m_undoStack.push(ccp);
    }

    m_undoStack.endMacro();
    generateWeave();
    update();
}

void Weave::paintEvent(QPaintEvent *)
{
    QPainter painter;
    painter.begin(this);
    paint(painter);
    painter.end();
}
