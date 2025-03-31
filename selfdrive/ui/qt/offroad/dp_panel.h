#pragma once

#include "selfdrive/ui/qt/offroad/settings.h"

class DPPanel : public ListWidget {
  Q_OBJECT
public:
  explicit DPPanel(SettingsWindow *parent);

public slots:
  void expandToggleDescription(const QString &param);

private:
  Params params;
  std::map<std::string, ParamControl*> toggles;
  QString brand;
  bool vehicle_has_long_ctrl;
  bool vehicle_has_radar_unavailable;

  void add_general_toggles();
  void add_lateral_toggles();
  void add_longitudinal_toggles();
  void add_toyota_toggles();
  void add_vag_toggles();
};
