#include "ResizableStackedWidget.h"

void ResizableStackedWidget::addWidget(QWidget *pWidget) {
  pWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  QStackedWidget::addWidget(pWidget);
}

void ResizableStackedWidget::onCurrentChanged(int index) {
  QWidget *pWidget = widget(index);
  Q_ASSERT(pWidget);
  pWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  pWidget->adjustSize();
  adjustSize();
}
ResizableStackedWidget::ResizableStackedWidget(QWidget *parent)
    : QStackedWidget(parent) {
  connect(this, &QStackedWidget::currentChanged, this,
          &ResizableStackedWidget::onCurrentChanged);
}
