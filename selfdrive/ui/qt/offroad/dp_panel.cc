#include "selfdrive/ui/qt/offroad/dp_panel.h"


void DPPanel::add_general_toggles() {
  std::vector<std::tuple<QString, QString, QString>> toggle_defs{
    {
      "",
      QString::fromUtf8("🐉 ") + tr("General"),
      "",
    },
  };
  std::vector<QString> display_off_mode_texts{tr("Std."), tr("MAIN+"), tr("OP+"), tr("MAIN-"), tr("OP-")};
  ButtonParamControl* display_off_mode_setting = new ButtonParamControl("dp_general_display_mode", tr("Display Mode"),
                                          tr("Std. - Stock behavior.\nMAIN+ - ACC MAIN on = Display ON.\nOP+ - OP enabled = Display ON.\nMAIN- - ACC MAIN on = Display OFF\nOP- - OP enabled = Display OFF."),
                                          "",
                                          display_off_mode_texts, 200);

  auto hide_hud = new ParamSpinBoxControl("dp_general_hide_hud_speed_kph", tr("Hide HUD When Moves above:"), tr("To prevent screen burn-in, hide Speed, MAX Speed, and Steering/DM Icons when the car moves.\nOff = Stock Behavior\n1 km/h ≈ 0.6 mph"), "", 0, 120, 5, tr(" km/h"), tr("Off"));
  for (auto &[param, title, desc] : toggle_defs) {
    if (param == "") {
      auto label = new LabelControl(title, "");
      addItem(label);
      addItem(display_off_mode_setting);
      addItem(hide_hud);
      continue;
    }
    auto toggle = new ParamControl(param, title, desc, "", this);

    bool locked = params.getBool((param + "Lock").toStdString());
    toggle->setEnabled(!locked);
    addItem(toggle);
    toggles[param.toStdString()] = toggle;
  }
}

void DPPanel::add_lateral_toggles() {
  std::vector<std::tuple<QString, QString, QString>> toggle_defs{
    {
      "",
      QString::fromUtf8("🐉 ") + tr("Lateral Ctrl"),
      "",
    },
    {
      "dp_lat_alka",
      tr("Enable Always-on Lane Keeping Assist"),
      "",
    },
    {
      "dp_lat_road_edge_detection",
      tr("Road Edge Detection"),
      tr("Block lane change assist when the system detects the road edge.")
    },
  };

  auto lca_speed_toggle = new ParamSpinBoxControl("dp_lat_lca_speed", tr("LCA Speed:"), tr("Off = Disable LCA\n1 mph ≈ 1.2 km/h"), "", 0, 100, 5, tr(" mph"), tr("Off"));

  for (auto &[param, title, desc] : toggle_defs) {
    if (param == "") {
      auto label = new LabelControl(title, "");
      addItem(label);
      addItem(lca_speed_toggle);
      continue;
    }
    auto toggle = new ParamControl(param, title, desc, "", this);

    bool locked = params.getBool((param + "Lock").toStdString());
    toggle->setEnabled(!locked);
    addItem(toggle);
    toggles[param.toStdString()] = toggle;
  }
}

void DPPanel::add_longitudinal_toggles() {
  std::vector<std::tuple<QString, QString, QString>> toggle_defs{
    {
      "",
      QString::fromUtf8("🐉 ") + tr("Longitudinal Ctrl"),
      "",
    },
    {
      "dp_lon_ext_radar",
      tr("Use External Radar"),
      tr("See https://github.com/eFiniLan/openpilot-ext-radar-addon for more information."),
    },
  };
  for (auto &[param, title, desc] : toggle_defs) {
    if (param == "") {
      auto label = new LabelControl(title, "");
      addItem(label);
      continue;
    }
    auto toggle = new ParamControl(param, title, desc, "", this);

    bool locked = params.getBool((param + "Lock").toStdString());
    toggle->setEnabled(!locked);
    addItem(toggle);
    toggles[param.toStdString()] = toggle;
  }
}

void DPPanel::add_toyota_toggles() {
  std::vector<std::tuple<QString, QString, QString>> toggle_defs{
    {
      "",
      QString::fromUtf8("🐉 ") + tr("Toyota / Lexus"),
      "",
    },
    {
      "dp_toyota_door_auto_lock_unlock",
      tr("Enable Door Auto Lock/Unlock"),
      "",
    },
    {
      "dp_toyota_tss1_sng",
      tr("Enable TSS1 SnG Mod"),
      "",
    },
    {
      "dp_toyota_stock_lon",
      tr("Use Stock Longitudinal Control"),
      "",
    },
  };
  for (auto &[param, title, desc] : toggle_defs) {
    if (param == "") {
      auto label = new LabelControl(title, "");
      addItem(label);
      continue;
    }
    auto toggle = new ParamControl(param, title, desc, "", this);

    bool locked = params.getBool((param + "Lock").toStdString());
    toggle->setEnabled(!locked);
    addItem(toggle);
    toggles[param.toStdString()] = toggle;
  }
}

void DPPanel::add_vag_toggles() {
  std::vector<std::tuple<QString, QString, QString>> toggle_defs{
    {
      "",
      QString::fromUtf8("🐉 ") + tr("VW / Audi / Skoda"),
      "",
    },
    {
      "dp_vag_a0_sng",
      tr("Enable MQB A0 SnG Mod"),
      "",
    }
  };
  for (auto &[param, title, desc] : toggle_defs) {
    if (param == "") {
      auto label = new LabelControl(title, "");
      addItem(label);
      continue;
    }
    if (param == "dp_lon_ext_radar") {
      if (!vehicle_has_radar_unavailable || !vehicle_has_long_ctrl) {
        continue;
      }
    }
    auto toggle = new ParamControl(param, title, desc, "", this);

    bool locked = params.getBool((param + "Lock").toStdString());
    toggle->setEnabled(!locked);
    addItem(toggle);
    toggles[param.toStdString()] = toggle;
  }
}

DPPanel::DPPanel(SettingsWindow *parent) : ListWidget(parent) {
  auto cp_bytes = params.get("CarParamsPersistent");
  if (!cp_bytes.empty()) {
    AlignedBuffer aligned_buf;
    capnp::FlatArrayMessageReader cmsg(aligned_buf.align(cp_bytes.data(), cp_bytes.size()));
    cereal::CarParams::Reader CP = cmsg.getRoot<cereal::CarParams>();
    brand = QString::fromStdString(CP.getBrand());
    vehicle_has_long_ctrl = hasLongitudinalControl(CP);
    vehicle_has_radar_unavailable = CP.getRadarUnavailable();
  }

  add_general_toggles();
  add_lateral_toggles();
  add_longitudinal_toggles();
  if (brand == "toyota") {
    add_toyota_toggles();
  } else if (brand == "volkswagen") {
    add_vag_toggles();
  }
}

void DPPanel::expandToggleDescription(const QString &param) {
  toggles[param.toStdString()]->showDescription();
}
