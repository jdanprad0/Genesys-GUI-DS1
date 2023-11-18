#ifndef ANIMATIONVARIABLE_H
#define ANIMATIONVARIABLE_H

#include <QGraphicsRectItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

class AnimationVariable : public QGraphicsRectItem {
public:
    // Construtor
    AnimationVariable();

    // Sobrecarga do método paint
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // Getters
    unsigned int getCounter();
    QPointF getOldPosition();

    // Setters
    void setCounter(unsigned int value);
    void setOldPosition(QPointF oldPosition);

    // Outros
    void startDrawing(QGraphicsSceneMouseEvent *event); // Inicia o desenho da tela
    void continueDrawing(QGraphicsSceneMouseEvent *event); // Continua o desenho na tela
    void stopDrawing(QGraphicsSceneMouseEvent *event); // Finaliza o desenho na tela
    void adjustSizeAndPosition(QGraphicsSceneMouseEvent *event); // Ajusta posição e dimensão do retângulo no final

private:
    unsigned int _counter;
    bool _isResizing;
    QPointF _startPoint;
    QPointF _oldPosition;
};

#endif // ANIMATIONVARIABLE_H
