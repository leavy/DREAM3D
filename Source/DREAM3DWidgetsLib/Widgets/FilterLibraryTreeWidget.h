/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2012 Joseph B. Kleingers (Intern, University of Dayton)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _TREEWIDGETBUILDER_H_
#define _TREEWIDGETBUILDER_H_

#include <QApplication>


#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLib.h"


class PipelineBuilderWidget;
class QAction;
class QTreeWidgetItem;

class DREAM3DWidgetsLib_EXPORT FilterLibraryTreeWidget : public QTreeWidget
{
    Q_OBJECT

  public:
    enum ItemType
    {
      Node_Item_Type = 1,
      Leaf_Item_Type = 2,
      Unknown_Item_Type = 3
    };

    /**
     * @brief FilterLibraryTreeWidget
     * @param parent
     */
    FilterLibraryTreeWidget(QWidget* parent = 0);


    /**
     * @brief setNodeActionList
     * @param list
     */
    void setNodeActionList(QList<QAction*> list);

    /**
     * @brief setLeafActionList
     * @param list
     */
    void setLeafActionList(QList<QAction*> list);

  protected:


    /**
     * @brief Adds the actions in the actionList parameter to the right-click menu
     */
    void addActionList(QList<QAction*> actionList);

  private slots:

    /**
     * @brief onCustomContextMenuRequested
     * @param pos
     */
    void onCustomContextMenuRequested(const QPoint& pos);

    /**
     * @brief showContextMenu
     * @param item
     * @param globalPos
     */
    void showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos);

  private:
    QMenu                     m_Menu;
    QList<QAction*>           m_NodeActions;
    QList<QAction*>           m_LeafActions;
};

#endif /* _TREEWIDGETBUILDER_H_ */
