/*
    This file is part of Viva.

    Viva is free software: you can redistribute it and/or modify it
    under the terms of the GNU Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Viva is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Public License for more details.

    You should have received a copy of the GNU Public License
    along with Viva. If not, see <http://www.gnu.org/licenses/>.
*/
#include "VTWidget.h"

VTWidget::VTWidget (QWidget *parent)
  : QGLWidget (QGLFormat (QGL::SampleBuffers), parent)
{
  treemap = NULL;
  currentDepth = 0;
  setFocusPolicy (Qt::StrongFocus);
  zoomType = GlobalZoom;

  entropyConfigurationWidget = new EntropyConfigurationWidget (0.3, this);
}

VTWidget::~VTWidget (void)
{
  delete treemap;
}

QSize VTWidget::minimumSizeHint() const
{
  return QSize(50, 50);
}

QSize VTWidget::sizeHint() const
{
  return QSize(400, 400);
}

void VTWidget::resizeEvent (QResizeEvent *event)
{
  repopulate();
}

void VTWidget::paintEvent(QPaintEvent *event)
{
  QBrush whiteBackground = QBrush(QColor(255, 255, 255));

  QPainter painter;
  painter.begin(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.fillRect(event->rect(), whiteBackground);
  redraw (&painter);
  painter.end();
}

void VTWidget::mousePressEvent(QMouseEvent *event)
{
}

void VTWidget::mouseMoveEvent(QMouseEvent *event)
{
}

void VTWidget::wheelEvent (QWheelEvent *event)
{
  if (event->delta() > 0){
    currentDepth++;
    if (currentDepth > treemap->maxDepth()) currentDepth = treemap->maxDepth();
  }else{
    currentDepth--;
    if (currentDepth < 0) currentDepth = 0;
  }
  update();
}

void VTWidget::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_E){
    //first, populate entropyConfigurationWidget
    entropyConfigurationWidget->updateVariables (spatialIntegrationOfContainer(rootInstance()));
    updateEntropyData ();
    entropyConfigurationWidget->show();
    zoomType = EntropyZoom;
  }else if(event->key() == Qt::Key_G){
    zoomType = GlobalZoom;
    update();
  }else if(event->key() == Qt::Key_L){
    zoomType = LocalZoom;
    update();
  }else{
    QWidget::keyPressEvent(event);
  }
}

void VTWidget::updateEntropyData (void)
{
  double p = entropyConfigurationWidget->p();
  PajeAggregatedType *type = entropyConfigurationWidget->type();
  if (!type) return;
  bestAggregation = findBestAggregation (rootInstance(), type, p);
  std::cout << "Considering a p: " << p << " and variable: " << type->description() << std::endl;
  std::cout << "-> maxRIC = " << bestAggregation.first << std::endl;
  std::vector<PajeContainer*>::iterator it;
  std::cout << "-> bestAggregation = {";
  for (it = bestAggregation.second.begin(); it != bestAggregation.second.end(); it++){
    PajeContainer *c = *it;
    std::cout << c->name() << ", ";
  }
  std::cout << "}" << std::endl;
  update();
}