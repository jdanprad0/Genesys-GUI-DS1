#include "AnimationVariable.h"

AnimationVariable::AnimationVariable() : _counter(0), _isResizing(false){
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

void AnimationVariable::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QBrush brush;

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::blue);
    painter->drawRect(boundingRect());

    QString counterText = QString::number(_counter);
    QFont font = painter->font();
    font.setPixelSize(20);
    painter->setFont(font);
    painter->setPen(Qt::white);
    painter->drawText(boundingRect(), Qt::AlignCenter, counterText);

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

unsigned int AnimationVariable::getCounter() {
    return _counter;
}

QPointF AnimationVariable::getOldPosition() {
    return _oldPosition;
}

void AnimationVariable::setCounter(unsigned int value) {
    _counter = value;
    update();
}

void AnimationVariable::setOldPosition(QPointF oldPosition) {
    _oldPosition = oldPosition;
}

void AnimationVariable::startDrawing(QGraphicsSceneMouseEvent *event) {
    _isResizing = true;
    _startPoint = event->scenePos();
    setPos(_startPoint);
}

void AnimationVariable::continueDrawing(QGraphicsSceneMouseEvent *event) {
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

void AnimationVariable::stopDrawing(QGraphicsSceneMouseEvent *event) {
    // Cria um novo retângulo com as dimensões e posições corretas
    adjustSizeAndPosition(event);

    // Termina de desenhar/redimencionar o retângulo
    _isResizing = false;
}

void AnimationVariable::adjustSizeAndPosition(QGraphicsSceneMouseEvent *event) {
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

