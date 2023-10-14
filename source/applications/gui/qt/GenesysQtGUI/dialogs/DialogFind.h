#ifndef DIALOGFIND_H
#define DIALOGFIND_H

#include <QDialog>
#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include "../graphicals/GraphicalModelComponent.h"
#include "ModelGraphicsScene.h"
#include <QGraphicsItem>
#include <QList>


class QLineEdit;
class QPushButton;

class MyLineEdit : public QLineEdit {
    Q_OBJECT

public:
    MyLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {
        connect(this, &QLineEdit::textChanged, this, &MyLineEdit::onTextChanged);
    }

private slots:
    void onTextChanged(const QString &text) {
//        qDebug() << "Texto atual: " << text;

        if (text.length() < previousText.length()) {
            this->setStyleSheet("color: black");
        }

        previousText = text;
    }

private:
    QString previousText;
};


class DialogFind : public QDialog
{
    Q_OBJECT

public:
    DialogFind(QWidget *parent = nullptr, ModelGraphicsScene * scene = nullptr);
    QString getFindText();

public slots:
    void findClicked();
    void nextClicked();
    void previousClicked();

private:
    //QLabel * countLabel;
    QPushButton *findButton;
    QPushButton *nextButton;
    QPushButton *previousButton;
    MyLineEdit *lineEdit;
    bool clicked;
    int counter;
    QList<QGraphicsItem*>* finded = new QList<QGraphicsItem*>();
    ModelGraphicsScene * scene;

};

#endif // DIALOGFIND_H


