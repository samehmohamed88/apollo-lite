{"payload":{"allShortcutsEnabled":false,"fileTree":{"zed_components/src/zed_camera/src":{"items":[{"name":"cost_traversability.cpp","path":"zed_components/src/zed_camera/src/cost_traversability.cpp","contentType":"file"},{"name":"zed_camera_component.cpp","path":"zed_components/src/zed_camera/src/zed_camera_component.cpp","contentType":"file"}],"totalCount":2},"zed_components/src/zed_camera":{"items":[{"name":"include","path":"zed_components/src/zed_camera/include","contentType":"directory"},{"name":"src","path":"zed_components/src/zed_camera/src","contentType":"directory"}],"totalCount":2},"zed_components/src":{"items":[{"name":"include","path":"zed_components/src/include","contentType":"directory"},{"name":"tools","path":"zed_components/src/tools","contentType":"directory"},{"name":"zed_camera","path":"zed_components/src/zed_camera","contentType":"directory"}],"totalCount":3},"zed_components":{"items":[{"name":"src","path":"zed_components/src","contentType":"directory"},{"name":"CMakeLists.txt","path":"zed_components/CMakeLists.txt","contentType":"file"},{"name":"package.xml","path":"zed_components/package.xml","contentType":"file"}],"totalCount":3},"":{"items":[{"name":".ci","path":".ci","contentType":"directory"},{"name":".github","path":".github","contentType":"directory"},{"name":"docker","path":"docker","contentType":"directory"},{"name":"images","path":"images","contentType":"directory"},{"name":"zed-ros2-interfaces","path":"zed-ros2-interfaces","contentType":"submodule","submoduleUrl":"/stereolabs/zed-ros2-interfaces/tree/f7f90f11f818c3dda23219cd05d8c854a569fc85","submoduleDisplayName":"zed-ros2-interfaces @ f7f90f1"},{"name":"zed_components","path":"zed_components","contentType":"directory"},{"name":"zed_ros2","path":"zed_ros2","contentType":"directory"},{"name":"zed_wrapper","path":"zed_wrapper","contentType":"directory"},{"name":".gitignore","path":".gitignore","contentType":"file"},{"name":".gitlab-ci.yml","path":".gitlab-ci.yml","contentType":"file"},{"name":".gitmodules","path":".gitmodules","contentType":"file"},{"name":"CHANGELOG.rst","path":"CHANGELOG.rst","contentType":"file"},{"name":"CONTRIBUTING.md","path":"CONTRIBUTING.md","contentType":"file"},{"name":"LICENSE","path":"LICENSE","contentType":"file"},{"name":"README.md","path":"README.md","contentType":"file"}],"totalCount":15}},"fileTreeProcessingTime":13.618604,"foldersToFetch":[],"reducedMotionEnabled":null,"repo":{"id":181023408,"defaultBranch":"master","name":"zed-ros2-wrapper","ownerLogin":"stereolabs","currentUserCanPush":false,"isFork":false,"isEmpty":false,"createdAt":"2019-04-12T14:23:46.000Z","ownerAvatar":"https://avatars.githubusercontent.com/u/14199582?v=4","public":true,"private":false,"isOrgOwned":true},"symbolsExpanded":false,"treeExpanded":true,"refInfo":{"name":"master","listCacheKey":"v0:1701701364.0","canEdit":false,"refType":"branch","currentOid":"5368f79d200ca89ecb39935751563ef64a4c8bf0"},"path":"zed_components/src/zed_camera/src/cost_traversability.cpp","currentUser":null,"blob":{"rawLines":["#include \"cost_traversability.hpp\"","","namespace stereolabs","{","namespace cost_traversability","{","template<typename T>","T clamp(T const & v, T const & lo, T const & hi)","{","  return (v < lo) ? lo : ((v > hi) ? hi : v);","}","","void initCostTraversibily(","  sl::Terrain & cost_terrain, float resolution, float range,","  float height_threshold)","{","  std::vector<sl::LayerName> layers;","  layers.push_back(TRAVERSABILITY_COST);","  layers.push_back(OCCUPANCY);","  layers.push_back(TRAVERSABILITY_COST_STEP);","  layers.push_back(TRAVERSABILITY_COST_SLOPE);","  layers.push_back(TRAVERSABILITY_COST_ROUGHNESS);","","  auto range_cell = round(range / resolution);","  cost_terrain.init(resolution, range_cell, layers);","}","","void computeCost(","  sl::Terrain & elevation_terrain, sl::Terrain & cost_terrain,","  const float grid_resolution, RobotParameters robot_parameters,","  TraversabilityParameters traversability_parameters)","{","  auto square_size_cost = robot_parameters.radius / grid_resolution;","  if (square_size_cost < 1) {","    square_size_cost = 1;","  }","","  auto factor_step_ = traversability_parameters.step_weight / robot_parameters.step_max;","  auto factor_slope_ = traversability_parameters.slope_weight / robot_parameters.slope_max;","  auto factor_roughness_ = traversability_parameters.roughness_weight /","    robot_parameters.roughness_max;","","  // Update only the recent one, and manage the border ?","  const float step_height_crit = robot_parameters.step_max;","","  double reso_d = grid_resolution * 1.;","","  sl::Timestamp ts_tmp_elevation, ts_tmp_cost;","","  double a_rad = robot_parameters.radius * 1.;","  int nb_cells = (2. * a_rad) / reso_d;           // big agent with small grid size is heavier to compute","","  const sl::float3 z_vector(0, 0, 1);","","  auto chunks_idx = elevation_terrain.getAllValidChunk();","","  // for each chunk","  for (auto chunk_id : chunks_idx) {","","    auto & chunk_elevation = elevation_terrain.getChunk(chunk_id);","    auto & layer_height = chunk_elevation.getLayer(sl::LayerName::ELEVATION);","","    auto & chunk_cost = cost_terrain.getChunk(chunk_id);","","    chunk_cost.getLayer(TRAVERSABILITY_COST).clear();","    chunk_cost.getLayer(OCCUPANCY).clear();","    chunk_cost.getLayer(TRAVERSABILITY_COST_STEP).clear();","    chunk_cost.getLayer(TRAVERSABILITY_COST_SLOPE).clear();","    chunk_cost.getLayer(TRAVERSABILITY_COST_ROUGHNESS).clear();","","    auto & cost_data = chunk_cost.getLayer(TRAVERSABILITY_COST).getData();","    auto & occupancy_data = chunk_cost.getLayer(OCCUPANCY).getData();","    auto & cost_step_data = chunk_cost.getLayer(TRAVERSABILITY_COST_STEP).getData();","    auto & cost_slope_data = chunk_cost.getLayer(TRAVERSABILITY_COST_SLOPE).getData();","    auto & cost_roughness_data = chunk_cost.getLayer(TRAVERSABILITY_COST_ROUGHNESS).getData();","","    auto dim = chunk_elevation.getDimension();","    const int size_ = dim.getSize() * dim.getSize();","","    auto & elevation_data = layer_height.getData();","","    unsigned int idx_tmp;","    float x, y;","    for (unsigned int idx_current = 0; idx_current < size_; idx_current++) {","      const float ref_height = elevation_data[idx_current];","      if (std::isfinite(ref_height)) {","        dim.index2x_y(idx_current, x, y);","        // SLOPE","        std::vector<sl::float3> normals_tmp;","        normals_tmp.reserve(nb_cells * nb_cells);","","        float max_diff_height = 0;","","        double x_area_min = x - a_rad;","        double y_area_min = y - a_rad;","","        for (int x_ = 0; x_ < nb_cells; x_++) {","          float x_v = x_area_min + (x_ * reso_d);","          for (int y_ = 0; y_ < nb_cells; y_++) {","            float y_v = y_area_min + (y_ * reso_d);","","            float curr_height;","            if (dim.getIndex(x_v, y_v, idx_tmp) /*True = error*/) {","              // Probably chunk edges","              curr_height = elevation_terrain.readValue(sl::LayerName::ELEVATION, x_v, y_v);","            } else {","              curr_height = elevation_data[idx_tmp];","            }","","            if (std::isfinite(curr_height)) {","              normals_tmp.emplace_back(x_v, y_v, curr_height);","              max_diff_height = std::max(max_diff_height, fabsf32(curr_height - ref_height));","            }","          }","        }","","        sl::float3 normal, centroid, eigen_values;","        plane::compute_pca(normals_tmp, normal, centroid, eigen_values);","","        float roughness = 0, slope = 0, step = 0, cost;","","        if (max_diff_height > step_height_crit) {","          step = max_diff_height;","        }","","        if (normals_tmp.size() >= 3) {                                                                                                                              // minimum points","          slope =","            acos(","            sl::float3::dot(","              normal,","              z_vector) / sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z)) *","            57.295779513;                                                                                                                                           // the norm of z_vector is 1","","        }","        if (slope > 90) {","          slope = 180.f - slope;","        }","","        roughness = sqrt(eigen_values.z);                 // Standard deviation of fitted plane","","        cost = clamp(","          (roughness * factor_roughness_ + slope * factor_slope_ + step * factor_step_) * 0.3f,","          0.f, 1.f);","        cost_data[idx_current] = cost;","        occupancy_data[idx_current] =","          (cost > traversability_parameters.occupancy_threshold) ? OCCUPIED_CELL : FREE_CELL;","        if (slope == 0) {","          slope = INVALID_CELL_DATA;","        }","        cost_slope_data[idx_current] = slope;","        cost_step_data[idx_current] = step;","        cost_roughness_data[idx_current] = roughness;","      } else {","        occupancy_data[idx_current] = UNKNOWN_CELL;","      }","    }","  }","}","","static const sl::float3 clr_a(244, 242, 246);","static const sl::float3 clr_b(0, 0, 0);","","// generate  a linear ColorMap to match ogl interpol","inline sl::uchar3 getColorMap(float value)","{","  auto new_clr = clr_a * value + clr_b * (1.f - value);","  return sl::uchar3(new_clr.b, new_clr.g, new_clr.r);","}","","void normalization(sl::Terrain & cost_terrain, sl::LayerName layer, sl::Mat & view)","{","  sl::Mat cost;","  auto cost_mat = cost_terrain.retrieveView(cost, sl::MAT_TYPE::F32_C1, layer);","  auto cost_res = cost.getResolution();","  view.alloc(cost_res, sl::MAT_TYPE::U8_C3);","","  for (int y = 0; y < cost_res.height; y++) {","","    auto ptr_cost = cost.getPtr<float>() + y * cost.getStep();","    auto ptr_view = view.getPtr<sl::uchar3>() + y * view.getStep();","","    for (int x = 0; x < cost_res.width; x++) {","      float cost = ptr_cost[x];","      if (std::isfinite(cost)) {","        ptr_view[x] = getColorMap(cost);","      } else {","        ptr_view[x] = sl::uchar3(22, 22, 22);","      }","    }","  }","}","}","}"],"stylingDirectives":[[{"start":1,"end":8,"cssClass":"pl-k"},{"start":9,"end":34,"cssClass":"pl-s"},{"start":9,"end":10,"cssClass":"pl-pds"},{"start":33,"end":34,"cssClass":"pl-pds"}],[],[{"start":0,"end":9,"cssClass":"pl-k"},{"start":10,"end":20,"cssClass":"pl-en"}],[],[{"start":0,"end":9,"cssClass":"pl-k"},{"start":10,"end":29,"cssClass":"pl-en"}],[],[{"start":0,"end":8,"cssClass":"pl-k"},{"start":9,"end":17,"cssClass":"pl-k"}],[{"start":2,"end":7,"cssClass":"pl-en"},{"start":10,"end":15,"cssClass":"pl-k"},{"start":23,"end":28,"cssClass":"pl-k"},{"start":37,"end":42,"cssClass":"pl-k"}],[],[{"start":2,"end":8,"cssClass":"pl-k"}],[],[],[{"start":0,"end":4,"cssClass":"pl-k"},{"start":5,"end":25,"cssClass":"pl-en"}],[{"start":30,"end":35,"cssClass":"pl-k"},{"start":48,"end":53,"cssClass":"pl-k"}],[{"start":2,"end":7,"cssClass":"pl-k"}],[],[],[{"start":9,"end":18,"cssClass":"pl-c1"}],[{"start":9,"end":18,"cssClass":"pl-c1"}],[{"start":9,"end":18,"cssClass":"pl-c1"}],[{"start":9,"end":18,"cssClass":"pl-c1"}],[{"start":9,"end":18,"cssClass":"pl-c1"}],[],[{"start":2,"end":6,"cssClass":"pl-k"},{"start":20,"end":25,"cssClass":"pl-c1"}],[{"start":15,"end":19,"cssClass":"pl-c1"}],[],[],[{"start":0,"end":4,"cssClass":"pl-k"},{"start":5,"end":16,"cssClass":"pl-en"}],[],[{"start":2,"end":7,"cssClass":"pl-k"},{"start":8,"end":13,"cssClass":"pl-k"}],[],[],[{"start":2,"end":6,"cssClass":"pl-k"},{"start":43,"end":49,"cssClass":"pl-smi"}],[{"start":2,"end":4,"cssClass":"pl-k"},{"start":25,"end":26,"cssClass":"pl-c1"}],[{"start":23,"end":24,"cssClass":"pl-c1"}],[],[],[{"start":2,"end":6,"cssClass":"pl-k"},{"start":48,"end":59,"cssClass":"pl-smi"},{"start":79,"end":87,"cssClass":"pl-smi"}],[{"start":2,"end":6,"cssClass":"pl-k"},{"start":49,"end":61,"cssClass":"pl-smi"},{"start":81,"end":90,"cssClass":"pl-smi"}],[{"start":2,"end":6,"cssClass":"pl-k"},{"start":53,"end":69,"cssClass":"pl-smi"}],[{"start":21,"end":34,"cssClass":"pl-smi"}],[],[{"start":2,"end":56,"cssClass":"pl-c"},{"start":2,"end":4,"cssClass":"pl-c"}],[{"start":2,"end":7,"cssClass":"pl-k"},{"start":8,"end":13,"cssClass":"pl-k"},{"start":50,"end":58,"cssClass":"pl-smi"}],[],[{"start":2,"end":8,"cssClass":"pl-k"},{"start":36,"end":37,"cssClass":"pl-c1"}],[],[],[],[{"start":2,"end":8,"cssClass":"pl-k"},{"start":34,"end":40,"cssClass":"pl-smi"},{"start":43,"end":44,"cssClass":"pl-c1"}],[{"start":2,"end":5,"cssClass":"pl-k"},{"start":18,"end":19,"cssClass":"pl-c1"},{"start":50,"end":105,"cssClass":"pl-c"},{"start":50,"end":52,"cssClass":"pl-c"}],[],[{"start":2,"end":7,"cssClass":"pl-k"},{"start":19,"end":27,"cssClass":"pl-smi"},{"start":28,"end":29,"cssClass":"pl-c1"},{"start":31,"end":32,"cssClass":"pl-c1"},{"start":34,"end":35,"cssClass":"pl-c1"}],[],[{"start":2,"end":6,"cssClass":"pl-k"},{"start":38,"end":54,"cssClass":"pl-c1"}],[],[{"start":2,"end":19,"cssClass":"pl-c"},{"start":2,"end":4,"cssClass":"pl-c"}],[{"start":2,"end":5,"cssClass":"pl-k"},{"start":7,"end":11,"cssClass":"pl-k"}],[],[{"start":4,"end":8,"cssClass":"pl-k"},{"start":47,"end":55,"cssClass":"pl-c1"}],[{"start":4,"end":8,"cssClass":"pl-k"},{"start":42,"end":50,"cssClass":"pl-c1"}],[],[{"start":4,"end":8,"cssClass":"pl-k"},{"start":37,"end":45,"cssClass":"pl-c1"}],[],[{"start":15,"end":23,"cssClass":"pl-c1"},{"start":45,"end":50,"cssClass":"pl-c1"}],[{"start":15,"end":23,"cssClass":"pl-c1"},{"start":35,"end":40,"cssClass":"pl-c1"}],[{"start":15,"end":23,"cssClass":"pl-c1"},{"start":50,"end":55,"cssClass":"pl-c1"}],[{"start":15,"end":23,"cssClass":"pl-c1"},{"start":51,"end":56,"cssClass":"pl-c1"}],[{"start":15,"end":23,"cssClass":"pl-c1"},{"start":55,"end":60,"cssClass":"pl-c1"}],[],[{"start":4,"end":8,"cssClass":"pl-k"},{"start":34,"end":42,"cssClass":"pl-c1"},{"start":64,"end":71,"cssClass":"pl-c1"}],[{"start":4,"end":8,"cssClass":"pl-k"},{"start":39,"end":47,"cssClass":"pl-c1"},{"start":59,"end":66,"cssClass":"pl-c1"}],[{"start":4,"end":8,"cssClass":"pl-k"},{"start":39,"end":47,"cssClass":"pl-c1"},{"start":74,"end":81,"cssClass":"pl-c1"}],[{"start":4,"end":8,"cssClass":"pl-k"},{"start":40,"end":48,"cssClass":"pl-c1"},{"start":76,"end":83,"cssClass":"pl-c1"}],[{"start":4,"end":8,"cssClass":"pl-k"},{"start":44,"end":52,"cssClass":"pl-c1"},{"start":84,"end":91,"cssClass":"pl-c1"}],[],[{"start":4,"end":8,"cssClass":"pl-k"},{"start":31,"end":43,"cssClass":"pl-c1"}],[{"start":4,"end":9,"cssClass":"pl-k"},{"start":10,"end":13,"cssClass":"pl-k"},{"start":26,"end":33,"cssClass":"pl-c1"},{"start":42,"end":49,"cssClass":"pl-c1"}],[],[{"start":4,"end":8,"cssClass":"pl-k"},{"start":41,"end":48,"cssClass":"pl-c1"}],[],[{"start":4,"end":12,"cssClass":"pl-k"},{"start":13,"end":16,"cssClass":"pl-k"}],[{"start":4,"end":9,"cssClass":"pl-k"}],[{"start":4,"end":7,"cssClass":"pl-k"},{"start":9,"end":17,"cssClass":"pl-k"},{"start":18,"end":21,"cssClass":"pl-k"},{"start":36,"end":37,"cssClass":"pl-c1"}],[{"start":6,"end":11,"cssClass":"pl-k"},{"start":12,"end":17,"cssClass":"pl-k"}],[{"start":6,"end":8,"cssClass":"pl-k"},{"start":10,"end":23,"cssClass":"pl-c1"}],[{"start":12,"end":21,"cssClass":"pl-c1"}],[{"start":8,"end":16,"cssClass":"pl-c"},{"start":8,"end":10,"cssClass":"pl-c"}],[],[{"start":20,"end":27,"cssClass":"pl-c1"}],[],[{"start":8,"end":13,"cssClass":"pl-k"},{"start":32,"end":33,"cssClass":"pl-c1"}],[],[{"start":8,"end":14,"cssClass":"pl-k"}],[{"start":8,"end":14,"cssClass":"pl-k"}],[],[{"start":8,"end":11,"cssClass":"pl-k"},{"start":13,"end":16,"cssClass":"pl-k"},{"start":22,"end":23,"cssClass":"pl-c1"}],[{"start":10,"end":15,"cssClass":"pl-k"}],[{"start":10,"end":13,"cssClass":"pl-k"},{"start":15,"end":18,"cssClass":"pl-k"},{"start":24,"end":25,"cssClass":"pl-c1"}],[{"start":12,"end":17,"cssClass":"pl-k"}],[],[{"start":12,"end":17,"cssClass":"pl-k"}],[{"start":12,"end":14,"cssClass":"pl-k"},{"start":20,"end":28,"cssClass":"pl-c1"},{"start":48,"end":64,"cssClass":"pl-c"},{"start":48,"end":50,"cssClass":"pl-c"},{"start":62,"end":64,"cssClass":"pl-c"}],[{"start":14,"end":37,"cssClass":"pl-c"},{"start":14,"end":16,"cssClass":"pl-c"}],[{"start":46,"end":55,"cssClass":"pl-c1"}],[{"start":14,"end":18,"cssClass":"pl-k"}],[],[],[],[{"start":12,"end":14,"cssClass":"pl-k"},{"start":16,"end":29,"cssClass":"pl-c1"}],[{"start":26,"end":38,"cssClass":"pl-c1"}],[{"start":32,"end":40,"cssClass":"pl-c1"},{"start":58,"end":65,"cssClass":"pl-c1"}],[],[],[],[],[{"start":19,"end":25,"cssClass":"pl-c1"}],[{"start":8,"end":26,"cssClass":"pl-c1"},{"start":40,"end":46,"cssClass":"pl-c1"}],[],[{"start":8,"end":13,"cssClass":"pl-k"},{"start":26,"end":27,"cssClass":"pl-c1"},{"start":37,"end":38,"cssClass":"pl-c1"},{"start":47,"end":48,"cssClass":"pl-c1"}],[],[{"start":8,"end":10,"cssClass":"pl-k"}],[],[],[],[{"start":8,"end":10,"cssClass":"pl-k"},{"start":24,"end":28,"cssClass":"pl-c1"},{"start":34,"end":35,"cssClass":"pl-c1"},{"start":164,"end":181,"cssClass":"pl-c"},{"start":164,"end":166,"cssClass":"pl-c"}],[],[{"start":12,"end":16,"cssClass":"pl-c1"}],[{"start":12,"end":27,"cssClass":"pl-c1"}],[{"start":14,"end":20,"cssClass":"pl-c1"}],[{"start":26,"end":30,"cssClass":"pl-c1"},{"start":31,"end":37,"cssClass":"pl-c1"},{"start":38,"end":39,"cssClass":"pl-smi"},{"start":42,"end":48,"cssClass":"pl-c1"},{"start":49,"end":50,"cssClass":"pl-smi"},{"start":53,"end":59,"cssClass":"pl-c1"},{"start":60,"end":61,"cssClass":"pl-smi"},{"start":64,"end":70,"cssClass":"pl-c1"},{"start":71,"end":72,"cssClass":"pl-smi"},{"start":75,"end":81,"cssClass":"pl-c1"},{"start":82,"end":83,"cssClass":"pl-smi"},{"start":86,"end":92,"cssClass":"pl-c1"},{"start":93,"end":94,"cssClass":"pl-smi"}],[{"start":12,"end":24,"cssClass":"pl-c1"},{"start":164,"end":192,"cssClass":"pl-c"},{"start":164,"end":166,"cssClass":"pl-c"}],[],[],[{"start":8,"end":10,"cssClass":"pl-k"},{"start":20,"end":22,"cssClass":"pl-c1"}],[{"start":18,"end":21,"cssClass":"pl-c1"},{"start":22,"end":23,"cssClass":"pl-smi"}],[],[],[{"start":20,"end":24,"cssClass":"pl-c1"},{"start":38,"end":39,"cssClass":"pl-smi"},{"start":58,"end":95,"cssClass":"pl-c"},{"start":58,"end":60,"cssClass":"pl-c"}],[],[{"start":15,"end":20,"cssClass":"pl-c1"}],[{"start":90,"end":91,"cssClass":"pl-c1"},{"start":92,"end":94,"cssClass":"pl-c1"}],[{"start":10,"end":11,"cssClass":"pl-c1"},{"start":12,"end":13,"cssClass":"pl-smi"},{"start":15,"end":16,"cssClass":"pl-c1"},{"start":17,"end":18,"cssClass":"pl-smi"}],[],[],[{"start":44,"end":63,"cssClass":"pl-smi"}],[{"start":8,"end":10,"cssClass":"pl-k"},{"start":21,"end":22,"cssClass":"pl-c1"}],[],[],[],[],[],[{"start":8,"end":12,"cssClass":"pl-k"}],[],[],[],[],[],[],[{"start":0,"end":6,"cssClass":"pl-k"},{"start":7,"end":12,"cssClass":"pl-k"},{"start":24,"end":29,"cssClass":"pl-en"},{"start":30,"end":33,"cssClass":"pl-c1"},{"start":35,"end":38,"cssClass":"pl-c1"},{"start":40,"end":43,"cssClass":"pl-c1"}],[{"start":0,"end":6,"cssClass":"pl-k"},{"start":7,"end":12,"cssClass":"pl-k"},{"start":24,"end":29,"cssClass":"pl-en"},{"start":30,"end":31,"cssClass":"pl-c1"},{"start":33,"end":34,"cssClass":"pl-c1"},{"start":36,"end":37,"cssClass":"pl-c1"}],[],[{"start":0,"end":52,"cssClass":"pl-c"},{"start":0,"end":2,"cssClass":"pl-c"}],[{"start":18,"end":29,"cssClass":"pl-en"},{"start":30,"end":35,"cssClass":"pl-k"}],[],[{"start":2,"end":6,"cssClass":"pl-k"},{"start":42,"end":43,"cssClass":"pl-c1"},{"start":44,"end":45,"cssClass":"pl-smi"}],[{"start":2,"end":8,"cssClass":"pl-k"},{"start":9,"end":19,"cssClass":"pl-c1"},{"start":28,"end":29,"cssClass":"pl-smi"},{"start":39,"end":40,"cssClass":"pl-smi"},{"start":50,"end":51,"cssClass":"pl-smi"}],[],[],[{"start":0,"end":4,"cssClass":"pl-k"},{"start":5,"end":18,"cssClass":"pl-en"}],[],[],[{"start":2,"end":6,"cssClass":"pl-k"},{"start":31,"end":43,"cssClass":"pl-c1"}],[{"start":2,"end":6,"cssClass":"pl-k"},{"start":23,"end":36,"cssClass":"pl-c1"}],[{"start":7,"end":12,"cssClass":"pl-c1"}],[],[{"start":2,"end":5,"cssClass":"pl-k"},{"start":7,"end":10,"cssClass":"pl-k"},{"start":15,"end":16,"cssClass":"pl-c1"},{"start":31,"end":37,"cssClass":"pl-smi"}],[],[{"start":4,"end":8,"cssClass":"pl-k"},{"start":25,"end":31,"cssClass":"pl-smi"},{"start":32,"end":37,"cssClass":"pl-k"},{"start":52,"end":59,"cssClass":"pl-c1"}],[{"start":4,"end":8,"cssClass":"pl-k"},{"start":25,"end":31,"cssClass":"pl-smi"},{"start":57,"end":64,"cssClass":"pl-c1"}],[],[{"start":4,"end":7,"cssClass":"pl-k"},{"start":9,"end":12,"cssClass":"pl-k"},{"start":17,"end":18,"cssClass":"pl-c1"},{"start":33,"end":38,"cssClass":"pl-smi"}],[{"start":6,"end":11,"cssClass":"pl-k"}],[{"start":6,"end":8,"cssClass":"pl-k"},{"start":10,"end":23,"cssClass":"pl-c1"}],[{"start":22,"end":33,"cssClass":"pl-c1"}],[{"start":8,"end":12,"cssClass":"pl-k"}],[{"start":22,"end":32,"cssClass":"pl-c1"},{"start":33,"end":35,"cssClass":"pl-c1"},{"start":37,"end":39,"cssClass":"pl-c1"},{"start":41,"end":43,"cssClass":"pl-c1"}],[],[],[],[],[],[]],"csv":null,"csvError":null,"dependabotInfo":{"showConfigurationBanner":false,"configFilePath":null,"networkDependabotPath":"/stereolabs/zed-ros2-wrapper/network/updates","dismissConfigurationNoticePath":"/settings/dismiss-notice/dependabot_configuration_notice","configurationNoticeDismissed":null,"repoAlertsPath":"/stereolabs/zed-ros2-wrapper/security/dependabot","repoSecurityAndAnalysisPath":"/stereolabs/zed-ros2-wrapper/settings/security_analysis","repoOwnerIsOrg":true,"currentUserCanAdminRepo":false},"displayName":"cost_traversability.cpp","displayUrl":"https://github.com/stereolabs/zed-ros2-wrapper/blob/master/zed_components/src/zed_camera/src/cost_traversability.cpp?raw=true","headerInfo":{"blobSize":"6.58 KB","deleteInfo":{"deleteTooltip":"You must be signed in to make or propose changes"},"editInfo":{"editTooltip":"You must be signed in to make or propose changes"},"ghDesktopPath":"https://desktop.github.com","gitLfsPath":null,"onBranch":true,"shortPath":"54d8101","siteNavLoginPath":"/login?return_to=https%3A%2F%2Fgithub.com%2Fstereolabs%2Fzed-ros2-wrapper%2Fblob%2Fmaster%2Fzed_components%2Fsrc%2Fzed_camera%2Fsrc%2Fcost_traversability.cpp","isCSV":false,"isRichtext":false,"toc":null,"lineInfo":{"truncatedLoc":"193","truncatedSloc":"156"},"mode":"file"},"image":false,"isCodeownersFile":null,"isPlain":false,"isValidLegacyIssueTemplate":false,"issueTemplateHelpUrl":"https://docs.github.com/articles/about-issue-and-pull-request-templates","issueTemplate":null,"discussionTemplate":null,"language":"C++","languageID":43,"large":false,"loggedIn":false,"newDiscussionPath":"/stereolabs/zed-ros2-wrapper/discussions/new","newIssuePath":"/stereolabs/zed-ros2-wrapper/issues/new","planSupportInfo":{"repoIsFork":null,"repoOwnedByCurrentUser":null,"requestFullPath":"/stereolabs/zed-ros2-wrapper/blob/master/zed_components/src/zed_camera/src/cost_traversability.cpp","showFreeOrgGatedFeatureMessage":null,"showPlanSupportBanner":null,"upgradeDataAttributes":null,"upgradePath":null},"publishBannersInfo":{"dismissActionNoticePath":"/settings/dismiss-notice/publish_action_from_dockerfile","dismissStackNoticePath":"/settings/dismiss-notice/publish_stack_from_file","releasePath":"/stereolabs/zed-ros2-wrapper/releases/new?marketplace=true","showPublishActionBanner":false,"showPublishStackBanner":false},"rawBlobUrl":"https://github.com/stereolabs/zed-ros2-wrapper/raw/master/zed_components/src/zed_camera/src/cost_traversability.cpp","renderImageOrRaw":false,"richText":null,"renderedFileInfo":null,"shortPath":null,"tabSize":8,"topBannersInfo":{"overridingGlobalFundingFile":false,"globalPreferredFundingPath":null,"repoOwner":"stereolabs","repoName":"zed-ros2-wrapper","showInvalidCitationWarning":false,"citationHelpUrl":"https://docs.github.com/en/github/creating-cloning-and-archiving-repositories/creating-a-repository-on-github/about-citation-files","showDependabotConfigurationBanner":false,"actionsOnboardingTip":null},"truncated":false,"viewable":true,"workflowRedirectUrl":null,"symbols":{"timed_out":false,"not_analyzed":false,"symbols":[{"name":"clamp","kind":"function","ident_start":114,"ident_end":119,"extent_start":112,"extent_end":210,"fully_qualified_name":"clamp","ident_utf16":{"start":{"line_number":7,"utf16_col":2},"end":{"line_number":7,"utf16_col":7}},"extent_utf16":{"start":{"line_number":7,"utf16_col":0},"end":{"line_number":10,"utf16_col":1}}},{"name":"initCostTraversibily","kind":"function","ident_start":217,"ident_end":237,"extent_start":212,"extent_end":683,"fully_qualified_name":"initCostTraversibily","ident_utf16":{"start":{"line_number":12,"utf16_col":5},"end":{"line_number":12,"utf16_col":25}},"extent_utf16":{"start":{"line_number":12,"utf16_col":0},"end":{"line_number":25,"utf16_col":1}}},{"name":"computeCost","kind":"function","ident_start":690,"ident_end":701,"extent_start":685,"extent_end":5754,"fully_qualified_name":"computeCost","ident_utf16":{"start":{"line_number":27,"utf16_col":5},"end":{"line_number":27,"utf16_col":16}},"extent_utf16":{"start":{"line_number":27,"utf16_col":0},"end":{"line_number":157,"utf16_col":1}}},{"name":"getColorMap","kind":"function","ident_start":5914,"ident_end":5925,"extent_start":5896,"extent_end":6052,"fully_qualified_name":"getColorMap","ident_utf16":{"start":{"line_number":163,"utf16_col":18},"end":{"line_number":163,"utf16_col":29}},"extent_utf16":{"start":{"line_number":163,"utf16_col":0},"end":{"line_number":167,"utf16_col":1}}},{"name":"normalization","kind":"function","ident_start":6059,"ident_end":6072,"extent_start":6054,"extent_end":6734,"fully_qualified_name":"normalization","ident_utf16":{"start":{"line_number":169,"utf16_col":5},"end":{"line_number":169,"utf16_col":18}},"extent_utf16":{"start":{"line_number":169,"utf16_col":0},"end":{"line_number":190,"utf16_col":1}}}]}},"copilotInfo":null,"copilotAccessAllowed":false,"csrf_tokens":{"/stereolabs/zed-ros2-wrapper/branches":{"post":"mphDv646YquBZI3lXSX1KiNF3gRgScS0w0GxUSevUdWCB4ZgXLhihGWQiXmflim1cdHfXh2r5Ve-0ayOVb9yVA"},"/repos/preferences":{"post":"pC8cw84ZJ3lkgZKIlc0S5X-CQadS25uYC9Ey0EKthn_D6EyYuhSJCBYCNbfPfY058imlIjJO3Reu8X4KBt9ioA"}}},"title":"zed-ros2-wrapper/zed_components/src/zed_camera/src/cost_traversability.cpp at master · stereolabs/zed-ros2-wrapper"}