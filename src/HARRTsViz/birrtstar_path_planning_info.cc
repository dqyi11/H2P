#include <sstream>
#include <cstdlib>
#include <list>
#include <QPixmap>
#include <QFile>

#include "inbetween_relation_function.h"
#include "avoid_relation_function.h"
#include "sideof_relation_function.h"
#include "birrtstar_path_planning_info.h"

using namespace std;
using namespace h2p;
using namespace birrts;

SpatialRelationInfo::SpatialRelationInfo() {
  m_type = "";
  m_obstacles.clear();
  mp_child_info = NULL;
}

SpatialRelationInfo::~SpatialRelationInfo() {
  m_type = "";
  m_obstacles.clear();
  mp_child_info = NULL;
}

BIRRTstarPathPlanningInfo::BIRRTstarPathPlanningInfo() {
  m_info_filename = "";
  m_map_filename = "";
  m_map_fullpath = "";
  m_objective_file = "";
  m_start.setX(-1);
  m_start.setY(-1);
  m_goal.setX(-1);
  m_goal.setY(-1);

  m_paths_output = "";
  mp_found_paths.clear();
     
  m_grammar_type = STRING_GRAMMAR_TYPE;
  m_run_type = RUN_BOTH_TREES_TYPE;
   
  m_min_dist_enabled = true;

  m_max_iteration_num = 3000;
  m_segment_length = 5.0;
  mCostDistribution = NULL;

  m_map_width = 0;
  m_map_height = 0;
}

BIRRTstarPathPlanningInfo::~BIRRTstarPathPlanningInfo() {

  for( unsigned int i=0; i<mp_spatial_rel_info_list.size(); i++ ) {
    SpatialRelationInfo* p_info = mp_spatial_rel_info_list[i];
    delete p_info;
    p_info = NULL;
  }
  for( unsigned int i=0; i<mp_found_paths.size(); i++ ) {
    Path* p_path = mp_found_paths[i];
    delete p_path;
    p_path = NULL;
  }
  m_obs_info_list.clear();
  mp_found_paths.clear();
  mp_spatial_rel_info_list.clear();
}

bool BIRRTstarPathPlanningInfo::get_obstacle_info( int** pp_obstacle_info ) {
  if( pp_obstacle_info==NULL ) {
    return false;
  }
  return get_pix_info( m_map_fullpath, pp_obstacle_info );
}

bool BIRRTstarPathPlanningInfo::get_cost_distribution( double** pp_cost_distribution ) {
  return get_pix_info( m_objective_file, pp_cost_distribution );
}

bool BIRRTstarPathPlanningInfo::get_pix_info( QString filename, double** pp_pix_info ) {
  if( pp_pix_info==NULL ) {
    return false;
  }
  QPixmap map(filename);
  QImage gray_img = map.toImage();
  int width = map.width();
  int height = map.height();

  for(int i=0;i<width;i++) {
    for(int j=0;j<height;j++) {
      QRgb col = gray_img.pixel(i,j);
      int g_val = qGray(col);
      if( g_val < 0 || g_val > 255 ) {
        qWarning() << "gray value out of range";
      }
      pp_pix_info[i][j] = (double)g_val/255.0;
    }
  }
  return true;
}

bool BIRRTstarPathPlanningInfo::get_pix_info(QString filename, int ** pp_pix_info) {
  if( pp_pix_info==NULL ) {
    return false;
  }
  QPixmap map(filename);
  QImage gray_img = map.toImage();
  int width = map.width();
  int height = map.height();

  for(int i=0;i<width;i++) {
    for(int j=0;j<height;j++) {
      QRgb col = gray_img.pixel(i,j);
      int g_val = qGray(col);
      if( g_val < 0 || g_val > 255 ) {
        qWarning() << "gray value out of range";
      }
      pp_pix_info[i][j] = g_val;
    }
  }
  return true;
}


void BIRRTstarPathPlanningInfo::init_func_param() {
  if( m_min_dist_enabled == true ) {
    mp_func = BIRRTstarPathPlanningInfo::calc_dist;
    mCostDistribution = NULL;
  }
  else {
    mp_func = BIRRTstarPathPlanningInfo::calc_cost;
    if(mCostDistribution) {
      delete[] mCostDistribution;
      mCostDistribution = NULL;
    }
    mCostDistribution = new double*[m_map_width];
    for(int i=0;i<m_map_width;i++) {
      mCostDistribution[i] = new double[m_map_height];
    }
    get_cost_distribution( mCostDistribution );
  }
}

void BIRRTstarPathPlanningInfo::read( xmlNodePtr root ) {
  if( root->type == XML_ELEMENT_NODE ){
    xmlChar* tmp = xmlGetProp( root, ( const xmlChar* )( "map_filename" ) );
    if ( tmp != NULL ) {
      string map_filename = ( char * )( tmp ); 
      m_map_filename = QString::fromStdString( map_filename );
      xmlFree( tmp );
    } 
    tmp = xmlGetProp( root, ( const xmlChar* )( "map_fullpath" ) );
    if ( tmp != NULL ) {
      string map_fullpath = ( char * )( tmp );
      m_map_fullpath = QString::fromStdString( map_fullpath ); 
      xmlFree( tmp );
    }
    tmp = xmlGetProp( root, ( const xmlChar* )( "map_width" ) );
    if ( tmp != NULL ) {
      string map_width = ( char * )( tmp );
      m_map_width = strtol( map_width.c_str(), NULL, 10 );
    }
    tmp = xmlGetProp( root, ( const xmlChar* )( "height_width" ) );
    if ( tmp != NULL ) {
      string map_height = ( char * )( tmp );
      m_map_height = strtol( map_height.c_str(), NULL, 10 );
    }
    int start_x_int = 0, start_y_int = 0;
    tmp = xmlGetProp( root, ( const xmlChar* )( "start_x" ) );
    if ( tmp != NULL ) {
      string start_x = ( char * )( tmp );
      start_x_int = strtol( start_x.c_str(), NULL, 10 );
    }
    tmp = xmlGetProp( root, ( const xmlChar* )( "start_y" ) );
    if ( tmp != NULL ) {
      string start_y = ( char * )( tmp );
      start_y_int = strtol( start_y.c_str(), NULL, 10 );
    }
    m_start = QPoint( start_x_int, start_y_int );
    int goal_x_int = 0, goal_y_int = 0;
    tmp = xmlGetProp( root, ( const xmlChar* )( "goal_x" ) );
    if ( tmp != NULL ) {
      string goal_x = ( char * )( tmp );
      goal_x_int = strtol( goal_x.c_str(), NULL, 10 );
    }
    tmp = xmlGetProp( root, ( const xmlChar* )( "goal_y" ) );
    if ( tmp != NULL ) {
      string goal_y = ( char * )( tmp );
      goal_y_int = strtol( goal_y.c_str(), NULL, 10 );
    }
    m_goal = QPoint( goal_x_int, goal_y_int );
    tmp = xmlGetProp( root, ( const xmlChar* )( "grammar_type" ) );
    if ( tmp != NULL ) {
      string grammar_type_str = ( char * )( tmp );
      m_grammar_type = static_cast<grammar_type_t>( strtol( grammar_type_str.c_str(), NULL, 10 ) );
    }
    tmp = xmlGetProp( root, ( const xmlChar* )( "run_type" ) );
    if ( tmp != NULL ) {
      string run_type_str = ( char * )( tmp );
      m_run_type = static_cast<RRTree_run_type_t>( strtol( run_type_str.c_str(), NULL, 10 ) );
    }
    tmp = xmlGetProp( root, ( const xmlChar* )( "min_dist_enabled" ) );
    if ( tmp != NULL ) {
      string min_dist_enabled = ( char * )( tmp );
      int min_dist_enabled_int = strtol( min_dist_enabled.c_str(), NULL, 10 );
      if (min_dist_enabled_int > 0) {
        m_min_dist_enabled = true;
      } else {
        m_min_dist_enabled = false;
      }
    }
    tmp = xmlGetProp( root, ( const xmlChar* )( "objective_file" ) );
    if ( tmp != NULL ) {
      string objective_file = ( char * )( tmp );
      m_objective_file = QString::fromStdString( objective_file );
    }
    tmp = xmlGetProp( root, ( const xmlChar* )( "path_output_file" ) );
    if ( tmp != NULL ) {
      string paths_output = ( char * )( tmp );
      m_paths_output = QString::fromStdString( paths_output );
    }
    tmp = xmlGetProp( root, ( const xmlChar* )( "max_iteration_num" ) );
    if ( tmp != NULL ) {
      string max_iter_num = ( char * )( tmp );
      m_max_iteration_num = strtol( max_iter_num.c_str(), NULL, 10);
    }
    tmp = xmlGetProp( root, ( const xmlChar* )( "segment_length" ) );
    if ( tmp != NULL ) {
      string seg_len = ( char * )( tmp );
      m_segment_length = strtof( seg_len.c_str(), NULL );
    }

    xmlNodePtr l1 = NULL;
    for( l1 = root->children; l1; l1 = l1->next ) {
      if( l1->type == XML_ELEMENT_NODE ) {
        if( xmlStrcmp( l1->name, ( const xmlChar* )( "obstacles" ) )  == 0 ) {
          xmlNodePtr l2 = NULL;
          for( l2 = l1->children; l2; l2 = l2->next ) {
            if( l2->type == XML_ELEMENT_NODE ) {
              if( xmlStrcmp( l2->name, ( const xmlChar* )( "obstacle" ) )  == 0 ) {
                ObsInfo obs_info;
                xmlChar* tmp_obs_name = xmlGetProp( l2, ( const xmlChar* )( "name" ) );
                if( tmp_obs_name != NULL ) {
                  string obs_name_string =  ( char* )( tmp_obs_name ) ;
                  obs_info.name =  obs_name_string;
                }
                xmlChar* tmp_center_x = xmlGetProp( l2, ( const xmlChar* )( "center_x" ) );
                if( tmp_center_x != NULL ) {
                  string obs_center_x_string = (char*)( tmp_center_x );
                  obs_info.center_x = strtol( obs_center_x_string.c_str(), NULL, 10 );
                }
                xmlChar* tmp_center_y = xmlGetProp( l2, ( const xmlChar* )( "center_y" ) );
                if( tmp_center_y != NULL ) {
                  string obs_center_y_string = (char*)( tmp_center_y );
                  obs_info.center_y = strtol( obs_center_y_string.c_str(), NULL, 10 );
                }
                m_obs_info_list.push_back( obs_info );
              }
            }
          }
        }
        else if( xmlStrcmp( l1->name, ( const xmlChar* )( "spatial_relations" ) ) == 0 ) {
          xmlNodePtr l2 = NULL;
          for( l2 = l1->children; l2; l2 = l2->next ) {
            if( l2->type == XML_ELEMENT_NODE ) {
              if( xmlStrcmp( l2->name, ( const xmlChar* )( "spatial_relation" ) )  == 0 ) {
                SpatialRelationInfo*  p_spatial_rel_info;
                xmlChar* tmp_type = xmlGetProp( l2, ( const xmlChar* )( "type" ) );
                if( tmp_type != NULL ) {
                  string type_string = (char*)( tmp_type );
                  p_spatial_rel_info->m_type = type_string;
                }
                xmlNodePtr l3 = NULL;
                for( l3 = l2->children; l3; l3 = l3->next ) {
                  if( l3->type == XML_ELEMENT_NODE ) {
                    if( xmlStrcmp( l3->name, ( const xmlChar* )( "obstacle" ) )  == 0 ) {
                      xmlChar* tmp_obs_name = xmlGetProp( l3, ( const xmlChar* )( "name" ) );
                      if( tmp_obs_name != NULL ) {
                        string obs_name_string = (char*)( tmp_obs_name );
                        p_spatial_rel_info->m_obstacles.push_back( obs_name_string );
                      }
                    }
                  }
                }
                mp_spatial_rel_info_list.push_back( p_spatial_rel_info );
              }
            }
          }
        }
      }
    }
  }

}

void BIRRTstarPathPlanningInfo::write( xmlDocPtr doc, xmlNodePtr root ) const {
    
  xmlNodePtr node = xmlNewDocNode( doc, NULL, ( const xmlChar* )( "world" ), NULL );
  xmlNewProp( node, ( const xmlChar* )( "map_filename" ), ( const xmlChar* )( m_map_filename.toStdString().c_str() ) );
  xmlNewProp( node, ( const xmlChar* )( "map_fullpath" ), ( const xmlChar* )( m_map_fullpath.toStdString().c_str() ) );
  stringstream width_str;
  width_str << m_map_width;
  xmlNewProp( node, ( const xmlChar* )( "map_width" ), ( const xmlChar* )( width_str.str().c_str() ) );
  stringstream height_str;
  height_str << m_map_height;
  xmlNewProp( node, ( const xmlChar* )( "map_height" ), ( const xmlChar* )( height_str.str().c_str() ) );
  stringstream start_x_str;
  start_x_str << m_start.x();
  xmlNewProp( node, ( const xmlChar* )( "start_x" ), ( const xmlChar* )( start_x_str.str().c_str() ) );
  stringstream start_y_str;
  start_y_str << m_start.y();
  xmlNewProp( node, ( const xmlChar* )( "start_y" ), ( const xmlChar* )( start_y_str.str().c_str() ) );
  stringstream goal_x_str;
  goal_x_str << m_goal.x();
  xmlNewProp( node, ( const xmlChar* )( "goal_x" ), ( const xmlChar* )( goal_x_str.str().c_str() ) );
  stringstream goal_y_str;
  goal_y_str << m_goal.y();
  xmlNewProp( node, ( const xmlChar* )( "goal_y" ), ( const xmlChar* )( goal_y_str.str().c_str() ) );
  stringstream grammar_t_str;
  grammar_t_str << static_cast<unsigned int>( m_grammar_type );
  xmlNewProp( node, ( const xmlChar* )( "grammar_type"), ( const xmlChar* )( grammar_t_str.str().c_str() ) );    
  stringstream run_t_str;
  run_t_str << static_cast<unsigned int>( m_run_type );
  xmlNewProp( node, ( const xmlChar* )( "run_type"), ( const xmlChar* )( run_t_str.str().c_str() ) );    

  string min_dist_enabled;
  if ( m_min_dist_enabled ) {
    min_dist_enabled = "1";
  } else {
    min_dist_enabled = "0";
  }
  xmlNewProp( node, ( const xmlChar* )( "min_dist_enabled" ), ( const xmlChar* )( min_dist_enabled.c_str() ) );
  xmlNewProp( node, ( const xmlChar* )( "objective_file" ), ( const xmlChar* )( m_objective_file.toStdString().c_str() ) );
  xmlNewProp( node, ( const xmlChar* )( "path_output_file" ), ( const xmlChar* )( m_paths_output.toStdString().c_str() ) );

  stringstream max_iter_num_str;
  max_iter_num_str << m_max_iteration_num;
  xmlNewProp( node, ( const xmlChar* )( "max_iteration_num" ), ( const xmlChar* )( max_iter_num_str.str().c_str() ) );
  stringstream seg_len_str;
  seg_len_str << m_segment_length;
  xmlNewProp( node, ( const xmlChar* )( "segment_length" ), ( const xmlChar* )( seg_len_str.str().c_str() ) );
    
  xmlAddChild( root, node );

  xmlNodePtr obs_list_node = xmlNewDocNode( doc, NULL, ( const xmlChar* )( "obstacles" ), NULL );
  for( unsigned int i = 0; i < m_obs_info_list.size(); i++ ) {
    ObsInfo obs_info = m_obs_info_list[i];
    xmlNodePtr obs_node = xmlNewDocNode( doc, NULL, ( const xmlChar* )( "obs" ), NULL );
    xmlNewProp( obs_node, ( const xmlChar* )( "name" ), ( const xmlChar* )( obs_info.name.c_str() ) );
    stringstream center_x_str, center_y_str;
    center_x_str << (int)obs_info.center_x;
    center_y_str << (int)obs_info.center_y;
    xmlNewProp( obs_node, ( const xmlChar* )( "center_x" ), ( const xmlChar* )( center_x_str.str().c_str() ) );
    xmlNewProp( obs_node, ( const xmlChar* )( "center_y" ), ( const xmlChar* )( center_y_str.str().c_str() ) );
    xmlAddChild( obs_list_node, obs_node );
  }
  xmlAddChild( node, obs_list_node );

  xmlNodePtr spatial_rel_list_node = xmlNewDocNode( doc, NULL, ( const xmlChar* )( "spatial_relations" ), NULL );
  for( unsigned int i = 0; i < mp_spatial_rel_info_list.size(); i++ ) {
    SpatialRelationInfo* p_spatial_rel_info = mp_spatial_rel_info_list[i];
    xmlNodePtr spatial_rel_node = xmlNewDocNode( doc, NULL, ( const xmlChar* )( "spatial_relation" ), NULL );
    xmlNewProp( spatial_rel_node, ( const xmlChar* )( "type" ), ( const xmlChar* )( p_spatial_rel_info->m_type.c_str() ) );
    for( unsigned int j = 0; j < p_spatial_rel_info->m_obstacles.size(); j++ ) {
      string obs_name = p_spatial_rel_info->m_obstacles[j];
      xmlNodePtr spatial_rel_obs_node = xmlNewDocNode( doc, NULL, ( const xmlChar* )( "obs" ), NULL );
      xmlNewProp( spatial_rel_obs_node, ( const xmlChar* )( "name" ), ( const xmlChar* )( obs_name.c_str() ) );
      xmlAddChild( spatial_rel_node, spatial_rel_obs_node );
    }
    xmlAddChild( spatial_rel_list_node, spatial_rel_node );
  }
  xmlAddChild( node, spatial_rel_list_node );
}

bool BIRRTstarPathPlanningInfo::save_to_file(QString filename) {
    
  xmlDocPtr doc = xmlNewDoc( ( xmlChar* )( "1.0" ) );
  xmlNodePtr root = xmlNewDocNode( doc, NULL, ( xmlChar* )( "root" ), NULL );    xmlDocSetRootElement( doc, root );
  write( doc, root );
  xmlSaveFormatFileEnc( filename.toStdString().c_str(), doc, "UTF-8", 1 );
  xmlFreeDoc( doc );
  return true;
}

bool BIRRTstarPathPlanningInfo::load_from_file(QString filename) {
  xmlDoc * doc = NULL;
  xmlNodePtr root = NULL;
  doc = xmlReadFile( filename.toStdString().c_str(), NULL, 0 );
  if ( doc != NULL ) {
    root = xmlDocGetRootElement( doc );
    if( root->type == XML_ELEMENT_NODE ) {
      xmlNodePtr l1 = NULL;
      for( l1 = root->children; l1; l1 = l1->next ) {
        if( l1->type == XML_ELEMENT_NODE ) { 
          if( xmlStrcmp( l1->name, ( const xmlChar * )( "world" ) ) == 0 ){ 
            read( l1 );
          }
        }
      }
    }
  }
  return true;
}

void BIRRTstarPathPlanningInfo::load_paths( vector<Path*> paths) {
  mp_found_paths = paths;
}

bool BIRRTstarPathPlanningInfo::export_paths(QString filename) {
  QFile file(filename);
  if( file.open(QIODevice::ReadWrite) ) {
    QTextStream stream( & file );
    for( vector<Path*>::iterator it = mp_found_paths.begin();
         it != mp_found_paths.end(); it++) {
      Path* p_path = (*it);
      // Save scores
      stream << p_path->m_cost << "\n";
      stream << "\n";
      for(unsigned int i=0;i<p_path->m_way_points.size();i++) {
        stream << p_path->m_way_points[i][0] << " " << p_path->m_way_points[i][1] << "\t";
      }
      stream << "\n";
      stream << "\n";
    }
    return true;
  }
  return false;
}

void BIRRTstarPathPlanningInfo::dump_cost_distribution( QString filename ) {
  QFile file(filename);
  if( file.open(QIODevice::ReadWrite) ) {
    QTextStream stream( & file );

    if( mCostDistribution ) {
      for(int i=0;i<m_map_width;i++) {
        for(int j=0;j<m_map_height;j++) {
          stream << mCostDistribution[i][j] << " ";
        }
        stream << "\n";
      }
    }
  }
}

SpatialRelationInfo* BIRRTstarPathPlanningInfo::spatialRelationFuncToInfo( SpatialRelationFunction* p_func ) {
  SpatialRelationInfo* p_spatial_rel_info = NULL;
  if( dynamic_cast<InBetweenRelationFunction*>( p_func ) ) {
    InBetweenRelationFunction* p_in_between_func = dynamic_cast<InBetweenRelationFunction*>( p_func ) ;
    p_spatial_rel_info = new SpatialRelationInfo();
    p_spatial_rel_info->m_type = SpatialRelationMgr::typeToString( h2p::SPATIAL_REL_IN_BETWEEN );
    for( vector<h2p::Obstacle*>::iterator it = p_in_between_func->mp_obstacles.begin();
         it != p_in_between_func->mp_obstacles.end(); it ++ ) {
      h2p::Obstacle* p_obs = (*it);
      p_spatial_rel_info->m_obstacles.push_back( p_obs->get_name() );
    }
  }
  else if( dynamic_cast<SideOfRelationFunction*>( p_func ) ) {
    SideOfRelationFunction* p_side_of_func = dynamic_cast<SideOfRelationFunction*>( p_func );
    p_spatial_rel_info = new SpatialRelationInfo();
    switch( p_side_of_func->m_type ) {
      case SIDE_TYPE_LEFT:
        p_spatial_rel_info->m_type = SpatialRelationMgr::typeToString( h2p::SPATIAL_REL_LEFT_OF);
        break;
      case SIDE_TYPE_RIGHT:
        p_spatial_rel_info->m_type = SpatialRelationMgr::typeToString( h2p::SPATIAL_REL_RIGHT_OF );
        break;
      case SIDE_TYPE_TOP:
        p_spatial_rel_info->m_type = SpatialRelationMgr::typeToString( h2p::SPATIAL_REL_TOP_OF );
        break;
      case SIDE_TYPE_BOTTOM:
        p_spatial_rel_info->m_type = SpatialRelationMgr::typeToString( h2p::SPATIAL_REL_BOTTOM_OF );
        break;
    }
    p_spatial_rel_info->m_obstacles.push_back( p_side_of_func->mp_obstacle->get_name() );
  }
  else if( dynamic_cast<AvoidRelationFunction*>( p_func ) ) {
    AvoidRelationFunction* p_avoid_func = dynamic_cast<AvoidRelationFunction*>( p_func );
    p_spatial_rel_info = new SpatialRelationInfo();
    p_spatial_rel_info->m_type = SpatialRelationMgr::typeToString( h2p::SPATIAL_REL_AVOID );
    p_spatial_rel_info->mp_child_info = BIRRTstarPathPlanningInfo::spatialRelationFuncToInfo( p_avoid_func->mp_child_func );
  }
  return p_spatial_rel_info;
}

SpatialRelationFunction* spatialRelationInfoToFunc( SpatialRelationInfo* p_info, SpatialRelationMgr* p_mgr ) {
  SpatialRelationFunction* p_func = NULL;
  if( p_info ) {
    SpatialRelationType type = SpatialRelationMgr::stringToType( p_info->m_type );
    switch( type ) {
      case h2p::SPATIAL_REL_IN_BETWEEN:
      {
        InBetweenRelationFunction* p_in_between_func = new InBetweenRelationFunction();
        if( p_mgr ) {
          for(unsigned int i=0; i<p_info->m_obstacles.size();i++) {
            string obs_name = p_info->m_obstacles[i];
            h2p::Obstacle* p_obs = p_mgr->get_world_map()->find_obstacle(obs_name);
            if( p_obs ) {
              p_in_between_func->mp_obstacles.push_back( p_obs );
            }
          }
        }
        break;
      }
      case h2p::SPATIAL_REL_LEFT_OF:
      {
        SideOfRelationFunction* p_side_of_func = new SideOfRelationFunction();
        p_side_of_func->m_type = SIDE_TYPE_LEFT;
        p_side_of_func->mp_obstacle = p_mgr->get_world_map()->find_obstacle( p_info->m_obstacles[0] );
        break;
      }
      case h2p::SPATIAL_REL_RIGHT_OF:
      {
        SideOfRelationFunction* p_side_of_func = new SideOfRelationFunction();
        p_side_of_func->m_type = SIDE_TYPE_RIGHT;
        p_side_of_func->mp_obstacle = p_mgr->get_world_map()->find_obstacle( p_info->m_obstacles[0] );
        break;
      }
      case h2p::SPATIAL_REL_TOP_OF:
      {
        SideOfRelationFunction* p_side_of_func = new SideOfRelationFunction();
        p_side_of_func->m_type = SIDE_TYPE_TOP;
        p_side_of_func->mp_obstacle = p_mgr->get_world_map()->find_obstacle( p_info->m_obstacles[0] );
        break;
      }
      case h2p::SPATIAL_REL_BOTTOM_OF:
      {
        SideOfRelationFunction* p_side_of_func = new SideOfRelationFunction();
        p_side_of_func->m_type = SIDE_TYPE_BOTTOM;
        p_side_of_func->mp_obstacle = p_mgr->get_world_map()->find_obstacle( p_info->m_obstacles[0] );
        break;
      }
      case h2p::SPATIAL_REL_AVOID:
      {
        AvoidRelationFunction* p_avoid_func = new AvoidRelationFunction();
        p_avoid_func->mp_child_func = spatialRelationInfoToFunc( p_info->mp_child_info, p_mgr );
        break;
      }
    }
  }
  return p_func;
}
