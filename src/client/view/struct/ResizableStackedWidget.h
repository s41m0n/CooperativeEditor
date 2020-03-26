#ifndef COOPERATIVEEDITOR_RESIZABLESTACKEDWIDGET_H
#define COOPERATIVEEDITOR_RESIZABLESTACKEDWIDGET_H

#include <QStackedWidget>

class ResizableStackedWidget : public QStackedWidget {
public:
  ResizableStackedWidget(QWidget *parent);
  void addWidget(QWidget* pWidget);
public slots:
  void onCurrentChanged(int index);
};

#endif // COOPERATIVEEDITOR_RESIZABLESTACKEDWIDGET_H
