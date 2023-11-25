#include "AnimationTransition.h"
#include "ModelGraphicsScene.h"
#include "graphicals/GraphicalImageAnimation.h"

// Inicializando variáveis estáticas
int* AnimationTransition::_timeExecution = new int(TEMPO_EXECUCAO_ANIMACAO); // Define um valor inicial para o timeExecution
int AnimationTransition::_oldTimeExecution = TEMPO_EXECUCAO_ANIMACAO;

AnimationTransition::AnimationTransition(ModelGraphicsScene* myScene, ModelComponent* graphicalStartComponent, ModelComponent* graphicalEndComponent, const QString imageName) :
    _myScene(myScene),
    _graphicalEndComponent(nullptr),
    _imageAnimation(nullptr) {

    // Pega o componente gráfico de início e fim da animação
    _graphicalStartComponent = _myScene->findGraphicalModelComponent(graphicalStartComponent->getId());
    _graphicalEndComponent = _myScene->findGraphicalModelComponent(graphicalEndComponent->getId());

    if (_graphicalStartComponent && !_graphicalStartComponent->getGraphicalOutputPorts().empty()) {
        QList<GraphicalComponentPort *> startComponentOutputPorts = _graphicalStartComponent->getGraphicalOutputPorts();
        GraphicalConnection* connection = nullptr;

        // Pega a conexão gráfica em que a animação de transição irá percorrer
        for (unsigned int i = 0; i < (unsigned int) startComponentOutputPorts.size(); i++) {
            if (!startComponentOutputPorts.at(i)->getConnections()->empty()) {
                if (startComponentOutputPorts.at(i)->getConnections()->at(0)->getDestination()->component->getId() == _graphicalEndComponent->getComponent()->getId()) {
                    connection = startComponentOutputPorts.at(i)->getConnections()->at(0);
                    _portNumber = i;
                    break;
                }
            }
        }

        // Pega o componente de destino do evento/animação de transição
        ModelComponent *destinationComponent;
        if (connection == nullptr)
            return;

        destinationComponent = connection->getDestination()->component;
        _graphicalEndComponent = _myScene->findGraphicalModelComponent(destinationComponent->getId());

        // Pega os pontos na tela em que a animação deve ocorrer
        QList<QPointF> pointsConnection = connection->getPoints();

        // Tamanho para imagem
        const int imageWidth = 50;
        const int imageHeight = 50;

        // Configurando pontos a serem percorridos na animação
        QPointF startPoint = QPointF(pointsConnection.first().x(), pointsConnection.first().y() - (imageHeight / 2));
        QPointF firstIntermediatePoint = QPointF(pointsConnection.at(1).x() - (imageWidth / 4), pointsConnection.at(1).y() - (imageHeight / 2));
        QPointF secondIntermediatePoint = QPointF(pointsConnection.at(2).x() - (imageWidth / 4), pointsConnection.at(2).y() - (imageHeight / 2));
        QPointF endPoint = QPointF(pointsConnection.last().x() - imageWidth, pointsConnection.last().y() - imageHeight / 2);

        // Adiciona novos pontos à lista
        _pointsForAnimation.append(startPoint);
        _pointsForAnimation.append(firstIntermediatePoint);
        _pointsForAnimation.append(secondIntermediatePoint);
        _pointsForAnimation.append(endPoint);

        // Carrega uma imagem
        _imageAnimation = new GraphicalImageAnimation(startPoint, imageWidth, imageHeight, imageName);

        // Configura a animação
        configureAnimation();
    }
}

AnimationTransition::~AnimationTransition(){
    this->stopAnimation();
}

// Getters
GraphicalModelComponent* AnimationTransition::getGraphicalStartComponent() const {
    return _graphicalStartComponent;
}

GraphicalModelComponent* AnimationTransition::getGraphicalEndComponent() const {
    return _graphicalEndComponent;
}

GraphicalConnection* AnimationTransition::getGraphicalConnection() const {
    return _graphicalConnection;
}

int AnimationTransition::getTimeExecution() {
    return *_timeExecution;
}

QList<QPointF> AnimationTransition::getPointsForAnimation() const {
    return _pointsForAnimation;
}

GraphicalImageAnimation* AnimationTransition::getImageAnimation() const {
    return _imageAnimation;
}

unsigned int AnimationTransition::getPortNumber() const {
    return _portNumber;
}

// Setters
void AnimationTransition::setImageAnimation(GraphicalImageAnimation* imageAnimation) {
    _imageAnimation = imageAnimation;
}

void AnimationTransition::setTimeExecution(int timeExecution) {
    *_timeExecution = timeExecution;
}

// Outros
void AnimationTransition::startAnimation() {
    // Adiciona a imagem na cena
    _myScene->addItem(_imageAnimation);

    // Inicia a animação
    start();
}

void AnimationTransition::stopAnimation() {
    // Remove a imagem na cena
    _myScene->removeItem(_imageAnimation);

    // Para a animação
    stop();
}

void AnimationTransition::restartAnimation() {
    // Configura o progresso
    setCurrentTime(_currentProgress * duration());

    // Reinicia a animação
    start();
}

void AnimationTransition::configureAnimation() {
    // Configura informações da animação
    setDuration(this->getTimeExecution() * 1000); // Define a duração da animação (em ms, porém _timeExecution é dado em s)
    setStartValue(0.0); // Valor para ponto de partida do progresso da animação
    setEndValue(1.0); // Valor atingido ao término da animação
    setEasingCurve(QEasingCurve::Linear); // Curva de atenuação entre os pontos no progresso da animação

    connectValueChangedSignal();
    connectFinishedSignal();
}

void AnimationTransition::updateDurationIfNeeded() {
    if (_oldTimeExecution != this->getTimeExecution()) {
        _oldTimeExecution = this->getTimeExecution();
        setDuration(this->getTimeExecution() * 1000);

        // reinicia a animação
        restartAnimation();
    }
}

void AnimationTransition::connectValueChangedSignal() {
    // Conecta o sinal valueChanged ao slot onAnimationValueChanged
    QObject::connect(this, &QVariantAnimation::valueChanged, this, &AnimationTransition::onAnimationValueChanged);
}

void AnimationTransition::onAnimationValueChanged(const QVariant& value) {
    updateDurationIfNeeded();

    // Progresso atual da animação (valor entre startValue e endValue)
    _currentProgress = value.toReal();

    // Se há pontos a serem percorridos, entra na condição
    if (!_pointsForAnimation.isEmpty()) {
        // Número de segmentos, ou seja, quantas linhas serão percorridas
        int numSegments = _pointsForAnimation.size() - 1;

        // Distância total a ser percorrida pela animação
        qreal totalDistance = 0.0;

        // Calcula a distância total entre os pontos
        for (int i = 0; i < numSegments; ++i) {
            totalDistance += QLineF(_pointsForAnimation[i], _pointsForAnimation[i + 1]).length();
        }

        qreal distanceCovered = _currentProgress * totalDistance;
        qreal currentDistance = 0.0;

        // Encontra o segmento onde a imagem está atualmente
        int currentSegment = 0;
        while (currentSegment < numSegments && currentDistance + QLineF(_pointsForAnimation[currentSegment], _pointsForAnimation[currentSegment + 1]).length() < distanceCovered) {
            currentDistance += QLineF(_pointsForAnimation[currentSegment], _pointsForAnimation[currentSegment + 1]).length();
            ++currentSegment;
        }

        // Calcula a posição interpolada dentro do segmento atual
        qreal segmentProgress = (distanceCovered - currentDistance) / QLineF(_pointsForAnimation[currentSegment], _pointsForAnimation[currentSegment + 1]).length();
        QPointF start = _pointsForAnimation[currentSegment];
        QPointF end = _pointsForAnimation[currentSegment + 1];
        QPointF imagePosition = start * (1 - segmentProgress) + end * segmentProgress;

        // Nova posição da imagem
        _imageAnimation->setPos(imagePosition);
    }
}

void AnimationTransition::connectFinishedSignal() {
    // Conecta o sinal finished ao slot onAnimationFinished
    QObject::connect(this, &QVariantAnimation::finished, this, &AnimationTransition::onAnimationFinished);
}

void AnimationTransition::onAnimationFinished() {
    _myScene->removeItem(_imageAnimation);
}

void AnimationTransition::verifyAddAnimationQueue() {
    if (_graphicalEndComponent) {
        if (_graphicalEndComponent->hasQueue()) {
            QList<Queue *> queues = _graphicalEndComponent->getMapQueue()->keys();

            for (Queue *queue : queues) {
                unsigned int newQueueSize = (unsigned int) queue->size();
                unsigned int oldQueueSize = _graphicalEndComponent->getSizeQueue(queue);
                unsigned int indexQueue = _graphicalEndComponent->getIndexQueue(queue);

                if (newQueueSize > oldQueueSize) {
                    unsigned int width = 30;
                    unsigned int height = 30;
                    QString animationImageName = _graphicalEndComponent->getAnimationImageName();

                    QPointF position = calculatePositionImageQueue(_graphicalEndComponent, indexQueue, newQueueSize, width, height);

                    GraphicalImageAnimation *image = new GraphicalImageAnimation(position, width, height, animationImageName);

                    _graphicalEndComponent->insertImageQueue(queue, image);
                    _myScene->addItem(image);
                    _myScene->update();
                }
            }
        }
    }
}

void AnimationTransition::verifyRemoveAnimationQueue() {
    if (_graphicalStartComponent) {
        if (_graphicalStartComponent->hasQueue()) {
            QList<Queue *> queues = _graphicalStartComponent->getMapQueue()->keys();

            for (Queue *queue : queues) {
                unsigned int newQueueSize = (unsigned int) queue->size();
                unsigned int oldQueueSize = _graphicalStartComponent->getSizeQueue(queue);

                if (newQueueSize < oldQueueSize) {
                    unsigned int quantityRemoved = oldQueueSize - newQueueSize;

                    QList<GraphicalImageAnimation *> *imageRemoved = _graphicalStartComponent->removeImageQueue(queue, quantityRemoved);
                    if (imageRemoved) {
                        for (unsigned int i = 0; i < quantityRemoved; i++) {
                            // pega a imagem a ser removida
                            GraphicalImageAnimation *image = imageRemoved->at(i);

                            // remove ela da cena
                            _myScene->removeItem(image);
                            // libera o espaço de memória dela
                            delete image;
                        }
                        // atualiza a cena
                        _myScene->update();

                        // limpa a lista que foi retornada
                        imageRemoved->clear();
                        // libera o espaço de memória da lista
                        delete imageRemoved;
                    }
                }
            }
        }
    }
}

QPointF AnimationTransition::calculatePositionImageQueue(GraphicalModelComponent *component, unsigned int indexQueue, unsigned int sizeQueue, unsigned int width, unsigned int height) {
    unsigned int multiplierX = sizeQueue;
    unsigned int multiplierY = indexQueue + 1;
    unsigned int spaceBetween = 2;

    qreal gmcPosX = component->sceneBoundingRect().topRight().x();
    qreal gmcPosY = component->sceneBoundingRect().topRight().y();

    qreal calculatePositionX = gmcPosX - ((multiplierX * width) + ((multiplierX - 1) * spaceBetween));
    qreal calculatePositionY = gmcPosY - ((height * multiplierY) + (multiplierY * spaceBetween));

    QPointF position(calculatePositionX, calculatePositionY);

    return position;
}
