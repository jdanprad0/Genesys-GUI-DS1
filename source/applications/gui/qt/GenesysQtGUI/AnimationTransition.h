#ifndef ANIMATIONTRANSITION_H
#define ANIMATIONTRANSITION_H

#include <QList>
#include <QPointF>
#include <QTime>
#include <QVariantAnimation>

#include "graphicals/GraphicalModelComponent.h"
#include "graphicals/GraphicalConnection.h"
#include "graphicals/GraphicalImageAnimation.h"

class ModelGraphicsScene;

class AnimationTransition : public QVariantAnimation
{
public:
    // Construtor
    AnimationTransition(ModelGraphicsScene* myScene, GraphicalModelComponent* graphicalStartComponent, const double timeStart, const QString imageName = "default.png");
    // Destrutor
    ~AnimationTransition();

    // Getters
    GraphicalModelComponent* getGraphicalStartComponent() const;
    GraphicalModelComponent* getGraphicalEndComponent() const;
    GraphicalConnection* getGraphicalConnection() const;
    double getTimeStart() const;
    QList<QPointF> getPointsForAnimation() const;
    GraphicalImageAnimation* getImageAnimation() const;

    // Setters
    void setImageAnimation(GraphicalImageAnimation* imageAnimation);

    // Inicia a animação
    void startAnimation();
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
    // Função a ser chamada ao término da execução da animação

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
    double _timeStart;
    QList<QPointF> _pointsForAnimation;
    GraphicalImageAnimation* _imageAnimation;
    double *_timeExecution;
    double _oldTimeExecution;
    qreal _currentProgress;
};

#endif // ANIMATIONTRANSITION_H
