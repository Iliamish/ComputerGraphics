#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CGlab3.h"

class CGlab3 : public QMainWindow
{
    Q_OBJECT

public:
    CGlab3(QWidget *parent = Q_NULLPTR);

private:
    Ui::CGlab3Class ui;
};
