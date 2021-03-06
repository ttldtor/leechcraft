/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2006-2013  Georg Rudoy
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

#include "riexmanager.h"
#include <QDomElement>
#include <QXmppClient.h>
#include <QXmppMessage.h>
#include "core.h"
#include "capsdatabase.h"
#include "entrybase.h"

namespace LeechCraft
{
namespace Azoth
{
namespace Xoox
{
	const QString NsRIEX = "http://jabber.org/protocol/rosterx";

	RIEXManager::Item::Item ()
	: Action_ (AAdd)
	{
	}

	RIEXManager::Item::Item (RIEXManager::Item::Action action,
			QString jid, QString name, QStringList groups)
	: Action_ (action)
	, JID_ (jid)
	, Name_ (name)
	, Groups_ (groups)
	{
	}

	RIEXManager::Item::Action RIEXManager::Item::GetAction () const
	{
		return Action_;
	}

	void RIEXManager::Item::SetAction (RIEXManager::Item::Action action)
	{
		Action_ = action;
	}

	QString RIEXManager::Item::GetJID () const
	{
		return JID_;
	}

	void RIEXManager::Item::SetJID (QString jid)
	{
		JID_ = jid;
	}

	QString RIEXManager::Item::GetName () const
	{
		return Name_;
	}

	void RIEXManager::Item::SetName (QString name)
	{
		Name_ = name;
	}

	QStringList RIEXManager::Item::GetGroups () const
	{
		return Groups_;
	}

	void RIEXManager::Item::SetGroups (QStringList groups)
	{
		Groups_ = groups;
	}

	QStringList RIEXManager::discoveryFeatures () const
	{
		return QStringList (NsRIEX);
	}

	bool RIEXManager::handleStanza (const QDomElement& elem)
	{
		if ((elem.tagName () != "message" && elem.tagName () != "iq") ||
				elem.attribute ("from").isEmpty ())
			return false;

		const QDomElement& x = elem.firstChildElement ("x");
		if (x.namespaceURI () != NsRIEX)
			return false;

		const bool isIq = elem.tagName () == "iq";

		QList<Item> items;

		QDomElement item = x.firstChildElement ("item");
		while (!item.isNull ())
		{
			QStringList groups;

			QDomElement group = item.firstChildElement ("group");
			while (!group.isNull ())
			{
				groups << group.text ();
				group = group.nextSiblingElement ("group");
			}

			Item::Action act = Item::AAdd;
			const QString& actAttr = item.attribute ("action");
			if (actAttr == "modify")
				act = Item::AModify;
			else if (actAttr == "delete")
				act = Item::ADelete;

			items << Item (act, item.attribute ("jid"), item.attribute ("name"), groups);

			item = item.nextSiblingElement ("item");
		}

		emit gotItems (elem.attribute ("from"), items, !isIq);

		return isIq;
	}

	void RIEXManager::SuggestItems (EntryBase *to, QList<RIEXManager::Item> items, QString message)
	{
		QXmppElement x;
		x.setTagName ("x");
		x.setAttribute ("xmlns", NsRIEX);

		Q_FOREACH (const RIEXManager::Item& item, items)
		{
			QXmppElement itElem;
			itElem.setTagName ("item");

			if (!item.GetJID ().isEmpty ())
				itElem.setAttribute ("jid", item.GetJID ());

			if (!item.GetName ().isEmpty ())
				itElem.setAttribute ("name", item.GetName ());

			switch (item.GetAction ())
			{
			case Item::AModify:
				itElem.setAttribute ("action", "modify");
				break;
			case Item::ADelete:
				itElem.setAttribute ("action", "delete");
				break;
			default:
				itElem.setAttribute ("action", "add");
				break;
			}

			Q_FOREACH (const QString& group, item.GetGroups ())
			{
				QXmppElement groupElem;
				groupElem.setTagName ("group");
				groupElem.setValue (group);

				itElem.appendChild (groupElem);
			}

			x.appendChild (itElem);
		}

		QString suppRes;

		CapsDatabase *db = Core::Instance ().GetCapsDatabase ();
		Q_FOREACH (const QString& variant, to->Variants ())
		{
			const QByteArray& ver = to->GetVariantVerString (variant);
			const QStringList& features = db->Get (ver);
			if (features.contains (NsRIEX))
			{
				suppRes = variant;
				break;
			}
		}

		if (!suppRes.isEmpty ())
		{
			QXmppIq iq (QXmppIq::Set);
			iq.setTo (to->GetJID () + '/' + suppRes);
			iq.setExtensions (QXmppElementList () << x);
			client ()->sendPacket (iq);
		}
		else
		{
			QXmppMessage msg;
			msg.setTo (to->GetHumanReadableID ());
			msg.setBody (message);
			msg.setExtensions (QXmppElementList () << x);
			client ()->sendPacket (msg);
		}
	}
}
}
}
