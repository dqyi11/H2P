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
    bool load_map( QString filename );

    bool m_show_subsegment;
    bool m_show_string_class_reference_path;
    void prev_region();
    void next_region();

    void prev_subregion();
    void next_subregion();

    int  get_region_idx()    { return m_region_idx; }
    int  get_subregion_idx() { return m_subregion_idx; }

    SubRegionSet* get_selected_region();
    SubRegion* get_selected_subregion();

    void prev_line_subsegment_set();
    void next_line_subsegment_set();

    void prev_line_subsegment();
    void next_line_subsegment();

    void prev_string_class();
    void next_string_class();
    
    int  get_line_subsegment_set_idx() { return m_subsegment_set_idx; }
    int  get_line_subsegment_idx() { return m_subsegment_idx; }

    LineSubSegmentSet* get_selected_line_subsegment_set();
    LineSubSegment*    get_selected_line_subsegment();
    StringClass*       get_selected_string_class();

    bool save( QString filename );
    bool load( QString filename );
        
    void set_mode( SpatialRelationsVizMode mode );
    SpatialRelationsVizMode get_mode() { return m_mode; }
    
    SpatialRelationMgr* get_spatial_relation_mgr() { return mp_mgr; }
    bool is_selected_obstacle( Obstacle* p_obstacle );
    bool unselect_obstacle( Obstacle* p_obstacle );

    std::vector<Obstacle*> get_selected_obstacles() { return m_selected_obstacles; }
    void clear_selected_obstacles() { m_selected_obstacles.clear(); }  

    ReferenceFrameSet* get_reference_frame_set() {  return mp_reference_frame_set; }
    void process_world();

    int get_width() { return m_world_width; }
    int get_height() { return m_world_height; }
  protected:
    bool init_world(QString filename);

    void update_viz_subregions();
    void update_viz_line_subsegments();
    void update_viz_string_class();
    void update_viz_reference_frames();

    void mousePressEvent( QMouseEvent * event );
    void paint( QPaintDevice * device );

    SpatialRelationMgr* mp_mgr;
    ReferenceFrameSet*  mp_reference_frame_set;
    int                  m_world_width;
    int                  m_world_height;
    
    SpatialRelationsVizMode  m_mode;

    int                  m_region_idx;
    int                  m_subregion_idx;
    
    int                  m_subsegment_set_idx;
    int                  m_subsegment_idx;

    int                  m_string_class_idx;
 
    std::vector<SubRegion*>      m_viz_subregions;
    std::vector<LineSubSegment*> m_viz_subsegments;

    std::vector<Obstacle*>       m_selected_obstacles;   
    StringClass*                 mp_viz_string_class;
 
    std::vector<ReferenceFrame*> m_viz_pos_refs;
    std::vector<ReferenceFrame*> m_viz_neg_refs;

  signals:

  public slots:

  protected slots:
    void paintEvent(QPaintEvent * e);
  };

}
#endif // SPATIALINFER_VIZ_H
