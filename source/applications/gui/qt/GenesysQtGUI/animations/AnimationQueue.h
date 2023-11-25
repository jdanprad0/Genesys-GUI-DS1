#ifndef ANIMATIONQUEUE_H
#define ANIMATIONQUEUE_H

#include "graphicals/GraphicalModelComponent.h"

class ModelGraphicsScene;

class AnimationQueue
{
public:
    // Construtor
    AnimationQueue(ModelGraphicsScene* myScene, ModelComponent* component);
    // Destrutor
    ~AnimationQueue();

    // Getters
    GraphicalModelComponent* getGraphicalComponent() const;

    // verifica se precisa adiconar animação de fila e adiciona se for o caso
    void verifyAddAnimationQueue();
    // verifica se precisa remover animação de fila e remove se for o caso
    void verifyRemoveAnimationQueue();
    // retorna a posição em que a animação da fila será desenhada
    QPointF calculatePositionImageQueue(unsigned int indexQueue, unsigned int sizeQueue, unsigned int width, unsigned int height);

private:
    ModelGraphicsScene* _myScene;
    GraphicalModelComponent* _graphicalComponent;
};

#endif // ANIMATIONQUEUE_H
