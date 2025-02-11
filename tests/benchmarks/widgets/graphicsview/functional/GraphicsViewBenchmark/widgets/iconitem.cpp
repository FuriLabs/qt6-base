// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtGui>
#include <QSvgRenderer>
#include <QGraphicsEffect>

#include "iconitem.h"

IconItem::IconItem(const QString &filename, QGraphicsItem *parent)
  : GvbWidget(parent)
  , m_filename(filename)
  , m_rotation(0.0)
  , m_opacityEffect(0)
  , m_smoothTransformation(false)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setContentsMargins(0,0,0,0);
    setPreferredSize(58,58);
}

IconItem::~IconItem()
{
}

void IconItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    reload();

    const QPointF c = boundingRect().center();
    painter->translate(c.x(), c.y());
    painter->rotate(m_rotation);
    painter->translate(-c.x(), -c.y());

    if (m_smoothTransformation)
        painter->setRenderHints(QPainter::SmoothPixmapTransform);

    painter->drawPixmap(0,0, m_pixmap);
}

QSizeF IconItem::sizeHint(Qt::SizeHint which,
    const QSizeF &constraint) const
{
    switch (which)
    {
    case Qt::MinimumSize:
    case Qt::PreferredSize:
    case Qt::MaximumSize:
        return m_pixmap.size();

    default:
        return GvbWidget::sizeHint(which, constraint);
    }
}

void IconItem::reload()
{
    const QSize iconSize = size().toSize();
    if (iconSize.width() == 0 || iconSize.height()  == 0)
        return;

    const QString key = m_filename+QString::number(iconSize.width())+QString::number(iconSize.height());
    if (QPixmapCache::find(key, m_pixmap))
        return;

    if (m_filename.endsWith(".svg", Qt::CaseInsensitive))
    {
        m_pixmap = QPixmap(iconSize);
        m_pixmap.fill(Qt::transparent);
        QSvgRenderer doc(m_filename);
        QPainter painter(&m_pixmap);
        painter.setViewport(0, 0, iconSize.width(), iconSize.height());
        doc.render(&painter);
    }
    else
    {
        m_pixmap = QPixmap(m_filename).scaled(iconSize);
    }

    QPixmapCache::insert(key, m_pixmap);
    updateGeometry();
}

QString IconItem::fileName() const
{
    return m_filename;
}

void IconItem::setFileName(const QString &filename)
{
    if( m_filename != filename) {
        m_filename = filename;
        reload();
    }
}

void IconItem::setOpacityEffectEnabled(const bool enable)
{
    if (!m_opacityEffect)
    {
        QRadialGradient gradient(0.5, 0.5, 1.0);
        gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
        gradient.setColorAt(0.0, QColor(0,0,0, 255));
        gradient.setColorAt(0.46, QColor(0,0,0, 255));
        gradient.setColorAt(0.62, QColor(0,0,0, 0));

        m_opacityEffect = new QGraphicsOpacityEffect;
        m_opacityEffect->setOpacityMask(gradient);
        m_opacityEffect->setOpacity(1.0);
        this->setGraphicsEffect(m_opacityEffect);
    }
    m_opacityEffect->setEnabled(enable);
}

bool IconItem::isOpacityEffectEnabled() const
{
    if (m_opacityEffect)
        return m_opacityEffect->isEnabled();

    return false;
}
