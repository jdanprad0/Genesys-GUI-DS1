#include "AnimationCounter.h"
#include "graphicals/GraphicalModelComponent.h"

AnimationCounter::AnimationCounter() : _isDrawingInicialized(true) {
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

void AnimationCounter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QBrush brush;

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::blue);
    painter->drawRect(boundingRect());

    QString valueText = QString::number(_value);
    QFont font = painter->font();

    // Ajusta o tamanho da fonte com base nas dimensões do retângulo
    int fontSize = qMin(boundingRect().width() / 5, boundingRect().height() / 2);
    font.setPixelSize(fontSize);

    painter->setFont(font);
    painter->setPen(Qt::white);
    painter->drawText(boundingRect(), Qt::AlignCenter, valueText);

    if (isSelected()) {
        // Tamanho dos quadrados dos cantos
        qreal cornerSize = 10.0;

        // Sem borda
        painter->setPen(Qt::NoPen);

        // Cor dos quadrados
        painter->setBrush(Qt::black);

        // Cantos superiores
        painter->drawRect(QRectF(-cornerSize, -cornerSize, cornerSize, cornerSize));
        painter->drawRect(QRectF(boundingRect().topRight() - QPointF(0, cornerSize), QSizeF(cornerSize, cornerSize)));

        // Cantos inferiores
        painter->drawRect(QRectF(-cornerSize, boundingRect().height(), cornerSize, cornerSize));
        painter->drawRect(QRectF(boundingRect().bottomRight(), QSizeF(cornerSize, cornerSize)));
    }
}

double AnimationCounter::getValue() {
    return _value;
}

QPointF AnimationCounter::getOldPosition() {
    return _oldPosition;
}

GraphicalModelComponent *AnimationCounter::getOwnerComponent(){
    return _ownerComponent;
}

Counter *AnimationCounter::getCounter(){
    if (Counter *counter = dynamic_cast<Counter *>(_counter)) {
        return _counter;
    } else {
        _counter = nullptr;
        return nullptr;
    }
}

void AnimationCounter::setValue(double value) {
    _value = value;
    update();
}

void AnimationCounter::setOldPosition(QPointF oldPosition) {
    _oldPosition = oldPosition;
}

void AnimationCounter::setOwnerComponent(GraphicalModelComponent *ownerComponent){
    _ownerComponent = ownerComponent;
}

void AnimationCounter::setCounter(Counter *counter){
    _counter = counter;
}

void AnimationCounter::startDrawing(QGraphicsSceneMouseEvent *event) {
    _isResizing = true;
    _startPoint = event->scenePos();
    setPos(_startPoint);
}

void AnimationCounter::continueDrawing(QGraphicsSceneMouseEvent *event) {
    if (_isResizing) {
        // Obtém a posição atual do mouse
        QPointF newPos = event->scenePos();

        // Calcula a diferença entre a posição atual do mouse e a posição inicial do retângulo
        QPointF delta = newPos - _startPoint;

        // Calcula o novo retângulo com base na diferença de posição do mouse
        QRectF newRect = QRectF(0, 0, delta.x(), delta.y());

        // Troca o retângulo atual para o novo com as modificações
        setRect(newRect.normalized());

        // Atualiza o item na cena
        update();
    }
}

void AnimationCounter::stopDrawing(QGraphicsSceneMouseEvent *event) {
    // Cria um novo retângulo com as dimensões e posições corretas
    adjustSizeAndPosition(event);

    // Termina de desenhar/redimencionar o retângulo
    _isResizing = false;

    // Indica que acabou de desenhar o objeto
    _isDrawingFinalized = true;
}

void AnimationCounter::adjustSizeAndPosition(QGraphicsSceneMouseEvent *event) {
    qreal minimunX = 0.0;
    qreal minimunY = 0.0;

    qreal maximunX = 0.0;
    qreal maximunY = 0.0;

    if (_startPoint.x() <= event->scenePos().x()) {
        minimunX = _startPoint.x();
        maximunX = event->scenePos().x();
    }
    else {
        minimunX = event->scenePos().x();
        maximunX = _startPoint.x();
    }

    if (_startPoint.y() <= event->scenePos().y()) {
        minimunY = _startPoint.y();
        maximunY = event->scenePos().y();
    }
    else {
        minimunY = event->scenePos().y();
        maximunY = _startPoint.y();
    }

    qreal width = maximunX - minimunX;
    qreal height = maximunY - minimunY;

    // Calcula o novo retângulo com base na diferença de posição do mouse
    QRectF newRect = QRectF(0, 0, width, height);

    // Troca o retângulo atual para o novo com as modificações
    setRect(newRect.normalized());

    QPointF position = QPointF(minimunX, minimunY);

    // Seta a posição
    setPos(position);
    _oldPosition = position;

    // Atualiza o item na cena
    update();
}

bool AnimationCounter::isDrawingInicialized() {
    return _isDrawingInicialized;
}

bool AnimationCounter::isDrawingFinalized() {
    return _isDrawingFinalized;
}

