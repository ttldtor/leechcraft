/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2006-2011  Georg Rudoy
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#ifndef PLUGINS_AZOTH_PLUGINS_ZHEET_MSNBUDDYENTRY_H
#define PLUGINS_AZOTH_PLUGINS_ZHEET_MSNBUDDYENTRY_H
#include <QObject>
#include <msn/buddy.h>
#include <interfaces/iclentry.h>

namespace MSN
{
	class Buddy;
}

namespace LeechCraft
{
namespace Azoth
{
namespace Zheet
{
	class MSNAccount;
	class MSNMessage;

	class MSNBuddyEntry : public QObject
						, public ICLEntry
	{
		Q_OBJECT
		Q_INTERFACES (LeechCraft::Azoth::ICLEntry);

		MSNAccount *Account_;

		MSN::Buddy Buddy_;
		QList<MSN::Group> Groups_;

		QList<MSNMessage*> AllMessages_;

		EntryStatus Status_;
	public:
		MSNBuddyEntry (const MSN::Buddy&, MSNAccount*);

		void HandleMessage (MSNMessage*);
		void UpdateState (State);

		// ICLEntry
		QObject* GetObject ();
		QObject* GetParentAccount () const;
		Features GetEntryFeatures () const;
		EntryType GetEntryType () const;
		QString GetEntryName () const;
		void SetEntryName (const QString& name);
		QString GetEntryID () const;
		QString GetHumanReadableID () const;
		QStringList Groups () const;
		void SetGroups (const QStringList& groups);
		QStringList Variants () const;
		QObject* CreateMessage (IMessage::MessageType, const QString&, const QString&);
		QList<QObject*> GetAllMessages () const;
		void PurgeMessages (const QDateTime&);
		void SetChatPartState (ChatPartState, const QString&);
		EntryStatus GetStatus (const QString&) const;
		QImage GetAvatar () const;
		QString GetRawInfo () const;
		void ShowInfo ();
		QList<QAction*> GetActions () const;
		QMap<QString, QVariant> GetClientInfo (const QString&) const;
		void MarkMsgsRead ();
	signals:
		void gotMessage (QObject*);
		void statusChanged (const EntryStatus&, const QString&);
		void availableVariantsChanged (const QStringList&);
		void avatarChanged (const QImage&);
		void rawinfoChanged (const QString&);
		void nameChanged (const QString&);
		void groupsChanged (const QStringList&);
		void chatPartStateChanged (const ChatPartState&, const QString&);
		void permsChanged ();
		void entryGenerallyChanged ();
	};
}
}
}

#endif
