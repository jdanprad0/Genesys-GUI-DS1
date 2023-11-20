#ifndef ANIMATIONCOUNTER_H
#define ANIMATIONCOUNTER_H

#include <QGraphicsRectItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "../../../../../kernel/simulator/Counter.h"

class GraphicalModelComponent;

class AnimationCounter : public QGraphicsRectItem {
public:
    // Construtor
    AnimationCounter();

    // Sobrecarga do método paint
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // Getters
    double getValue();
    QPointF getOldPosition();
    GraphicalModelComponent *getOwnerComponent();
    Counter *getCounter();

    // Setters
    void setValue(double value);
    void setOldPosition(QPointF oldPosition);
    void setOwnerComponent(GraphicalModelComponent *ownerComponent);
    void setCounter(Counter *myCounter);

    // Outros
    void startDrawing(QGraphicsSceneMouseEvent *event); // Inicia o desenho da tela
    void continueDrawing(QGraphicsSceneMouseEvent *event); // Continua o desenho na tela
    void stopDrawing(QGraphicsSceneMouseEvent *event); // Finaliza o desenho na tela
    void adjustSizeAndPosition(QGraphicsSceneMouseEvent *event); // Ajusta posição e dimensão do retângulo no final
    bool isDrawingInicialized(); // Diz se a variável começou a ser desenhada
    bool isDrawingFinalized(); // Diz se a variável terminou de ser desenhada

private:
    double _value = 0.0;
    QPointF _startPoint = QPointF(0, 0);
    QPointF _oldPosition = QPointF(0, 0);
    GraphicalModelComponent *_ownerComponent = nullptr;
    bool _isResizing = false;
    bool _isDrawingInicialized = false;
    bool _isDrawingFinalized = false;
    Counter *_counter = nullptr;
};

#endif // ANIMATIONCOUNTER_H
