// MIT Non-Commercial License
// Copyright (c) 2025 Rick Lan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, for non-commercial purposes only, subject to the following conditions:
//
// - The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// - Commercial use (e.g., use in a product, service, or activity intended to generate revenue) is prohibited without explicit written permission from Rick Lan. Contact ricklan@gmail.com for inquiries.
// - Any project that uses the Software must visibly mention the following acknowledgment: "This project uses software from Rick Lan and is licensed under a custom license requiring permission for use."
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "selfdrive/ui/qt/offroad/model_selector.h"

ModelSelector::ModelSelector(QStackedWidget* panel_widget, QWidget *parent)
    : QWidget(parent), panel_widget(panel_widget) {

    setupUI();
    loadModelList();
    connectSignals();
}

void ModelSelector::setupUI() {
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    main_layout->addSpacing(10);

    // Selector button
    QWidget* model_selector_btn_widget = new QWidget;
    QHBoxLayout* model_selector_btn_layout = new QHBoxLayout();
    QLabel* vehicle_model_label = new QLabel(tr("Vehicle Model:"));
    vehicle_model_label->setStyleSheet("margin-right: 2px; font-size: 48px;");
    model_selector_btn_layout->addWidget(vehicle_model_label);

    QString model_selected = QString::fromUtf8(Params().get("dp_general_model_selected").c_str());
    model_selector_btn = new QPushButton(model_selected.isEmpty() ? tr("[AUTO DETECT]") : model_selected);
    model_selector_btn->setObjectName("ModelSelectorBtn");
    model_selector_btn->setStyleSheet("background-color: #00309a; font-size: 48px;");
    model_selector_btn_layout->addWidget(model_selector_btn);
    model_selector_btn_layout->setAlignment(Qt::AlignCenter);
    model_selector_btn_layout->setStretch(1, 1);

    model_selector_btn_widget->setLayout(model_selector_btn_layout);
    main_layout->addWidget(model_selector_btn_widget);
    main_layout->addSpacing(10);

    // Car list widget
    model_list = new QListWidget(this);
    model_list->setStyleSheet(R"(
        QListWidget {
            font-size: 64px;
        }
        QScrollBar:vertical {
            width: 96px;
        }
    )");
    model_list->setFixedHeight(750);

    // Car list panel
    QWidget* model_list_panel = new QWidget;
    QVBoxLayout* model_list_layout = new QVBoxLayout(model_list_panel);
    model_list_layout->setContentsMargins(QMargins());
    model_list_layout->addWidget(model_list);

    // Scroll view for the car list
    panel_frame = new ScrollView(model_list_panel, this);
    panel_widget->addWidget(panel_frame);

    main_layout->addWidget(panel_widget);
    setLayout(main_layout);
}

void ModelSelector::loadModelList() {
    model_list->addItem(tr("[AUTO DETECT]"));

    Params params;
    QString list = QString::fromStdString(params.get("dp_general_model_list"));

    QJsonDocument document = QJsonDocument::fromJson(list.toUtf8());
    if (document.isArray()) {
        QJsonArray models = document.array();

        // [{"group": "Toyota", "models": ["Camry", "Corolla"]}, ...]
        for (const auto& groupValue : models) {
            QJsonObject group = groupValue.toObject();
            QString groupName = group["group"].toString();

            // Add group header item
            QListWidgetItem* groupHeader = new QListWidgetItem(groupName);
            groupHeader->setFlags(Qt::NoItemFlags); // Make non-selectable
            groupHeader->setBackground(QColor(200, 200, 200)); // Gray background
            groupHeader->setForeground(QColor(0, 0, 0)); // Black text
            groupHeader->setTextAlignment(Qt::AlignCenter);
            model_list->addItem(groupHeader);

            // Add models in this group
            QJsonArray groupModels = group["models"].toArray();
            for (const auto& model : groupModels) {
                model_list->addItem("  " + model.toString()); // Indent models
            }
        }
    } else {
        qWarning() << "Error: Invalid JSON format or not an array.";
    }
}

void ModelSelector::connectSignals() {
    connect(model_selector_btn, &QPushButton::clicked, [this]() {
        panel_widget->setCurrentWidget(panel_frame);
    });

    connect(model_list, &QListWidget::itemClicked, [this](QListWidgetItem* item) {
        // Only process clicks on selectable items (not group headers)
        if (item->flags() & Qt::ItemIsSelectable) {
            QString text = item->text().trimmed(); // Remove leading spaces from indentation
            text = (text == tr("[AUTO DETECT]")) ? QString() : text;
            Params().put("dp_general_model_selected", text.toStdString());
            model_selector_btn->setText(text.isEmpty() ? tr("[AUTO DETECT]") : text);

            // Hide the panel after selection
//            panel_widget->setCurrentIndex(0);
        }
    });
}
