/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2012  Georg Rudoy
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

#include "streamlistfetcherbase.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QStandardItem>
#include <interfaces/media/iradiostationprovider.h>
#include "roles.h"

namespace LeechCraft
{
namespace HotStreams
{
	StreamListFetcherBase::StreamListFetcherBase (QStandardItem *root, QNetworkAccessManager *nam, QObject *parent)
	: QObject (parent)
	, NAM_ (nam)
	, Root_ (root)
	, RadioIcon_ (":/hotstreams/resources/images/radio.png")
	{
	}

	void StreamListFetcherBase::Request (const QNetworkRequest& req)
	{
		auto reply = NAM_->get (req);
		connect (reply,
				SIGNAL (finished ()),
				this,
				SLOT (handleReplyFinished ()));
	}

	void StreamListFetcherBase::HandleData (const QByteArray& data)
	{
		auto result = Parse (data);
		std::sort (result.begin (), result.end (),
				[] (decltype (result.at (0)) left, decltype (result.at (0)) right)
					{ return QString::localeAwareCompare (left.Name_, right.Name_) < 0; });
		for (const auto& stream : result)
		{
			auto name = stream.Name_;
			if (!stream.Genres_.isEmpty ())
				name += " (" + stream.Genres_.join ("; ") + ")";

			auto tooltip = "<span style=\"white-space: nowrap\">" + stream.Description_;
			if (!stream.DJ_.isEmpty ())
				tooltip += "<br /><em>DJ:</em> " + stream.DJ_;
			tooltip += "</span>";

			auto item = new QStandardItem (name);
			item->setToolTip (tooltip);
			item->setIcon (RadioIcon_);
			item->setData (stream.Name_, StreamItemRoles::PristineName);
			item->setData (Media::RadioType::Predefined, Media::RadioItemRole::ItemType);
			item->setData (stream.URL_, Media::RadioItemRole::RadioID);
			item->setData (stream.PlaylistFormat_, StreamItemRoles::PlaylistFormat);
			item->setEditable (false);
			Root_->appendRow (item);
		}

		deleteLater ();
	}

	void StreamListFetcherBase::handleReplyFinished ()
	{
		auto reply = qobject_cast<QNetworkReply*> (sender ());
		if (!reply)
			return;

		reply->deleteLater ();
		HandleData (reply->readAll ());
	}
}
}
