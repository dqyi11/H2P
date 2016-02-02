#ifndef SPATIAL_INFER_CONFIG_H
#define SPATIAL_INFER_CONFIG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>

namespace h2p {
  
  class SpatialRelationsWindow;

  class SpatialRelationsConfig : public QDialog {
    Q_OBJECT
  public:
    SpatialRelationsConfig( SpatialRelationsWindow* parent );
    void updateDisplay();   
  private:
    QListWidget* mpListWidget;
    QPushButton* mpBtnRemove;
    QPushButton* mpBtnOK;
  
    QLabel* mpLabelSpatialRelations;
  
    SpatialRelationsWindow* mpParentWindow;

  public slots:
    void onBtnOKClicked();
    void onBtnRemoveClicked();
 
  };


}

#endif // SPATIAL_INFER_CONFIG_H
