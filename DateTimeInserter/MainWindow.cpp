#include "pch.h"
#include "MainWindow.h"
#include "../qglobalshortcut/src/qglobalshortcut.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
  auto gs = new QGlobalShortcut(QKeySequence("Alt+F10"), this);
  connect(gs, &QGlobalShortcut::activated, this, [this]() {
    setVisible(!isVisible());
    });
}