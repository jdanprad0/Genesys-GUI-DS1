#include "MoveUndoCommand.h"
#include "ModelGraphicsScene.h"

MoveUndoCommand::MoveUndoCommand(QList<GraphicalModelComponent*> gmc, ModelGraphicsScene *scene, QList<QPointF> &oldPos, QList<QPointF> &newPos, QUndoCommand *parent)
    : QUndoCommand(parent), _myGraphicalModelComponent(gmc), _myGraphicsScene(scene), _myOldPos(oldPos), _myNewPos(newPos), _firstExecution(true) {
}

MoveUndoCommand::~MoveUndoCommand() {}

void MoveUndoCommand::undo() {

    for (int i = 0; i < _myGraphicalModelComponent.size(); i++) {
        GraphicalModelComponent *item = _myGraphicalModelComponent[i];
        QPointF oldPos = _myOldPos[i];

        item->setPos(oldPos);
        item->setOldPosition(oldPos);
        _myGraphicsScene->update();
    }

    _myGraphicsScene->update();
}

void MoveUndoCommand::redo() {
    if (!_firstExecution) {
        for (int i = 0; i < _myGraphicalModelComponent.size(); i++) {
            GraphicalModelComponent *item = _myGraphicalModelComponent[i];
            QPointF newPos = _myNewPos[i];

            item->setPos(newPos);
            _myGraphicsScene->update();
        }
    }
    _firstExecution = false;

    _myGraphicsScene->update();
}
