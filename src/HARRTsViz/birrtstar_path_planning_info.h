#ifndef BIRRTSTAR_PATH_PLANNING_INFO_H_
#define BIRRTSTAR_PATH_PLANNING_INFO_H_

#include <libxml/tree.h>
#include <QString>
#include <QPoint>
#include <list>
#include <vector>
#include <QDebug>
#include <math.h>
#include "spatial_relation_mgr.h"
#include "birrtstar.h"

namespace birrts {

  class SpatialRelationInfo {
  public:
    SpatialRelationInfo();
    virtual ~SpatialRelationInfo();
    std::string              m_type;
    std::vector<std::string> m_obstacles;
    SpatialRelationInfo*     mp_child_info;
  };

  typedef struct {
    std::string name;
    int center_x;
    int center_y;
  } ObsInfo;

  class BIRRTstarPathPlanningInfo {
  public:
    BIRRTstarPathPlanningInfo();
    virtual ~BIRRTstarPathPlanningInfo();

    bool get_obstacle_info( int** pp_obstacle_info );
    bool get_cost_distribution( double** pp_cost_distribution );

    bool get_pix_info( QString filename, double** pp_pix_info );
    bool get_pix_info( QString filename, int** pp_pix_info );
    void init_func_param();

    void dump_cost_distribution( QString filename );

    bool save_to_file( QString filename );
    bool load_from_file( QString filename );

    void read( xmlNodePtr root );
    void write( xmlDocPtr doc, xmlNodePtr root ) const;

    SpatialRelationInfo* load_spatial_relation_info( xmlNodePtr node );
    xmlNodePtr           save_spatial_relation_info( xmlDocPtr doc, SpatialRelationInfo* p_info ) const;

    void load_paths( std::vector<Path*> paths );
    bool export_paths( QString filename );

    static SpatialRelationInfo* spatial_relation_func_to_info( h2p::SpatialRelationFunction* p_func );
    static h2p::SpatialRelationFunction* spatial_relation_info_to_func( SpatialRelationInfo* p_info, h2p::SpatialRelationMgr* p_mgr );

    static double calc_dist( POS2D pos_a, POS2D pos_b, double** pp_distribution, void* tree ) {
      double dist = 0.0;
      if (pos_a == pos_b) {
        return dist;
      }
      double delta_x = fabs(pos_a[0]-pos_b[0]);
      double delta_y = fabs(pos_a[1]-pos_b[1]);
      dist = sqrt(delta_x*delta_x+delta_y*delta_y);

      if(dist < 0.0) {
        qWarning() << "Dist negative " << dist ;
      }
      return dist;
    }

    static double calc_cost( POS2D pos_a, POS2D pos_b, double** pp_distribution, void* tree ) {
      double cost = 0.0;
      BIRRTstar* rrts = (BIRRTstar*)tree;
      if ( pos_a == pos_b ) {
        return cost;
      }
      if( pp_distribution == NULL ) {
        return cost;
      }

      float x1 = pos_a[0];
      float y1 = pos_a[1];
      float x2 = pos_b[0];
      float y2 = pos_b[1];

      const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
      if (steep) {
        std::swap(x1, y1);
        std::swap(x2, y2);
      }

      if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
      }

      const float dx = x2 - x1;
      const float dy = fabs(y2 - y1);

      float error = dx / 2.0f;
      const int ystep = (y1 < y2) ? 1 : -1;
      int y = (int)y1;

      const int maxX = (int)x2;

      for(int x=(int)x1; x<maxX; x++) {
        if(steep) {
          if (y>=0 && y<rrts->get_sampling_width() && x>=0 && x<rrts->get_sampling_height()) {
            cost += pp_distribution[y][x];
          }
        }
        else {
          if (x>=0 && x<rrts->get_sampling_width() && y>=0 && y<rrts->get_sampling_height()) {
            cost += pp_distribution[x][y];
          }
        }

        error -= dy;
        if(error < 0) {
          y += ystep;
          error += dx;
        }
      }
      return cost;
    }

    /* Member variables */
    QString m_info_filename;
    QString m_map_filename;
    QString m_map_fullpath;
    int m_map_width;
    int m_map_height;

    QPoint m_start;
    QPoint m_goal;

    QString m_paths_output;
    bool m_min_dist_enabled;
    QString m_objective_file;

    COST_FUNC_PTR mp_func;
    double** mCostDistribution; 

    h2p::grammar_type_t m_grammar_type;
    RRTree_run_type_t m_run_type;

    int m_max_iteration_num;
    double m_segment_length;

    std::vector< SpatialRelationInfo* > mp_spatial_rel_info_list;
    std::vector< ObsInfo >              m_obs_info_list;
    std::vector< Path* >                mp_found_paths;
  };

}

#endif //  BIRRTSTAR_PATH_PLANNING_INFO_H_
