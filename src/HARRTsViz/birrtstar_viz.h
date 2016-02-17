#ifndef BIRRTSTAR_VIZ_H_
#define BIRRTSTAR_VIZ_H_

#include <vector>
#include <QLabel>
#include "birrtstar.h"
#include "string_class_mgr.h"
#include "birrtstar_path_planning_info.h"
#include "reference_frames.h"

namespace birrts {

  typedef enum{
    NONE_TREE_SHOW,
    START_TREE_SHOW,
    GOAL_TREE_SHOW,
    BOTH_TREES_SHOW
  } tree_show_type_t;

  enum BIRRTstarVizMode {
    SUBREGION,
    LINE_SUBSEGMENT,
  };

  class BIRRTstarViz : public QLabel
  {
  public:
    explicit BIRRTstarViz(QWidget *parent = 0);
    void set_tree(BIRRTstar* p_tree);

    bool m_show_subsegment;
    bool m_show_string_class_reference_path;
    void prev_region();
    void next_region();

    void prev_subregion();
    void next_subregion();

    int  get_region_idx()    { return m_region_idx; }
    int  get_subregion_idx() { return m_subregion_idx; }

    h2p::SubRegionSet* get_selected_region();
    h2p::SubRegion* get_selected_subregion();

    void prev_line_subsegment_set();
    void next_line_subsegment_set();

    void prev_line_subsegment();
    void next_line_subsegment();

    void prev_string_class();
    void next_string_class();

    int  get_line_subsegment_set_idx() { return m_subsegment_set_idx; }
    int  get_line_subsegment_idx() { return m_subsegment_idx; }

    h2p::LineSubSegmentSet* get_selected_line_subsegment_set();
    h2p::LineSubSegment*    get_selected_line_subsegment();
    StringClass*            get_selected_string_class();

    BIRRTstarVizMode get_mode() { return m_mode; }
    void set_mode( BIRRTstarVizMode mode );

    bool save( QString filename );
    virtual bool load( QString filename );

    virtual bool load_map( QString filename );
    void process_world();

    bool is_selected_obstacle( h2p::Obstacle* p_obstacle );
    bool unselect_obstacle( h2p::Obstacle* p_obstacle );

    std::vector<h2p::Obstacle*> get_selected_obstacles() { return m_selected_obstacles; }
    void clear_selected_obstacles() { m_selected_obstacles.clear(); }

    bool draw_path(QString filename);
    bool save_current_viz(QString filename);

    void set_finished_planning( bool finished ) { m_finished_planning = finished; }
    bool is_finished_planning() { return m_finished_planning; }

    tree_show_type_t get_tree_show_type() { return m_tree_show_type; }
    void switch_tree_show_type();

    StringClassMgr* get_string_class_mgr();

    h2p::ReferenceFrameSet* get_reference_frame_set() {  return mp_reference_frame_set; }
    
    BIRRTstarPathPlanningInfo m_PPInfo;
    int get_width() { return m_world_width; }
    int get_height() { return m_world_height; }
  signals:
    
  public slots:

  protected:
    virtual bool init_world(QString filename);

    void update_viz_subregions();
    void update_viz_line_subsegments();
    void update_viz_string_class();
    void update_viz_reference_frames();

    void mousePressEvent( QMouseEvent * event );

    void draw_path_on_map(QPixmap& map);
    void draw_current_viz(QPixmap& map);

    void paint(QPaintDevice * device);

    h2p::WorldMap* get_world_map();

    StringClassMgr* mp_mgr;
    h2p::ReferenceFrameSet*  mp_reference_frame_set;
    int                  m_world_width;
    int                  m_world_height;
    BIRRTstarVizMode     m_mode;

    BIRRTstar*              mp_tree;
    bool                m_show_reference_frames;
    bool                m_show_regions;
    bool                m_finished_planning;
    tree_show_type_t    m_tree_show_type;

    int                  m_region_idx;
    int                  m_subregion_idx;
    int                  m_subsegment_set_idx;
    int                  m_subsegment_idx;
    int                  m_string_class_idx;

    std::vector<h2p::SubRegion*>      m_viz_subregions;
    std::vector<h2p::LineSubSegment*> m_viz_subsegments;

    std::vector<h2p::Obstacle*>       m_selected_obstacles;
    StringClass*                      mp_viz_string_class;

    std::vector<h2p::ReferenceFrame*> m_viz_pos_refs;
    std::vector<h2p::ReferenceFrame*> m_viz_neg_refs;

  protected slots:
    void paintEvent(QPaintEvent * e);
  };

}

#endif // BIRRTSTAR_VIZ_H_
