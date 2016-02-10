#ifndef SPATIALINFER_VIZ_H
#define SPATIALINFER_VIZ_H

#include <vector>
#include <QLabel>
#include "spatial_relation_mgr.h"
#include "reference_frames.h"

namespace h2p {
  
  enum SpatialRelationsVizMode {
    SUBREGION,
    LINE_SUBSEGMENT,
  };

  class SpatialRelationsViz : public QLabel {
    Q_OBJECT
  public:
    explicit SpatialRelationsViz( QWidget *parent = 0 );
    bool loadMap( QString filename );

    bool mShowSubsegment;
    void prevRegion();
    void nextRegion();

    void prevSubregion();
    void nextSubregion();

    int  getRegionIdx()    { return mRegionIdx; }
    int  getSubregionIdx() { return mSubRegionIdx; }

    SubRegionSet* getSelectedRegion();
    SubRegion* getSelectedSubregion();

    void prevLineSubsegmentSet();
    void nextLineSubsegmentSet();

    void prevLineSubsegment();
    void nextLineSubsegment();

    void prevStringClass();
    void nextStringClass();
    
    int  getLineSubsegmentSetIdx() { return mSubsegmentSetIdx; }
    int  getLineSubsegmentIdx() { return mSubsegmentIdx; } 

    LineSubSegmentSet* getSelectedLineSubsegmentSet();
    LineSubSegment*    getSelectedLineSubsegment();
    StringClass*       getSelectedStringClass();  

    bool save( QString filename );
    bool load( QString filename );
        
    void setMode( SpatialRelationsVizMode mode );
    SpatialRelationsVizMode getMode() { return mMode; }
    
    SpatialRelationMgr* get_spatial_relation_mgr() { return mpMgr; }
    bool is_selected_obstacle( Obstacle* p_obstacle );
    bool unselect_obstacle( Obstacle* p_obstacle );

    std::vector<Obstacle*> get_selected_obstacles() { return m_selected_obstacles; }
    void clear_selected_obstacles() { m_selected_obstacles.clear(); }  

    ReferenceFrameSet* get_reference_frame_set() {  return mpReferenceFrameSet; }
    void processWorld();
  protected:
    bool initWorld(QString filename);

    void updateVizSubregions();
    void updateVizLineSubsegments();
    void updateVizStringClass();
    void updateVizReferenceFrames();

    void mousePressEvent( QMouseEvent * event );

    SpatialRelationMgr* mpMgr;
    ReferenceFrameSet*  mpReferenceFrameSet;
    int                  mWorldWidth;
    int                  mWorldHeight;
    
    SpatialRelationsVizMode  mMode;

    int                  mRegionIdx;
    int                  mSubRegionIdx;
    
    int                  mSubsegmentSetIdx;
    int                  mSubsegmentIdx;

    int                  mStringClassIdx;
 
    std::vector<SubRegion*>      m_viz_subregions;
    std::vector<LineSubSegment*> m_viz_subsegments;

    std::vector<Obstacle*>       m_selected_obstacles;   
    StringClass*                 mp_viz_string_class;
 
    std::vector<ReferenceFrame*> m_viz_pos_refs;
    std::vector<ReferenceFrame*> m_viz_neg_refs;

  signals:

  public slots:

  private slots:
    void paintEvent(QPaintEvent * e);
  };

}
#endif // SPATIALINFER_VIZ_H
