#include "pch.h"
#include "MainWindow.h"
#include "../qglobalshortcut/src/qglobalshortcut.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
  auto gs = new QGlobalShortcut(QKeySequence("Alt+F10"), this);
  connect(gs, &QGlobalShortcut::activated, this, &MainWindow::insertDateTimeThroughClipboard);
}

void MainWindow::insertDateTimeThroughClipboard() {
	QString str = QDateTime::currentDateTime().toString("yyyy.MM.dd ddd hh:mm:ss");
	str.replace("Mon", u"Пн"_s);
	str.replace("Tue", u"Вт"_s);
	str.replace("Wed", u"Ср"_s);
	str.replace("Thu", u"Чт"_s);
	str.replace("Fri", u"Пт"_s);
	str.replace("Sat", u"Сб"_s);
	str.replace("Sun", u"Вс"_s);
	str = QString("\n=== %1 ").arg(str);
	qApp->clipboard()->setText(str);
}