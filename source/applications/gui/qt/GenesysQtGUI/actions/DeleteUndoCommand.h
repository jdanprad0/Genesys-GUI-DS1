#ifndef DELETEUNDOCOMMAND_H
#define DELETEUNDOCOMMAND_H

#include <QUndoCommand>
#include "ModelGraphicsScene.h"
#include "graphicals/GraphicalConnection.h"
#include "graphicals/GraphicalModelComponent.h"

struct ComponentItem {
    GraphicalModelComponent *graphicalComponent;
    QPointF initialPosition;
    QList<GraphicalConnection *> inputConnections;
    QList<GraphicalConnection *> outputConnections;
};

class DeleteUndoCommand : public QUndoCommand
{
public:
    explicit DeleteUndoCommand(QList<QGraphicsItem *> items, ModelGraphicsScene *scene, QUndoCommand *parent = nullptr);
    ~DeleteUndoCommand();

    void undo() override;
    void redo() override;

private:
    QList<ComponentItem> *_myComponentItems;
    QList<GraphicalConnection *> *_myConnectionItems;
    QList<QGraphicsItem *> *_myDrawingItems;
    ModelGraphicsScene *_myGraphicsScene;
};

#endif // DELETEUNDOCOMMAND_H
