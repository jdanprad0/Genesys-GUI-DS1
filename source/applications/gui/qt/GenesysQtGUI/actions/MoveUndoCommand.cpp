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
    //_myGraphicsScene->clearSelection();
    _firstExecution = false;

    _myGraphicsScene->update();
}

//bool MoveUndoCommand::mergeWith(const QUndoCommand *command) {
//    const MoveUndoCommand *moveCommand = static_cast<const MoveUndoCommand *>(command);
//    QList<GraphicalModelComponent *> itensCommand = moveCommand->_myGraphicalModelComponent;

//    struct indexesPosition {
//        int oldGmc;
//        int newGmc;
//    };

//    QList<indexesPosition> peersIndex;

//    if (itensCommand.size() != _myGraphicalModelComponent.size())
//        return false;

//    for (int i = 0; i < _myGraphicalModelComponent.size(); i++) {
//        bool dif = false;
//        indexesPosition peer;

//        for (int j = 0; j < itensCommand.size(); j++) {
//            GraphicalModelComponent *itemCommand = itensCommand[j];
//            if (_myGraphicalModelComponent[i] == itemCommand) {
//                dif = true;
//                peer.oldGmc = i;
//                peer.newGmc = j;
//                peersIndex.append(peer);
//                break;
//            }
//        }

//        if (!dif)
//            return false;
//    }

//    for (int k = 0; k < peersIndex.size(); k++) {
//        _myNewPos[peersIndex[k].newGmc] = itensCommand[peersIndex[k].oldGmc]->scenePos();
//    }

//    return true;
//}
