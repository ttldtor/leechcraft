/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2006-2009  Georg Rudoy
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

#include "roompublicmessage.h"
#include <gloox/mucroom.h>
#include <gloox/delayeddelivery.h>
#include "roomclentry.h"

namespace LeechCraft
{
	namespace Plugins
	{
		namespace Azoth
		{
			namespace Plugins
			{
				namespace Xoox
				{
					RoomPublicMessage::RoomPublicMessage (const QString& msg, RoomCLEntry *entry)
					: QObject (entry)
					, ParentEntry_ (entry)
					, Message_ (msg)
					, Datetime_ (QDateTime::currentDateTime ())
					, Direction_ (DOut)
					{
					}

					RoomPublicMessage::RoomPublicMessage (const gloox::Message& msg, RoomCLEntry *entry)
					: QObject (entry)
					, ParentEntry_ (entry)
					, Message_ (QString::fromUtf8 (msg.body ().c_str ()))
					, Direction_ (DIn)
					, FromJID_ (msg.from ())
					{
						const gloox::DelayedDelivery *dd = msg.when ();
						Datetime_ = dd ?
								QDateTime::fromString (dd->stamp ().c_str (), Qt::ISODate) :
								QDateTime::currentDateTime ();
					}

					QObject* RoomPublicMessage::GetObject ()
					{
						return this;
					}

					void RoomPublicMessage::Send ()
					{
						ParentEntry_->GetRoom ()->
								send (Message_.toUtf8 ().constData ());
						Datetime_ = QDateTime::currentDateTime ();
					}

					IMessage::Direction RoomPublicMessage::GetDirection () const
					{
						return Direction_;
					}

					IMessage::MessageType RoomPublicMessage::GetMessageType () const
					{
						return MTChat;
					}

					ICLEntry* RoomPublicMessage::OtherPart () const
					{
						return ParentEntry_;
					}

					QString RoomPublicMessage::GetOtherVariant() const
					{
						return "";
					}

					QString RoomPublicMessage::GetBody () const
					{
						return Message_;
					}

					void RoomPublicMessage::SetBody (const QString& msg)
					{
						Message_ = msg;
					}

					QDateTime RoomPublicMessage::GetDateTime () const
					{
						return Datetime_;
					}

					void RoomPublicMessage::SetDateTime (const QDateTime& dt)
					{
						Datetime_ = dt;
					}
				}
			}
		}
	}
}
