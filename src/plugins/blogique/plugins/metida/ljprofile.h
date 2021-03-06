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

#pragma once

#include <memory>
#include <QObject>
#include <QSet>
#include <interfaces/blogique/iprofile.h>
#include "profiletypes.h"
#include "ljfriendentry.h"

class QNetworkReply;

namespace LeechCraft
{
namespace Blogique
{
namespace Metida
{
	class LJFriendEntry;
	class ProfileWidget;

	class LJProfile : public QObject
					, public IProfile
	{
		Q_OBJECT
		Q_INTERFACES (LeechCraft::Blogique::IProfile)

		QObject *ParentAccount_;
		LJProfileData ProfileData_;
		QHash<QNetworkReply*, QString> Reply2AvatarId_;
	public:
		LJProfile (QObject *parentAccount, QObject *parent = 0);

		QWidget* GetProfileWidget ();
		QList<QPair<QIcon, QString>> GetPostingTargets () const;

		LJProfileData GetProfileData () const;
		QObject* GetParentAccount () const;

		void AddFriends (const QList<LJFriendEntry_ptr>& friends);
		QList<LJFriendEntry_ptr> GetFriends () const;

		QList<LJFriendGroup> GetFriendGroups () const;

		int GetFreeGroupId () const;

	private:
		void SaveAvatar (QUrl url = QUrl ());
		void SaveOthersAvatars (const QString& id, const QUrl& url);

	public slots:
		void handleProfileUpdate (const LJProfileData& profile);
	private slots:
		void handleAvatarDownloadFinished ();
		void handleOtherAvatarDownloadFinished ();

	signals:
		void profileUpdated ();
	};
}
}
}
