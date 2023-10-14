#include "DeleteUndoCommand.h"
#include "ModelGraphicsView.h"

DeleteUndoCommand::DeleteUndoCommand(GraphicalModelComponent *gmc, ModelGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent), myGraphicalModelComponent(gmc), myGraphicsScene(scene) {
    initialPosition = QPointF(gmc->scenePos().x(), gmc->scenePos().y() + gmc->getHeight()/2);

    std::string position = "position=(x=" + std::to_string(gmc->scenePos().x()) + ", y=" + std::to_string(gmc->scenePos().y()) + ")";

    myGraphicsScene->update();
    setText(QObject::tr("Delete %1")
                .arg(QString::fromStdString("name=" + gmc->getComponent()->getName() + ", " + position)));
}

DeleteUndoCommand::~DeleteUndoCommand() {}

void DeleteUndoCommand::undo() {
    //add in model
    myGraphicsScene->getSimulator()->getModels()->current()->insert(myGraphicalModelComponent->getComponent());

    //graphically
    myGraphicsScene->addItem(myGraphicalModelComponent);
    myGraphicsScene->getGraphicalModelComponents()->append(myGraphicalModelComponent);

    //refaz as conexões
    myGraphicsScene->reconnectConnectionsOnRedoComponent(myGraphicalModelComponent);

    //notify graphical model change
    GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::CREATE, GraphicalModelEvent::EventObjectType::COMPONENT, myGraphicalModelComponent);
    dynamic_cast<ModelGraphicsView*> (myGraphicsScene->views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);

    myGraphicsScene->update();
}

void DeleteUndoCommand::redo() {
    //remove in model
    myGraphicsScene->removeModelComponentInModel(myGraphicalModelComponent);

    //graphically
    myGraphicsScene->removeItem(myGraphicalModelComponent);
    myGraphicsScene->getGraphicalModelComponents()->removeOne(myGraphicalModelComponent);

    //limpa as conexoes
    myGraphicsScene->handleClearConnectionsOnDeleteComponent(myGraphicalModelComponent);

    //notify graphical model change
    GraphicalModelEvent* modelGraphicsEvent = new GraphicalModelEvent(GraphicalModelEvent::EventType::REMOVE, GraphicalModelEvent::EventObjectType::COMPONENT, myGraphicalModelComponent);
    dynamic_cast<ModelGraphicsView*> (myGraphicsScene->views().at(0))->notifySceneGraphicalModelEventHandler(modelGraphicsEvent);

    myGraphicsScene->update();
}
