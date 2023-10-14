#ifndef DELETEUNDOCOMMAND_H
#define DELETEUNDOCOMMAND_H

#include <QUndoCommand>
#include "ModelGraphicsScene.h"
#include "graphicals/GraphicalModelComponent.h"

class DeleteUndoCommand : public QUndoCommand
{
public:
    explicit DeleteUndoCommand(GraphicalModelComponent *gmc, ModelGraphicsScene *scene, QUndoCommand *parent = nullptr);
    ~DeleteUndoCommand();

    void undo() override;
    void redo() override;

private:
    GraphicalModelComponent *myGraphicalModelComponent;
    ModelGraphicsScene *myGraphicsScene;
    QPointF initialPosition;
};

#endif // DELETEUNDOCOMMAND_H
