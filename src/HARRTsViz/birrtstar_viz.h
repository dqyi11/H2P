#ifndef BIRRTSTAR_VIZ_H_
#define BIRRTSTAR_VIZ_H_

#include "spatial_relations_viz.h"
#include "birrtstar.h"
#include "birrtstar_path_planning_info.h"
#include "reference_frames.h"

namespace birrts {

  typedef enum{
    NONE_TREE_SHOW,
    START_TREE_SHOW,
    GOAL_TREE_SHOW,
    BOTH_TREES_SHOW
  } tree_show_type_t;

  class BIRRTstarViz : public h2p::SpatialRelationsViz
  {
  public:
    explicit BIRRTstarViz(QWidget *parent = 0);
    void set_tree(BIRRTstar* p_tree);

    bool draw_path(QString filename);
    bool save_current_viz(QString filename);

    void set_finished_planning( bool finished ) { m_finished_planning = finished; }
    bool get_finished_planning() { return m_finished_planning; }

    bool is_finished_planning() { return m_finished_planning; }
 
    QString generate_string();

    tree_show_type_t get_tree_show_type() { return m_tree_show_type; }
    void switch_tree_show_type();
    
    BIRRTstarPathPlanningInfo m_PPInfo;
    
  signals:
    
  public slots:

  protected:

    void draw_path_on_map(QPixmap& map);
    void draw_current_viz(QPixmap& map);

    void paint(QPaintDevice * device);
    BIRRTstar*              mp_tree;
    h2p::ReferenceFrameSet* mp_reference_frames;
    bool                m_show_reference_frames;
    bool                m_show_regions;
    bool                m_finished_planning;
    int                 m_reference_frame_index;
    int                 m_found_path_index;
    int                 m_region_index;
    int                 m_subregion_index;
    std::vector<QColor> m_colors;
    tree_show_type_t    m_tree_show_type;

  protected slots:
    void paintEvent(QPaintEvent * e);
  };

}

#endif // BIRRTSTAR_VIZ_H_
