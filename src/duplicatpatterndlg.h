#ifndef DUPLICATPATTERNDLG_H
#define DUPLICATPATTERNDLG_H

#include <QDialog>
#include <QtWidgets>

class DuplicatPatternDlg : public QDialog
{
    Q_OBJECT
public:
    explicit DuplicatPatternDlg(QWidget *parent = nullptr);

    QSpinBox *sp,*sp2;
signals:

public slots:
};

#endif // DUPLICATPATTERNDLG_H
