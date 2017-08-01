#ifndef MODIFYDLG_H
#define MODIFYDLG_H

#include <QDialog>
#include <QtWidgets>

class Modifydlg : public QDialog
{
    Q_OBJECT
public:
    explicit Modifydlg(QWidget *parent = nullptr);

signals:
    void flipHorz();
    void flipVert();
    void move(int direction);

public slots:
    void movePressed();
};

#endif // MODIFYDLG_H
