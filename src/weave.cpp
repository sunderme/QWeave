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
#include <QToolButton>
#include <QDate>
#include <QPixmapCache>
#include <QAction>

//#include <valgrind/callgrind.h>

Weave::Weave(QWidget *parent) : QWidget(parent)
{
    init();
    generateWeave();
    setMouseTracking(true);
}

void Weave::init()
{
    QBitArray line(4);
    /*line.setBit(1,true);
    line.setBit(3,true);
    QBitArray line2(4);
    line2.setBit(0,true);
    line2.setBit(2,true);*/
    lines<<line<<line<<line<<line;
    nrCols=4;
    nrLines=lines.count();
    scale=8;

    //QBitArray line3(4);
    //line3.setBit(0,true);
    positions<<line<<line<<line<<line;
    nrPositions=4;

    //QBitArray line4(4);
    translation<<line<<line<<line<<line;

    nrShafts=4;
    //line3.fill(false,0,3);
    shafts<<line<<line<<line<<line;
    xDist=1;
    yDist=1;
    offset=(nrLines+yDist+nrShafts)*scale;

    exclusiveShaft=true;
    exclusiveLines=true;

    mousePressPoint.setX(-1);
    mousePressPoint.setY(-1);
    mouseMovePoint.setX(-1);
    mouseMovePoint.setY(-1);

    clrDown=Qt::white;
    clrUp=Qt::black;

    lineColors.resize(nrLines);
    lineColors.fill(clrDown);
    colColors.resize(nrCols);
    colColors.fill(clrUp);

    mode=op_none;

    yOff=2;

    m_showInverted=false;
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
            if(m_showInverted){
                if(!zw.count(true)){
                    line.setBit(k,true);
                }
            }else{
                if(zw.count(true)){
                    line.setBit(k,true);
                }
            }
        }
        lines[y]=line;
    }
}

void Weave::generatePattern()
{

}

void Weave::save(const QString &fileName){
    if(fileName.endsWith(".weave")){
        writeJson(fileName);
    }else{
        writeWIF(fileName);
    }
}

void Weave::writeJson(const QString &fileName)
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

void Weave::writeWIF(const QString &fileName)
{
    QFile saveFile(fileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }
    QTextStream out(&saveFile);
    out<<"[WIF]\n"<<"Version=1.2\n"<<"Date="+QDate::currentDate().toString("MM/dd/yyyy")+"\n";
    out<<"[CONTENTS]\n"<<"COLOR PALETTE=yes\n"<<"WEAVING=yes\n"<<"WARP=yes\n"<<"WEFT=yes\n"<<"TIEUP=yes\n"<<"COLOR TABLE=yes\n";
    out<<"THREADING=yes\n"<<"WARP COLORS=yes\n"<<"TREADLING=yes\n"<<"WEFT COLORS=yes\n"<<"[WEAVING]\n";
    out<<QString("Shafts=%1\n").arg(nrShafts);
    out<<QString("Treadles=%1\n").arg(nrPositions);
    out<<"Rising Shed=yes\n"<<"Profile=no\n";
    // color palette
    QList<QColor> lstColors;
    for(const auto& clr : colColors){
        if(!lstColors.contains(clr)){
            lstColors<<clr;
        }
    }
    for(const auto& clr : lineColors){
        if(!lstColors.contains(clr)){
            lstColors<<clr;
        }
    }
    out<<"[COLOR PALETTE]\n"<<QString("Entries=%1\n").arg(lstColors.length())<<"Form=RGB\n"<<"Range=0,255\n";
    out<<"[COLOR TABLE]\n";
    for(int i=0;i<lstColors.length();i++){
        out<<QString("%1=").arg(i+1);
        const QColor& clr=lstColors.at(i);
        out<<QString("%1,%2,%3\n").arg(clr.red()).arg(clr.green()).arg(clr.blue());
    }
    out<<"[WARP]\n"<<QString("Threads=%1\n").arg(nrCols);
    out<<"Units=Centimeters\n"<<"Spacing=0.0185\n"<<"Thickness=0.0213\n"; //generic values
    out<<"[WEFT]\n"<<QString("Threads=%1\n").arg(nrLines);
    out<<"Units=Centimeters\n"<<"Spacing=0.0185\n"<<"Thickness=0.0213\n"; //generic values
    out<<"[TIEUP]\n";
    bitField transposed=transpose(translation);
    for(int i=0;i<transposed.length();i++){
        out<<QString("%1=").arg(i+1);
        QBitArray ba=transposed.at(i);
        bool first=true;
        for(int j=0;j<ba.size();j++){
            if(ba[j]){
                if(!first){
                    out<<",";
                }
                out<<QString("%1").arg(j+1);
                first=false;
            }
        }
        out<<"\n";
    }
    out<<"[THREADING]\n";
    for(int i=0;i<shafts.length();i++){
        out<<QString("%1=").arg(i+1);
        QBitArray ba=shafts.at(i);
        bool first=true;
        for(int j=0;j<ba.size();j++){
            if(ba[j]){
                if(!first){
                    out<<",";
                }
                out<<QString("%1").arg(j+1);
                first=false;
            }
        }
        out<<"\n";
    }
    out<<"[TREADLING]\n";
    for(int i=0;i<positions.length();i++){
        out<<QString("%1=").arg(i+1);
        QBitArray ba=positions.at(i);
        bool first=true;
        for(int j=0;j<ba.size();j++){
            if(ba[j]){
                if(!first){
                    out<<",";
                }
                out<<QString("%1").arg(j+1);
                first=false;
            }
        }
        out<<"\n";
    }
    out<<"[WARP COLORS]\n";
    for(int i=0;i<colColors.length();i++){
        QColor clr=colColors.at(i);
        int paletteNumber=lstColors.indexOf(clr);
        if(paletteNumber>=0){
            out<<QString("%1=%2\n").arg(i+1).arg(paletteNumber+1);
        }
    }
    out<<"[WEFT COLORS]\n";
    for(int i=0;i<lineColors.length();i++){
        QColor clr=lineColors.at(i);
        int paletteNumber=lstColors.indexOf(clr);
        if(paletteNumber>=0){
            out<<QString("%1=%2\n").arg(i+1).arg(paletteNumber+1);
        }
    }
}

void Weave::open(const QString &fileName)
{
    m_undoStack.clear();

    if(fileName.endsWith(".weave")){
        readJson(fileName);
    }else{
        readWIF(fileName);
    }
}

void Weave::readJson(const QString &fileName){
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

void Weave::readWIF(const QString &fileName){
    QFile loadFile(fileName);

    if (!loadFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Couldn't open save file.");
        return;
    }
    QTextStream in(&loadFile);
    bool isWIF=false;
    //bool inText=false;

    int newNrLines=0;
    int newNrCols=0;
    int newNrShafts=0;
    int newNrPositions=0;

    int maxColorNumber=255;

    bitField newTranslation,newShafts,newPositions;
    QVector<QColor> newColColors,newLineColors;

    QVector<QColor> lstColors;

    QHash<QString,QStringList> doc;
    QString key;

    while (!in.atEnd()) {
        QString line = in.readLine();
        // process file
        if(line=="[WIF]")
            isWIF=true;
        if(line.startsWith("[")){
            key=line;
            doc.insert(key,QStringList());
        }else{
            doc[key]<<line;
        }
        if(!isWIF)
            return; // abort if not first line is [WIF]
    }
    QStringList keyWords;
    keyWords<<"[COLOR PALETTE]"<<"[WEAVING]"<<"[COLOR TABLE]"<<"[WEFT]"<<"[WARP]"<<"[TIEUP]"<<"[THREADING]"<<"[TREADLING]"<<"[WARP COLORS]"<<"[WEFT COLORS]";

    foreach(QString key,keyWords){
        bool inWarp=false;
        bool inWeft=false;
        bool inTieUp=false;
        bool inThreading=false;
        bool inTreadling=false;
        bool inWarpColors=false;
        bool inWeftColors=false;
        bool inWeaving=false;
        bool inColorTable=false;
        bool inColorPalette=false;
        int nr=-1;
        if(key=="[WEAVING]"){
            inWeaving=true;
        }
        if(key=="[COLOR PALETTE]"){
            inColorPalette=true;
        }
        if(key=="[COLOR TABLE]"){
            inColorTable=true;
        }
        if(key=="[WEFT]"){
            inWeft=true;
        }
        if(key=="[WARP]"){
            inWarp=true;
        }
        if(key=="[TIEUP]"){
            inTieUp=true;
        }
        if(key=="[THREADING]"){
            inThreading=true;
        }
        if(key=="[TREADLING]"){
            inTreadling=true;
        }
        if(key=="[WARP COLORS]"){
            inWarpColors=true;
        }
        if(key=="[WEFT COLORS]"){
            inWeftColors=true;
        }
        foreach(QString line,doc[key]){
            if(inWeaving){
                if(line.startsWith("Shafts=")){
                    newNrShafts=line.midRef(7).toInt();
                }
                if(line.startsWith("Treadles=")){
                    newNrPositions=line.midRef(9).toInt();
                }
            }
            if(inColorTable){
                int p=line.indexOf("=");
                if(p>0){
                    QString clr=line.mid(p+1);
                    QStringList clrs=clr.split(",");
                    if(clrs.length()==3){
                        bool ok;
                        bool error=false;
                        QList<int> numbers;
                        foreach(QString elem,clrs){
                            int zw=elem.toInt(&ok);
                            if(maxColorNumber!=255){
                                zw=qRound(zw*255.0/maxColorNumber);
                            }
                            numbers<<zw;
                            error|=!ok;
                        }
                        if(!error){
                            lstColors<<QColor(numbers.at(0),numbers.at(1),numbers.at(2));
                        }
                    }
                }
            }
            if(inColorPalette){
                if(line.startsWith("Range=")){
                    QString rest=line.mid(6);
                    QStringList strNumbers=rest.split(",");
                    QList<int> numbers;
                    bool ok;
                    bool error=false;
                    foreach(QString elem,strNumbers){
                        numbers<<elem.toInt(&ok);
                        error|=!ok;
                    }
                    if(numbers.size()==2){
                        maxColorNumber=numbers.last();
                    }
                }
            }
            if(inWarp){
                if(line.startsWith("Threads=")){
                    newNrCols=line.midRef(8).toInt();
                }
                if(line.startsWith("Color=")){
                    QString rest=line.mid(6);
                    QStringList strNumbers=rest.split(",");
                    QList<int> numbers;
                    bool ok;
                    bool error=false;
                    foreach(QString elem,strNumbers){
                        numbers<<elem.toInt(&ok);
                        error|=!ok;
                    }
                    if(!error){
                        nr=numbers.first();

                    }
                }
                if(nr>0 && nr<=lstColors.length()&&newNrCols>0){
                    newColColors.fill(lstColors.at(nr-1),newNrCols);
                    clrUp=lstColors.at(nr-1);
                    nr=-1;
                }
            }
            if(inWeft){
                if(line.startsWith("Threads=")){
                    newNrLines=line.midRef(8).toInt();
                }
                if(line.startsWith("Color=")){
                    QString rest=line.mid(6);
                    QStringList strNumbers=rest.split(",");
                    QList<int> numbers;
                    bool ok;
                    bool error=false;
                    foreach(QString elem,strNumbers){
                        numbers<<elem.toInt(&ok);
                        error|=!ok;
                    }
                    if(!error){
                        nr=numbers.first();
                    }
                }
                if(nr>0 && nr<=lstColors.length()&&newNrLines>0){
                    newLineColors.fill(lstColors.at(nr-1),newNrLines);
                    clrDown=lstColors.at(nr-1);
                    nr=-1;
                }
            }
            if(inTieUp){
                int p=line.indexOf("=");
                if(p>0){
                    QString rest=line.mid(p+1);
                    QStringList strNumbers=rest.split(",");
                    QList<int> numbers;
                    bool ok;
                    bool error=false;
                    foreach(QString elem,strNumbers){
                        numbers<<elem.toInt(&ok);
                        error|=!ok;
                    }
                    if(!error){
                        QBitArray ba(newNrShafts);
                        foreach (int elem, numbers) {
                            if(elem>0 && elem<=newNrShafts)
                                ba.setBit(elem-1,true);
                        }
                        newTranslation<<ba;
                    }
                }
            }
            if(inThreading){
                int p=line.indexOf("=");
                if(p>0){
                    QString rest=line.mid(p+1);
                    QStringList strNumbers=rest.split(",");
                    QList<int> numbers;
                    bool ok;
                    bool error=false;
                    foreach(QString elem,strNumbers){
                        numbers<<elem.toInt(&ok);
                        error|=!ok;
                    }
                    if(!error){
                        QBitArray ba(newNrShafts);
                        foreach (int elem, numbers) {
                            if(elem>0 && elem<=newNrShafts)
                                ba.setBit(elem-1,true);
                        }
                        newShafts<<ba;
                    }
                    newNrCols=newShafts.size();
                }
            }
            if(inTreadling){
                int p=line.indexOf("=");
                if(p>0){
                    QString rest=line.mid(p+1);
                    QStringList strNumbers=rest.split(",");
                    QList<int> numbers;
                    bool ok;
                    bool error=false;
                    foreach(QString elem,strNumbers){
                        numbers<<elem.toInt(&ok);
                        error|=!ok;
                    }
                    if(!error){
                        QBitArray ba(newNrPositions);
                        foreach (int elem, numbers) {
                            if(elem>0 && elem<=newNrPositions)
                                ba.setBit(elem-1,true);
                        }
                        newPositions<<ba;
                    }
                    newNrLines=newPositions.size();
                }
            }
            if(inWarpColors){
                int p=line.indexOf("=");
                if(p>0){
                    QString rest=line.mid(p+1);
                    QStringList strNumbers=rest.split(",");
                    QList<int> numbers;
                    bool ok;
                    bool error=false;
                    foreach(QString elem,strNumbers){
                        numbers<<elem.toInt(&ok);
                        error|=!ok;
                    }
                    if(!error){
                        int nr=numbers.first();
                        if(nr>0 && nr<=lstColors.length()){
                            newColColors<<lstColors.at(nr-1);
                        }
                    }
                }
            }
            if(inWeftColors){
                int p=line.indexOf("=");
                if(p>0){
                    QString rest=line.mid(p+1);
                    QStringList strNumbers=rest.split(",");
                    QList<int> numbers;
                    bool ok;
                    bool error=false;
                    foreach(QString elem,strNumbers){
                        numbers<<elem.toInt(&ok);
                        error|=!ok;
                    }
                    if(!error){
                        int nr=numbers.first();
                        if(nr>0 && nr<=lstColors.length()){
                            newLineColors<<lstColors.at(nr-1);
                        }
                    }
                }
            }
        }

    }

    resizeWeave(newNrLines,newNrCols,newNrShafts,newNrPositions);

    translation.clear();
    translation=transpose(newTranslation);

    positions.clear();
    positions=newPositions;

    shafts.clear();
    shafts=newShafts;

    lineColors.resize(newNrLines);
    lineColors.fill(clrDown);
    if(!newLineColors.isEmpty()){
        lineColors=newLineColors;
    }

    colColors.resize(newNrCols);
    colColors.fill(clrUp);
    if(!newColColors.isEmpty()){
        colColors=newColColors;
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
            auto *cp=new ChangeArray(&positions,i,line);
            m_undoStack.push(cp);
            //positions[i].fill(false);
        }
        m_undoStack.endMacro();
    }
    if(pos==pos_shaft || pos1==pos_shaft){
        m_undoStack.beginMacro("clear");
        for(int i=qMin(origin_x0,origin_x1);i<=qMax(origin_x0,origin_x1);i++){
            QBitArray line(nrShafts);
            auto *cp=new ChangeArray(&shafts,i,line);
            m_undoStack.push(cp);
            //shafts[i].fill(false);
        }
        m_undoStack.endMacro();
    }
    if(pos==pos_colColors || pos1==pos_colColors){
        m_undoStack.beginMacro("clear");
        for(int i=qMin(origin_x0,origin_x1);i<=qMax(origin_x0,origin_x1);i++){
            auto *ccp=new ChangeColorArray(&colColors,i,clrUp);
            m_undoStack.push(ccp);
            //shafts[i].fill(false);
        }
        m_undoStack.endMacro();
    }
    if(pos==pos_lineColors || pos1==pos_lineColors){
        m_undoStack.beginMacro("clear");
        for(int i=qMin(origin_x0,origin_x1);i<=qMax(origin_x0,origin_x1);i++){
            auto *ccp=new ChangeColorArray(&lineColors,i,clrDown);
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
            auto *cp=new ChangeArray(&positions,i,line2);
            m_undoStack.push(cp);
        }
        m_undoStack.endMacro();
    }
    if(pos==pos_shaft || pos1==pos_shaft){
        m_undoStack.beginMacro("mirrorX");
        bool doColors=(pos==pos_colColors || pos1==pos_colColors);
        int l=qMax(origin_x0,origin_x1)-qMin(origin_x0,origin_x1)+1;
        int start=qMin(origin_x0,origin_x1);
        bitField field=shafts.mid(start,l);
        QVector<QColor> field2=colColors.mid(start,l);
        for(int j=0;j<l;j++){
            auto *cp=new ChangeArray(&shafts,j+start,field.at(l-j-1));
            m_undoStack.push(cp);
            if(doColors){
                auto *ccp=new ChangeColorArray(&colColors,j+start,field2.at(l-j-1));
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
            auto *cp=new ChangeArray(&positions,j+start,field.at(l-j-1));
            m_undoStack.push(cp);
            if(doColors){
                auto *ccp=new ChangeColorArray(&lineColors,j+start,field2.at(l-j-1));
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
            auto *cp=new ChangeArray(&shafts,i,line2);
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
    for(int i=0;i<nrCols;i++){
        QBitArray ba=shafts.at(i);
        for(int k=0;k<nrShafts;k++){
            if(ba.at(k)){
                sum[k]++;
            }
        }
    }
    // output result
    QString result;
    for(int k=0;k<nrShafts;k++){
        result+=tr("heddle %1 %2 \n").arg(k+1).arg(sum[k]);
    }
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("analyze heddles"));
    msgBox.setText(result);
    msgBox.exec();
}

void Weave::modifySelected(int direction)
{
    if(pos!=pos1)
        return;
    if(pos!=pos_shaft && pos!=pos_position)
        return;
    bitField *target=&shafts;
    if(pos==pos_position){
        target=&positions;
        direction=direction-1;
        if(direction<0)
            direction=direction+4;
    }
    int start=qMin(origin_x0,origin_x1);
    int stop=qMax(origin_x0,origin_x1);
    m_undoStack.beginMacro("modify");
    QBitArray ba_helper;
    for(int x=start;x<=stop;x++){
        QBitArray ba=target->at(x);
        if(direction%2==0){
            int delta=1;
            int k=0;
            if(direction==0){
                delta=-1;
                k=ba.size()-1;
            }
            bool zw=ba.at(k);
            for(k+=delta;k<ba.size()&&k>=0;k+=delta){
                ba[k-delta]=ba.at(k);
            }
            ba[k-delta]=zw;
        }else{
            if(direction==1){
                if(x==start){
                    ba_helper=ba;
                    ba=target->at(stop);
                }else{
                    QBitArray zw=ba;
                    ba=ba_helper;
                    ba_helper=zw;
                }
            }else{
                if(x==start){
                    ba_helper=ba;
                }
                if(x==stop){
                    ba=ba_helper;
                }else{
                    ba=target->at(x+1);
                }
            }
        }
        auto *cp=new ChangeArray(target,x,ba);
        m_undoStack.push(cp);
    }
    m_undoStack.endMacro();
    generateWeave();
    update();
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

    if(newCols>nrCols){
        QBitArray line(nrShafts);
        for(int k=0;k<(newCols-nrCols);k++){
            shafts.prepend(line);
            colColors.prepend(clrUp);
        }
    }
    if(newCols<nrCols){
        colColors.resize(newCols);
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
    for(int k=0;k<newCols;k++){
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
    if(mode<op_selA){
        mouseMovePoint.setX(-1);
        mouseMovePoint.setY(-1);
    }
    if (event->button() == Qt::LeftButton) {
        if(mode==op_copy || mode==op_move){
            panePos pos0;
            int x,y;
            determinePos(event->pos(),pos0,x,y);
            if(pos==pos0){
                performCopy(x,y,mode==op_move,false,(pos==pos_position)||(pos==pos_lineColors)||(pos==pos_shaft)||(pos==pos_colColors));
            }else{
                if((pos==pos_position||pos==pos_shaft)&&(pos0==pos_position||pos0==pos_shaft)){
                    performCopy(x,y,mode==op_move,true,(pos==pos_position)||(pos==pos_shaft));
                }
            }

            mode=op_none;
            return;
        }
        if(mode==op_selB){
            mouseMovePoint=event->pos();
            update();
        }else{
            mousePressPoint=event->pos();
            if(mode==op_selA){
                update();
            }
        }
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
            x=nrCols-x-1;
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
                        auto *cp=new ChangeArray(&positions,yn,newLine);
                        m_undoStack.push(cp);
                    }
                }else{
                    QBitArray newLine(nrPositions);
                    newLine=positions[yn];
                    newLine.toggleBit(x);
                    auto *cp=new ChangeArray(&positions,yn,newLine);
                    m_undoStack.push(cp);
                }
            }else{
                if(x<0){
                    x=x+nrCols+xDist;
                    if( y>=0 && x<nrCols && y<nrShafts){
                        x=nrCols-x-1;
                        if(exclusiveShaft){
                            if(!shafts[x].at(nrShafts-y-1)){
                                //shafts[x].fill(false);
                                //shafts[x].setBit(nrShafts-y-1,true);
                                QBitArray newLine(nrShafts);
                                newLine.setBit(nrShafts-y-1,true);
                                auto *cp=new ChangeArray(&shafts,x,newLine);
                                m_undoStack.push(cp);
                            }
                        }else{
                            //shafts[x].toggleBit(nrShafts-y-1);
                            QBitArray newLine(nrShafts);
                            newLine=shafts[x];
                            newLine.toggleBit(nrShafts-y-1);
                            auto *cp=new ChangeArray(&shafts,x,newLine);
                            m_undoStack.push(cp);
                        }
                    }
                }else{
                    if( x>=0 && y>=0 && x<nrPositions && y<nrShafts){
                        //translation[nrShafts-y-1].toggleBit(x);
                        QBitArray newLine(nrPositions);
                        newLine=translation[nrShafts-y-1];
                        newLine.toggleBit(x);
                        auto *cp=new ChangeArray(&translation,nrShafts-y-1,newLine);
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
            x=nrCols-x-1;
            return;
        }
        y-=yOff;
        if( x<nrCols && y>nrShafts){
            y=y-nrShafts-yDist;
            if(y<nrLines)
                pos=pos_lines;
            return;
            //lines[y].toggleBit(x);
        }

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
        }
        if(x<0){
            x=x+nrCols+xDist;
            if( y>=0 && x<nrCols && y<nrShafts){
                pos=pos_shaft;
                y=nrShafts-y-1;
                x=nrCols-x-1;
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

void Weave::performCopy(int x, int y,bool clearSel,bool crossCopy,bool newOrignLeft)
{
    bitField *source=&shafts;
    bitField *target=&shafts;
    QVector<QColor> *source2=nullptr;
    QVector<QColor> *target2=nullptr;
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

    int targetSize=target->at(0).size();
    for(int i=0;i<field.size();i++){
        if(x+i<0 || x+i>=target->size())
            continue;
        QBitArray ba=field.at(i);
        ba.resize(targetSize);
        auto *cp=new ChangeArray(target,x+i,ba);
        m_undoStack.push(cp);
        if(target2){
            auto *ccp=new ChangeColorArray(target2,x+i,field2.at(i));
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
    if(mode>=op_selA) return;
    QPoint delta=event->pos()-mousePressPoint;
    if(delta.manhattanLength()<=1)
        clicked();
    //mousePressPoint.setX(-1);
}

void Weave::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePressPoint.x()>=0 && event->buttons()!=Qt::NoButton){
        mouseMovePoint=event->pos();
        QPoint delta=mousePressPoint-mouseMovePoint;
        if(delta.manhattanLength()>1){
            update();
        }
    }
    // generate info for status bar
    panePos pos;
    int x,y;
    determinePos(mouseMovePoint,pos,x,y);
    int zone=-1;
    if(pos==pos_shaft){
        zone=0;
        // reverse x
        x=nrCols-x;
    }
    if(pos==pos_translate)
        zone=1;
    if(pos==pos_position)
        zone=2;
    if(pos==pos_lines){
        zone=3;
        x=nrCols-x;
    }
    emit currentPosition(x,y,zone);
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

QString Weave::bitToString(const QBitArray& bits)
{
    // lsb left
    QString result;
    for(int j=0;j<bits.size();j++){
        result.append(bits.at(j) ? "1" : "0");
    }
    return result;
}

QBitArray Weave::stringToBit(const QString& txt)
{
    // lsb left
    QBitArray bit(txt.size());
    for(int j=0;j<txt.size();j++){
        if(txt.at(j)==QChar('1'))
            bit.setBit(j,true);
    }
    return bit;
}

QBitArray Weave::shiftBitArray(QBitArray ba,int shift)
{
    if(!shift)
        return ba;
    if(abs(shift)>1){
        int sgn=shift>0 ? 1 : -1;
        for(int k=0;k<abs(shift);k++){
            ba=shiftBitArray(ba,sgn);
        }
        return ba;
    }
    int delta=shift;
    int k=0;
    if(shift<0){
        k=ba.size()-1;
    }
    bool zw=ba.at(k);
    for(k+=delta;k<ba.size()&&k>=0;k+=delta){
        ba[k-delta]=ba.at(k);
    }
    ba[k-delta]=zw;
    return ba;
}

bitField Weave::transpose(const bitField& in)
{
    // intended for readWIF translation (tieup)
    // read in bitvector in y
    // needed bitvector in x
    bitField out;
    if(in.isEmpty())
        return out;
    int wx=in.length();
    int wy=in.at(0).size();
    for(int y=0;y<wy;y++){
        QBitArray ba(wx);
        for(int x=0;x<wx;x++){
            if(in.at(x).at(y)){
                ba.setBit(x,true);
            }
        }
        out<<ba;
    }
    return out;
}

void Weave::paint(QPainter &paint,int useScale)
{
    //QTime time;
    //time.start();
    //CALLGRIND_START_INSTRUMENTATION;
    if(useScale<1)
        useScale=scale;
    QBrush up(Qt::black);
    QBrush down(Qt::white);
    paint.setPen(Qt::darkGray);
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
        // draw one horizontal line for down
        paint.setBrush(lineColors.at(y));
        paint.setPen(Qt::SolidLine);
        paint.drawRect(0,(nrShafts+yDist+y+yOff)*useScale,nrCols*useScale,useScale);
        // draw up
        QPixmap cached(useScale+1,useScale+1);
        for (int x = 0; x < nrCols; ++x) {
            if(line.at(x)){
                QColor clr=colColors.at(x);
                if(!QPixmapCache::find(clr.name(),&cached)){
                    QPainter pt(&cached);
                    pt.setBrush(clr);
                    pt.setPen(Qt::NoPen);
                    pt.drawRect(0,0,useScale,useScale);
                    pt.setPen(Qt::SolidLine);
                    pt.drawLine(0,0,0,useScale);
                    pt.drawLine(useScale,0,useScale,useScale);
                    QPixmapCache::insert(clr.name(),cached);
                }
                paint.drawPixmap((nrCols-x-1)*useScale,(nrShafts+yDist+y+yOff)*useScale,useScale+1,useScale+1,cached);
                //x*useScale,(nrShafts+yDist+y+yOff)*useScale
            }
        }
        paint.restore();
        line=positions.at(y);
        for (int x = 0; x < nrPositions; ++x) {
            if(inSelectMode && (pos==pos_position || pos1==pos_position)){
                if(y>=qMin(origin_x0,origin_x1) && y<=qMax(origin_x0,origin_x1)){
                    paint.setPen(Qt::yellow);
                }else{
                    paint.setPen(Qt::darkGray);
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
            paint.setPen(Qt::darkGray);
        }
        // paint colour selector
        if(inSelectMode && (pos==pos_lineColors || pos1==pos_lineColors)){
            if(y>=qMin(origin_x0,origin_x1) && y<=qMax(origin_x0,origin_x1)){
                paint.setPen(Qt::yellow);
            }else{
                paint.setPen(Qt::darkGray);
            }
        }
        paint.setBrush(lineColors.at(y));
        paint.drawRect((nrCols+xDist+nrPositions+1)*useScale,(nrShafts+yDist+y+yOff)*useScale,useScale,useScale);
        if(inSelectMode && (pos==pos_lineColors || pos1==pos_lineColors)){
            paint.setPen(Qt::darkGray);
        }
    }
    //qDebug()<<time.elapsed();
    // colColours
    for (int x = 0; x < nrCols; ++x) {
        if(inSelectMode && (pos==pos_colColors || pos1==pos_colColors)){
            if(x>=qMin(origin_x0,origin_x1) && x<=qMax(origin_x0,origin_x1)){
                paint.setPen(Qt::yellow);
            }else{
                paint.setPen(Qt::darkGray);
            }
        }
        paint.setBrush(colColors.at(x));
        paint.drawRect((nrCols-x-1)*useScale,(0)*useScale,useScale,useScale);
        if(inSelectMode && (pos==pos_colColors || pos1==pos_colColors)){
            paint.setPen(Qt::darkGray);
        }
    }
    // measurement lines
    paint.save();
    paint.setPen(Qt::darkRed);
    bool large=true;
    for (int x = nrCols; x >= 0; x-=5) {
        if(large){
            paint.drawLine(x*useScale,(2)*useScale,x*useScale,qRound(1.5*useScale));
        }else{
            paint.drawLine(x*useScale,(2)*useScale,x*useScale,qRound(1.7*useScale));
        }
        large=!large;
    }
    large=true;
    for(int y=0;y<nrLines;y+=5){
        int xOff=(nrCols+nrPositions+xDist)*useScale;
        int yOffset=(yOff+nrShafts+1)*useScale;
        if(large){
            paint.drawLine(xOff,y*useScale+yOffset,qRound(xOff+0.5*useScale),y*useScale+yOffset);
        }else{
            paint.drawLine(xOff,y*useScale+yOffset,qRound(xOff+0.2*useScale),y*useScale+yOffset);
        }
        large=!large;
    }
    paint.restore();
    for(int y=0;y<nrShafts;y++){
        QBitArray line;
        for (int x = 0; x < nrCols; ++x) {
            line=shafts.at(x);
            if(inSelectMode && (pos==pos_shaft || pos1==pos_shaft)){
                if(x>=qMin(origin_x0,origin_x1) && x<=qMax(origin_x0,origin_x1)){
                    paint.setPen(Qt::yellow);
                }else{
                    paint.setPen(Qt::darkGray);
                }
            }
            if(line.at(y)){
                paint.setBrush(up);
            }else{
                paint.setBrush(down);
            }
            paint.drawRect((nrCols-x-1)*useScale,(nrShafts-y-1+yOff)*useScale,useScale,useScale);
        }
        if(inSelectMode && (pos==pos_shaft || pos1==pos_shaft)){
            paint.setPen(Qt::darkGray);
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
    //CALLGRIND_STOP_INSTRUMENTATION;
}

void Weave::duplicatePattern(int shift,int times)
{
    if(pos!=pos_shaft && pos!=pos_position && pos != pos_none)
        return;
    // duplicate and shift
    bitField *target=&shafts;

    if(pos==pos_position){
        target=&positions;
    }
    m_undoStack.beginMacro("duplicate");
    bitField zw=*target;
    int start=0;
    int delta=1;

    for(int i=0;i<zw.size()/(times+1);i++){
        QBitArray ba=zw.at(start+i*delta);
        auto *cp=new ChangeArray(target,start+(times+1)*i*delta,ba);
        m_undoStack.push(cp);
        for(int k=0;k<times;k++){
            ba=shiftBitArray(ba,shift);
            cp=new ChangeArray(target,start+(times+1)*i*delta+delta*(k+1),ba);
            m_undoStack.push(cp);
        }
    }
    m_undoStack.endMacro();
    generateWeave();
    update();
}

void Weave::generateColourPattern(const QList<QColor>& colors, const QList<int>& pattern, int side)
{
    m_undoStack.beginMacro("Pattern");
    int period=pattern.length();
    QVector<QColor> *target=&lineColors;
    int end=nrLines;
    if(side==1){
        target=&colColors;
        end=nrCols;
    }
    // pattern colours
    for(int k=0;k<end;k++){
        int j=k%period;
        j=pattern.at(j);
        if(j>=colors.length())
            j=0;
        if(j<0)
            j=0;
        //lineColors[k]=colors.at(j);
        auto *ccp=new ChangeColorArray(target,k,colors.at(j));
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

void Weave::setCursor(){
    auto *act=qobject_cast<QAction *>(sender());
    operationMode newMode=op_none;
    if(act!=nullptr){
        int i=act->data().toInt();
        switch (i) {
        case 0: newMode=op_selA;
        break;
        case 1: newMode=op_selB;
        break;
        default: ;
        }
        if(newMode!=mode){
            mode=newMode;
        }else{
            mode=op_none;
        }
    }

}
