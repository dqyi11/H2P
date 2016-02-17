#include <QVBoxLayout>
#include <QHBoxLayout>
#include "spatial_relations_window.h"
#include "spatial_relations_config.h"

using namespace std;
using namespace h2p;

SpatialRelationsConfig::SpatialRelationsConfig( SpatialRelationsWindow* parent ) {
  mpParentWindow = parent;

  mpLabelSpatialRelations = new QLabel("Spatial Relation Functions");
  
  mpListWidget = new QListWidget();
  mpListWidget->setViewMode(QListView::IconMode);
  mpListWidget->show();

  mpBtnRemove = new QPushButton(tr("Remove"));
  mpBtnOK = new QPushButton(tr("OK"));
  QHBoxLayout* buttonsLayout = new QHBoxLayout();
  buttonsLayout->addWidget( mpBtnRemove );
  buttonsLayout->addWidget( mpBtnOK );
  
  connect(mpBtnRemove, SIGNAL(clicked()), this, SLOT(onBtnRemoveClicked()) );
  connect(mpBtnOK, SIGNAL(clicked()), this, SLOT(onBtnOKClicked()) );

  QVBoxLayout* mainLayout = new QVBoxLayout();
  mainLayout->addWidget( mpLabelSpatialRelations );
  mainLayout->addWidget( mpListWidget );
  mainLayout->addLayout( buttonsLayout );

  setLayout( mainLayout );
}

void SpatialRelationsConfig::onBtnOKClicked() {
  close();
}
 
void SpatialRelationsConfig::onBtnRemoveClicked() {
  QList<QListWidgetItem*> selected_items = mpListWidget->selectedItems();
  QList<QListWidgetItem*>::const_iterator iter;
  for( iter = selected_items.begin(); iter != selected_items.end(); iter++ ) {
    QListWidgetItem* p_item = (*iter);
    if( p_item ) {
      if( mpParentWindow ) {
        if( mpParentWindow->mpViz ) {
          if( mpParentWindow->mpViz->get_spatial_relation_mgr() ) {
            SpatialRelationStringClassMgr* p_cls_mgr = dynamic_cast<SpatialRelationStringClassMgr*>( mpParentWindow->mpViz->get_spatial_relation_mgr() );
            if( p_cls_mgr ) {
              p_cls_mgr->remove_spatial_relation_function( p_item->text().toStdString() );
            }
          }
        }
      }
    } 
  }
  qDeleteAll( selected_items );
}

void SpatialRelationsConfig::updateDisplay() {
  mpListWidget->clear();
  if( mpParentWindow ) {
    if( mpParentWindow->mpViz ) {
      if( mpParentWindow->mpViz->get_spatial_relation_mgr() ) {
        SpatialRelationStringClassMgr* p_cls_mgr = dynamic_cast<SpatialRelationStringClassMgr*>( mpParentWindow->mpViz->get_spatial_relation_mgr() );
        if( p_cls_mgr ) {
          vector< string > names = p_cls_mgr->get_spatial_relation_function_names();
          for( unsigned int i = 0; i < names.size(); i++ ) {
            mpListWidget->addItem( QString::fromStdString( names[i] ) );
          }
        }
      }
    }
  }
  repaint();
}
