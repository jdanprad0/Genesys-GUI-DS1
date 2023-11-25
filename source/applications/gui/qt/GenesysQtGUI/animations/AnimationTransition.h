#ifndef ANIMATIONTRANSITION_H
#define ANIMATIONTRANSITION_H

#include <QList>
#include <QPointF>
#include <QTimer>
#include <QVariantAnimation>
#include <QElapsedTimer>

#include "graphicals/GraphicalImageAnimation.h"
#include "graphicals/GraphicalModelComponent.h"
#include "graphicals/GraphicalConnection.h"

// Em segundos
#define TEMPO_EXECUCAO_ANIMACAO 1

class ModelGraphicsScene;

class AnimationTransition : public QVariantAnimation
{
public:
    // Construtor
    AnimationTransition(ModelGraphicsScene* myScene, ModelComponent* graphicalStartComponent, ModelComponent* graphicalEndComponent, const QString imageName);
    // Destrutor
    ~AnimationTransition();

    // Getters
    GraphicalModelComponent* getGraphicalStartComponent() const;
    GraphicalModelComponent* getGraphicalEndComponent() const;
    GraphicalConnection* getGraphicalConnection() const;
    static int getTimeExecution();
    QList<QPointF> getPointsForAnimation() const;
    GraphicalImageAnimation* getImageAnimation() const;
    unsigned int getPortNumber() const;

    // Setters
    void setImageAnimation(GraphicalImageAnimation* imageAnimation);
    static void setTimeExecution(int timeExecution);

    // Inicia a animação
    void startAnimation();
    // Finaliza a animação
    void stopAnimation();
    // Reinicia a animação
    void restartAnimation();
    // Configuração informações para a animação
    void configureAnimation();
    // Atualiza a duração da animação se ela for alterada em tempo de execução
    void updateDurationIfNeeded();
    // Conecta o sinal valueChanged da animação ao slot onAnimationValueChanged
    void connectValueChangedSignal();
    // Conecta o sinal finished da animação ao slot onAnimationFinished
    void connectFinishedSignal();
    // verifica se precisa adiconar animação de fila e adiciona se for o caso
    void verifyAddAnimationQueue();
    // verifica se precisa remover animação de fila e remove se for o caso
    void verifyRemoveAnimationQueue();
    // retorna a posição em que a animação da fila será desenhada
    QPointF calculatePositionImageQueue(GraphicalModelComponent *component, unsigned int indexQueue, unsigned int sizeQueue, unsigned int width, unsigned int height);

public slots:
    // Slot para ser conectado ao sinal valueChanged
    void onAnimationValueChanged(const QVariant& value);
    // Slot que será conectado a um sinal
    void onAnimationFinished();

private:
    ModelGraphicsScene* _myScene;
    GraphicalModelComponent* _graphicalStartComponent;
    GraphicalModelComponent* _graphicalEndComponent;
    GraphicalConnection* _graphicalConnection;
    static int *_timeExecution;
    static int _oldTimeExecution;
    QList<QPointF> _pointsForAnimation;
    GraphicalImageAnimation* _imageAnimation;
    unsigned int _portNumber;
    qreal _currentProgress;
};

#endif // ANIMATIONTRANSITION_H
