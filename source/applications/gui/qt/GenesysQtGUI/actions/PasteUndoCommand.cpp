#include "PasteUndoCommand.h"

PasteUndoCommand::PasteUndoCommand(QList<GraphicalModelComponent *> *graphicalComponents, QList<GraphicalConnection *> *connections, QList<QGraphicsItemGroup *> *groups, QList<QGraphicsItem *> *drawing, ModelGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent), _myComponentItems(new QList<ComponentItem>()), _myConnectionItems(connections), _myDrawingItems(drawing), _myGroupItems(new QList<GroupItem>()), _myGraphicsScene(scene) {

    for (int i = 0; i < graphicalComponents->size(); i++) {
        GraphicalModelComponent *component = graphicalComponents->at(i);
        ComponentItem componentItem;

        componentItem.graphicalComponent = component;
        componentItem.initialPosition = component->pos();

        if (!component->getGraphicalInputPorts().empty() && !component->getGraphicalInputPorts().at(0)->getConnections()->empty())
            componentItem.inputConnections.append(component->getGraphicalInputPorts().at(0)->getConnections()->at(0));

        for (int j = 0; j < component->getGraphicalOutputPorts().size(); ++j) {
            GraphicalComponentPort *port = component->getGraphicalOutputPorts().at(j);

            if (!port->getConnections()->empty())
                componentItem.outputConnections.append(port->getConnections()->at(0));
        }

        _myComponentItems->append(componentItem);
    }

    if (groups != nullptr) {
        for (int i = 0; i < groups->size(); i++) {
            QGraphicsItemGroup *group = groups->at(i);
            GroupItem groupItem;

            groupItem.group = group;

            for (int j = 0; j < group->childItems().size(); j++) {
                GraphicalModelComponent * component = dynamic_cast<GraphicalModelComponent *>(group->childItems().at(i));

                ComponentItem componentItem;

                componentItem.graphicalComponent = component;
                componentItem.initialPosition = component->pos();

                if (!component->getGraphicalInputPorts().empty() && !component->getGraphicalInputPorts().at(0)->getConnections()->empty())
                    componentItem.inputConnections.append(component->getGraphicalInputPorts().at(0)->getConnections()->at(0));

                for (int j = 0; j < component->getGraphicalOutputPorts().size(); ++j) {
                    GraphicalComponentPort *port = component->getGraphicalOutputPorts().at(j);

                    if (!port->getConnections()->empty())
                        componentItem.outputConnections.append(port->getConnections()->at(0));
                }

                groupItem.myComponentItems.append(componentItem);
            }
            _myGroupItems->append(groupItem);
        }
    }

    setText(QObject::tr("Paste"));
}

PasteUndoCommand::~PasteUndoCommand() {}

void PasteUndoCommand::undo() {
    // remove tudo que e grafico
    // comeca removendo os componentes e suas conexoes
    for (int i = 0; i < _myComponentItems->size(); ++i) {
        ComponentItem componentItem = _myComponentItems->at(i);

        _myGraphicsScene->removeItem(componentItem.graphicalComponent);

        for (int j = 0; j < _myComponentItems->at(i).inputConnections.size(); ++j) {
            GraphicalConnection *connection = componentItem.inputConnections.at(j);
            _myGraphicsScene->removeItem(connection);
        }

        for (int j = 0; j < _myComponentItems->at(i).outputConnections.size(); ++j) {
            GraphicalConnection *connection = componentItem.outputConnections.at(j);
            _myGraphicsScene->removeItem(connection);
        }
    }

    // remove as conexoes selecionadas individualmente
    for (int i = 0; i < _myConnectionItems->size(); ++i) {
        GraphicalConnection *connection = _myConnectionItems->at(i);
        _myGraphicsScene->removeItem(connection);
    }

    // varre todos os outros itens simples do tipo QGraphicsItem e remove da tela
//    for (int i = 0; i < _myDrawingItems->size(); ++i) {
//        QGraphicsItem *item = _myDrawingItems->at(i);

//        //add graphically
//        _myGraphicsScene->removeDrawing(item);
//    }

    // agora remove o que deve ser removido do modelo
    // varre todos os GraphicalModelComponent
    for (int i = 0; i < _myComponentItems->size(); ++i) {
        ComponentItem componentItem = _myComponentItems->at(i);

        _myGraphicsScene->removeComponent(componentItem.graphicalComponent);
    }

    for (int i = 0; i < _myGroupItems->size(); ++i) {
        QGraphicsItemGroup *group = _myGroupItems->at(i).group;

        _myGraphicsScene->removeGroup(group);
    }

    // varre todos os GraphicalConnection
    for (int i = 0; i < _myConnectionItems->size(); ++i) {
        GraphicalConnection *connection = _myConnectionItems->at(i);
        GraphicalModelComponent *source = _myGraphicsScene->findGraphicalModelComponent(connection->getSource()->component->getId());
        GraphicalModelComponent *destination = _myGraphicsScene->findGraphicalModelComponent(connection->getDestination()->component->getId());

        // verifica se a conexao ainda existe, pois ela pode ja ter sido removida caso fizesse parte de um componente que foi removido
        if (_myGraphicsScene->getGraphicalConnections()->contains(connection)) {
            // se ela existe, a remove
            _myGraphicsScene->removeGraphicalConnection(connection, source, destination);
        } else {
            // se nao existe, quer dizer que a conexao faz parte de um componente que foi removido, e portando ela ja foi removida
            // entao a remove da lista
            _myConnectionItems->removeOne(connection);
        }
    }

    GraphicalModelEvent::EventType eventType = GraphicalModelEvent::EventType::REMOVE;
    GraphicalModelEvent::EventObjectType eventObjectType = GraphicalModelEvent::EventObjectType::OTHER;

    _myGraphicsScene->notifyGraphicalModelChange(eventType, eventObjectType, nullptr);

    _myGraphicsScene->update();
}

void PasteUndoCommand::redo() {
    // adiciona tudo que e grafico
    // comeca adicionando os componentes e suas conexoes
    for (int i = 0; i < _myComponentItems->size(); ++i) {
        ComponentItem componentItem = _myComponentItems->at(i);

        componentItem.initialPosition.setX(componentItem.initialPosition.x() + 100);

        _myGraphicsScene->addItem(componentItem.graphicalComponent);

        for (int j = 0; j < _myComponentItems->at(i).inputConnections.size(); ++j) {
            GraphicalConnection *connection = componentItem.inputConnections.at(j);
            _myGraphicsScene->addItem(connection);
        }

        for (int j = 0; j < _myComponentItems->at(i).outputConnections.size(); ++j) {
            GraphicalConnection *connection = componentItem.outputConnections.at(j);
            _myGraphicsScene->addItem(connection);
        }
    }

    // adiciona as conexoes selecionadas individualmente
    for (int i = 0; i < _myConnectionItems->size(); ++i) {
        GraphicalConnection *connection = _myConnectionItems->at(i);
        _myGraphicsScene->addItem(connection);
    }

    // varre todos os outros itens simples do tipo QGraphicsItem e adiciona da tela
    for (int i = 0; i < _myDrawingItems->size(); ++i) {
        QGraphicsItem *item = _myDrawingItems->at(i);
        _myGraphicsScene->addItem(item);
    }

    // agora comeca a adicionar o que se deve no modelo
    // varre todos os GraphicalModelComponent
    for (int i = 0; i < _myComponentItems->size(); ++i) {
        ComponentItem componentItem = _myComponentItems->at(i);

        //add in model (apenas para delete)
        _myGraphicsScene->getSimulator()->getModels()->current()->insert(componentItem.graphicalComponent->getComponent());

        //add graphically
        _myGraphicsScene->getGraphicalModelComponents()->append(componentItem.graphicalComponent);

        //refaz as conexões
        _myGraphicsScene->redoConnections(componentItem.graphicalComponent, &componentItem.inputConnections, &componentItem.outputConnections);
    }

    // varre todos os GraphicalConnection e realiza a conexao
    for (int i = 0; i < _myConnectionItems->size(); ++i) {
        GraphicalConnection *connection = _myConnectionItems->at(i);

        // verifico se a conexao ainda nao existe (ela pode ser uma conexao que foi refeita no connectComponents)
        // por exemplo, pode ter uma conexao selecionada que foi eliminada anteriormente pois ela fazia parte de um componente
        // entao ao refazer as conexoes do componente, ela ja foi religada
        if (!_myGraphicsScene->getGraphicalConnections()->contains(connection)) {
            _myGraphicsScene->connectComponents(connection, nullptr, nullptr);
        }
    }

    // volta os grupos no modelo
    for (int i = 0; i < _myGroupItems->size(); ++i) {
        GroupItem groupItem = _myGroupItems->at(i);
        QList<GraphicalModelComponent *> *componentsGroup = new QList<GraphicalModelComponent *>();

        for (int j = 0; j < groupItem.myComponentItems.size(); ++j) {
            ComponentItem componentItem = groupItem.myComponentItems.at(j);
            componentsGroup->append(componentItem.graphicalComponent);
        }

        _myGraphicsScene->groupModelComponents(componentsGroup, groupItem.group);

        delete componentsGroup;
    }

    _myGraphicsScene->notifyGraphicalModelChange(GraphicalModelEvent::EventType::CREATE, GraphicalModelEvent::EventObjectType::OTHER, nullptr);

    // atualiza a cena
    _myGraphicsScene->update();
}
