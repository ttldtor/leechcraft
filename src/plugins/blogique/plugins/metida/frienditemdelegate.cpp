/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2010-2012  Oleg Linkin
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 **********************************************************************/

#include "frienditemdelegate.h"
#include <QApplication>
#include <QPainter>
#include <QTreeView>
#include <QtDebug>
#include "xmlsettingsmanager.h"

namespace LeechCraft
{
namespace Blogique
{
namespace Metida
{
	FriendItemDelegate::FriendItemDelegate (QTreeView *parent)
	: QStyledItemDelegate (parent)
	, ColoringItems_ (true)
	, View_ (parent)
	{
		XmlSettingsManager::Instance ().RegisterObject ("ColoringFriendsList",
				this, "handleColoringItemChanged");
		handleColoringItemChanged ();
	}

	void FriendItemDelegate::paint (QPainter *painter,
			const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyleOptionViewItemV4 o = option;
		const QRect& r = o.rect;
		const QString& backgroundColor = index.sibling (index.row (), Columns::UserName)
				.data (ItemColorRoles::BackgroundColor).toString ();
		const QString& foregroundColor = index.sibling (index.row (), Columns::UserName)
				.data (ItemColorRoles::ForegroundColor).toString ();

		if (index.parent ().isValid () &&
				ColoringItems_)
		{
			if (!backgroundColor.isEmpty ())
				painter->fillRect (o.rect, QColor (backgroundColor));
			if (!foregroundColor.isEmpty ())
				o.palette.setColor (QPalette::Text, QColor (foregroundColor));
		}

		QStyledItemDelegate::paint (painter, o, index);

		painter->save ();
		if (!index.parent ().isValid () &&
				index.column () == Columns::UserName)
		{
			const int textWidth = o.fontMetrics.width (index.data ().value<QString> () + " ");
			const int rem = r.width () - textWidth;

			const QString& str = QString (" (%1) ")
					.arg (index.model ()->rowCount (index));

			if (o.state & QStyle::State_Selected)
				painter->setPen (o.palette.color (QPalette::HighlightedText));

			const QRect numRect (r.left () + textWidth - 1, r.top () + CPadding,
					rem - 1, r.height () - 2 * CPadding);
			painter->drawText (numRect, Qt::AlignVCenter | Qt::AlignLeft, str);
		}
		painter->restore ();
	}

	void FriendItemDelegate::handleColoringItemChanged ()
	{
		ColoringItems_ = XmlSettingsManager::Instance ()
				.Property ("ColoringFriendsList", true).toBool ();

		View_->viewport ()->update ();
		View_->update ();
	}

}
}
}
