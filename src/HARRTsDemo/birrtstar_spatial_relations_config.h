#ifndef BIRRTSTAR_SPATIAL_INFER_CONFIG_H
#define BIRRTSTAR_SPATIAL_INFER_CONFIG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>

namespace birrts {
  
  class BIRRTstarWindow;

  class BIRRTstarSpatialRelationsConfig : public QDialog {
    Q_OBJECT
  public:
    BIRRTstarSpatialRelationsConfig( BIRRTstarWindow* parent );
    void updateDisplay();   
  private:
    QListWidget* mpListWidget;
    QPushButton* mpBtnRemove;
    QPushButton* mpBtnOK;
  
    QLabel* mpLabelSpatialRelations;
  
    BIRRTstarWindow* mpParentWindow;

  public slots:
    void onBtnOKClicked();
    void onBtnRemoveClicked();
 
  };


}

#endif // BIRRTSTAR_SPATIAL_INFER_CONFIG_H
