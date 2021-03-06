/*
 * OperationDiagramView.cpp
 *
 *  Created on: Jun 19, 2015
 *  \copyright 2015 DCBlaha.  Distributed under the GPL.
 */

#include "OperationDiagramView.h"
#include "Svg.h"
#include "DiagramDrawer.h"
#include "Options.h"

static OperationDiagramView *gOperationDiagramView;

static GraphPoint gStartPosInfo;


OperationDiagramListener::~OperationDiagramListener()
    {}


void OperationDiagramView::drawToDrawingArea()
    {
    setCairoContext();
    CairoDrawer cairoDrawer(mOperationDiagram, mCairoContext.getCairo());
    cairoDrawer.clearAndSetDefaults();

    mOperationDiagram.drawDiagram(cairoDrawer);
    updateDrawingAreaSize();
    }

OovStatusReturn OperationDiagramView::drawSvgDiagram(File &file)
    {
    SvgDrawer svgDrawer(mOperationDiagram, file, mCairoContext.getCairo());
    mOperationDiagram.drawDiagram(svgDrawer);
    return svgDrawer.writeFile();
    }

void OperationDiagramView::updateDrawingAreaSize()
    {
    /// @todo - this is not efficient at the moment.
    GraphSize size = mOperationDiagram.getDrawingSize(mNullDrawer);
    gtk_widget_set_size_request(getDiagramWidget(), size.x, size.y);
    }

void OperationDiagramView::gotoClass(OovStringRef const className)
    {
    if(mListener)
        {
        mListener->gotoClass(className);
        }
    }

void OperationDiagramView::gotoOperation(OperationCall const *opcall)
    {
    OovString className = getDiagram().getNodeName(*opcall);
    if(mListener)
        {
        mListener->gotoOperation(className, opcall->getName(), opcall->isConst());
        }
//    clearGraphAndAddOperation(className, opcall->getName(), opcall->isConst());
//    requestRedraw();
    }

void OperationDiagramView::graphButtonPressEvent(const GdkEventButton *event)
    {
    gOperationDiagramView = this;
    gStartPosInfo.set(static_cast<int>(event->x), static_cast<int>(event->y));
    }

void OperationDiagramView::viewSource(OovStringRef const module,
        unsigned int lineNum)
    {
    ::viewSource(mGuiOptions, module, lineNum);
    }

static void displayContextMenu(guint button, guint32 acttime, gpointer data)
    {
    GdkEventButton *event = static_cast<GdkEventButton*>(data);
    const OperationNode *node = gOperationDiagramView->getDiagram().getNode(
        static_cast<int>(event->x), static_cast<int>(event->y));
    const OperationCall *opcall = gOperationDiagramView->getDiagram().getOperation(
            gStartPosInfo.x, gStartPosInfo.y);
    OovStringRef const nodeitems[] =
        {
        "OperGotoClassMenuitem",
        "RemoveOperClassMenuitem",
        "AddCallerVarRefsMenuitem"
        };
    Builder *builder = Builder::getBuilder();
    for(size_t i=0; i<sizeof(nodeitems)/sizeof(nodeitems[i]); i++)
        {
        gtk_widget_set_sensitive(builder->getWidget(nodeitems[i]),
                node != nullptr);
        }
    OovStringRef const operitems[] =
        {
        "OperGotoOperationMenuitem",
        "AddCallsMenuitem",
        "AddCallersMenuitem",
        "RemoveCallsMenuitem"
        };
    for(size_t i=0; i<sizeof(operitems)/sizeof(operitems[i]); i++)
        {
        gtk_widget_set_sensitive(builder->getWidget(operitems[i]),
                opcall != nullptr);
        }
    gtk_widget_set_sensitive(builder->getWidget("ViewOperSourceMenuitem"),
            opcall != nullptr || node != nullptr);

    GtkMenu *menu = builder->getMenu("DrawOperationPopupMenu");
    gtk_menu_popup(menu, nullptr, nullptr, nullptr, nullptr, button, acttime);
    gStartPosInfo.set(static_cast<int>(event->x), static_cast<int>(event->y));
    }

void OperationDiagramView::graphButtonReleaseEvent(const GdkEventButton *event)
    {
    if(event->button == 1)
        {
        /*
        OperationNode const *node = gOperationDiagramView->getDiagram().getNode(
                gStartPosInfo.x, gStartPosInfo.y);
        if(node)
            {
            DiagramPoint offset = gStartPosInfo.startPos;
            offset.sub(node->getPosition());

            DiagramPoint newPos = DiagramPoint(event->x, event->y);
            newPos.sub(offset);

            node->setPosition(newPos);
            gOperationDiagram->getOpGraph().drawDiagram();
            }
        */
        }
    else
        {
        displayContextMenu(event->button, event->time, (gpointer)event);
        }
    }

extern "C" G_MODULE_EXPORT void on_OperGotoOperationmenuitem_activate(
        GtkWidget * /*widget*/, gpointer /*data*/)
    {
    const OperationCall *opcall = gOperationDiagramView->getDiagram().getOperation(
            gStartPosInfo.x, gStartPosInfo.y);
    if(opcall)
        {
        gOperationDiagramView->gotoOperation(opcall);
        }
    }

extern "C" G_MODULE_EXPORT void on_OperGotoClassMenuitem_activate(
        GtkWidget * /*widget*/, gpointer /*data*/)
    {
    const OperationClass *node = gOperationDiagramView->getDiagram().getClass(
            gStartPosInfo.x, gStartPosInfo.y);
    if(node)
        {
        gOperationDiagramView->gotoClass(node->getType()->getName());
        }
    }

extern "C" G_MODULE_EXPORT void on_RemoveOperClassMenuitem_activate(
        GtkWidget * /*widget*/, gpointer /*data*/)
    {
    const OperationNode *node = gOperationDiagramView->getDiagram().getNode(
            gStartPosInfo.x, gStartPosInfo.y);
    if(node)
        {
        gOperationDiagramView->getDiagram().removeNode(node);
        gOperationDiagramView->requestRedraw();
        }
    }

extern "C" G_MODULE_EXPORT void on_AddCallsMenuitem_activate(
        GtkWidget * /*widget*/, gpointer /*data*/)
    {
    const OperationCall *opcall = gOperationDiagramView->getDiagram().getOperation(
            gStartPosInfo.x, gStartPosInfo.y);
    if(opcall)
        {
        gOperationDiagramView->getDiagram().addOperDefinition(*opcall);
        gOperationDiagramView->requestRedraw();
        }
    }

extern "C" G_MODULE_EXPORT void on_RemoveCallsMenuitem_activate(
        GtkWidget * /*widget*/, gpointer /*data*/)
    {
    const OperationCall *opcall = gOperationDiagramView->getDiagram().getOperation(
            gStartPosInfo.x, gStartPosInfo.y);
    if(opcall)
        {
        gOperationDiagramView->getDiagram().removeOperDefinition(*opcall);
        gOperationDiagramView->requestRedraw();
        }
    }

extern "C" G_MODULE_EXPORT void on_AddCallersMenuitem_activate(
        GtkWidget * /*widget*/, gpointer /*data*/)
    {
    const OperationCall *opcall = gOperationDiagramView->getDiagram().getOperation(
            gStartPosInfo.x, gStartPosInfo.y);
    if(opcall)
        {
        gOperationDiagramView->getDiagram().addOperCallers(*opcall);
        gOperationDiagramView->requestRedraw();
        }
    }

extern "C" G_MODULE_EXPORT void on_AddCallerVarRefsMenuitem_activate(
        GtkWidget * /*widget*/, gpointer /*data*/)
    {
    const OperationNode *node = gOperationDiagramView->getDiagram().getNode(
            gStartPosInfo.x, gStartPosInfo.y);
    if(node)
        {
        gOperationDiagramView->addVarRefs(node);
        }
    }

extern "C" G_MODULE_EXPORT void on_OperationFontMenuitem_activate(
    GtkWidget * /*widget*/, gpointer /*data*/)
    {
    int fontSize = gOperationDiagramView->getFontSize();
    if(setFontDialog(fontSize))
        {
        gOperationDiagramView->setFontSize(fontSize);
        }
    }

extern "C" G_MODULE_EXPORT void on_ViewOperSourceMenuitem_activate(
    GtkWidget * /*widget*/, gpointer /*data*/)
    {
    const OperationCall *opcall = gOperationDiagramView->getDiagram().getOperation(
            gStartPosInfo.x, gStartPosInfo.y);
    if(opcall)
        {
        const ModelOperation &oper = opcall->getOperation();
        if(oper.getModule())
            {
            gOperationDiagramView->viewSource(oper.getModule()->getModulePath(),
                    oper.getLineNum());
            }
        }
    const OperationClass *node = gOperationDiagramView->getDiagram().getClass(
            gStartPosInfo.x, gStartPosInfo.y);
    if(node)
        {
        const ModelClassifier *cls = ModelClassifier::getClass(node->getType());
        if(cls->getModule())
            {
            gOperationDiagramView->viewSource(cls->getModule()->getModulePath(),
                    cls->getLineNum());
            }
        }
    }

extern "C" G_MODULE_EXPORT void on_RestartOperationsMenuitem_activate(
        GtkWidget * /*widget*/, gpointer /*data*/)
    {
    gOperationDiagramView->restart();
    }

extern "C" G_MODULE_EXPORT void on_OperRemoveAllMenuitem_activate(
        GtkWidget * /*widget*/, gpointer /*data*/)
    {
    gOperationDiagramView->getDiagram().clearGraph();
    gOperationDiagramView->requestRedraw();
    }
