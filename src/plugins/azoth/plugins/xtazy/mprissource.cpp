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

#include "mprissource.h"
#include <QDBusConnectionInterface>
#include <QDBusMetaType>
#include <QtDebug>

namespace LeechCraft
{
namespace Azoth
{
namespace Xtazy
{
	const QString MPRISPrefix = "org.mpris";

	enum MPRISVersion
	{
		MV1 = 1,
		MV2
	};

	namespace
	{
		MPRISVersion GetVersion (const QString& service)
		{
			return service.contains ("MediaPlayer2") ?
					MV1 :
					MV2;
		}
	}

	enum PlayStatus
	{
		PSPlaying = 0,
		PSPaused,
		PSStopped
	};

	namespace
	{
		PlayStatus GetMPRIS2PlayStatus (const QString& status)
		{
			if (status == "Playing")
				return PSPlaying;
			else if (status == "Paused")
				return PSPaused;
			else
				return PSStopped;
		}
	}

	QDBusArgument& operator<< (QDBusArgument& arg, const PlayerStatus& ps)
	{
		arg.beginStructure ();
		arg << ps.PlayStatus_
			<< ps.PlayOrder_
			<< ps.PlayRepeat_
			<< ps.StopOnce_;
		arg.endStructure ();
		return arg;
	}

	const QDBusArgument& operator>> (const QDBusArgument& arg, PlayerStatus& ps)
	{
		arg.beginStructure ();
		arg >> ps.PlayStatus_
			>> ps.PlayOrder_
			>> ps.PlayRepeat_
			>> ps.StopOnce_;
		arg.endStructure ();
		return arg;
	}

	MPRISSource::MPRISSource (QObject *parent)
	: TuneSourceBase (parent)
	, SB_ (QDBusConnection::sessionBus ())
	{
		setObjectName ("MPRISSource");

		qDBusRegisterMetaType<PlayerStatus> ();

		Players_ = SB_.interface ()->registeredServiceNames ()
				.value ().filter (MPRISPrefix);

		Q_FOREACH (const QString& player, Players_)
			ConnectToBus (player);

		SB_.connect ("org.freedesktop.DBus",
				"/org/freedesktop/DBus",
				"org.freedesktop.DBus",
				"NameOwnerChanged",
				this,
				SLOT (checkMPRISService (QString, QString, QString)));
	}

	MPRISSource::~MPRISSource ()
	{
		Q_FOREACH (const QString& player, Players_)
			DisconnectFromBus (player);

		SB_.disconnect ("org.freedesktop.DBus",
				"/org/freedesktop/DBus",
				"org.freedesktop.DBus",
				"NameOwnerChanged",
				this,
				SLOT (checkMPRISService (QString, QString, QString)));
	}

	void MPRISSource::ConnectToBus (const QString& service)
	{
		switch (GetVersion (service))
		{
			case MV1:
				SB_.connect (service,
						"/Player",
						"org.freedesktop.MediaPlayer",
						"StatusChange",
						"(iiii)",
						this,
						SLOT (handlePlayerStatusChange (PlayerStatus)));
				SB_.connect (service,
						"/Player",
						"org.freedesktop.MediaPlayer",
						"TrackChange",
						"a{sv}",
						this,
						SLOT (handleTrackChange (QVariantMap)));
			case MV2:
				SB_.connect (service,
						"/org/mpris/MediaPlayer2",
						"org.freedesktop.DBus.Properties",
						"PropertiesChanged",
						this,
						SLOT (handlePropertyChange (QDBusMessage)));
		}
	}

	void MPRISSource::DisconnectFromBus (const QString& service)
	{
		switch (GetVersion (service))
		{
			case MV1:
				SB_.disconnect (service,
						"/Player",
						"org.freedesktop.MediaPlayer",
						"StatusChange",
						"(iiii)",
						this,
						SLOT (handlePlayerStatusChange (PlayerStatus)));
				SB_.disconnect (service,
						"/Player",
						"org.freedesktop.MediaPlayer",
						"TrackChange",
						"a{sv}",
						this,
						SLOT (handleTrackChange (QVariantMap)));
			case MV2:
				SB_.disconnect (service,
						"/org/mpris/MediaPlayer2",
						"org.freedesktop.DBus.Properties",
						"PropertiesChanged",
						this,
						SLOT (handlePropertyChange (QDBusMessage)));
		}
	}

	TuneSourceBase::TuneInfo_t MPRISSource::GetTuneMV2 (const QVariantMap& map)
	{
		TuneInfo_t result;
		if (map.contains ("xesam:title"))
			result ["title"] = map ["xesam:title"];
		if (map.contains ("xesam:artist"))
			result ["artist"] = map ["xesam:artist"];
		if (map.contains ("xesam:album"))
			result ["source"] = map ["xesam:album"];
		if (map.contains ("xesam:trackNumber"))
			result ["track"] = map ["xesam:trackNumber"];
		if (map.contains ("xesam:length"))
			result ["length"] = map ["xesam:length"].toLongLong () / 1000000;
		return result;
	}

	void MPRISSource::handlePropertyChange (const QDBusMessage& msg)
	{
		QDBusArgument arg = msg.arguments ().at (1).value<QDBusArgument> ();
		const QVariantMap& map = qdbus_cast<QVariantMap> (arg);

		QVariant v = map.value ("Metadata");
		if (v.isValid ())
		{
			arg = v.value<QDBusArgument> ();
			TuneInfo_t tune = GetTuneMV2 (qdbus_cast<QVariantMap> (arg));
			if (tune != Tune_)
			{
				Tune_ = tune;
				if (!Tune_.isEmpty ())
					emit tuneInfoChanged (Tune_);
			}
		}

		v = map.value ("PlaybackStatus");
		if (v.isValid ())
		{
			PlayerStatus status;
			status.PlayStatus_ = GetMPRIS2PlayStatus (v.toString ());
			handlePlayerStatusChange (status);
		}
	}

	void MPRISSource::handlePlayerStatusChange (PlayerStatus ps)
	{
		if (ps.PlayStatus_ != PSPlaying)
		{
			emit tuneInfoChanged (TuneInfo_t ());
			if (ps.PlayStatus_ == PSStopped)
				Tune_ = TuneInfo_t ();
		}
		else if (!Tune_.isEmpty ())
			emit tuneInfoChanged (Tune_);
	}

	void MPRISSource::handleTrackChange (const QVariantMap& map)
	{
		TuneInfo_t tune = map;
		if (tune.contains ("album"))
			tune ["source"] = tune.take ("album");
		if (tune.contains ("time"))
			tune ["length"] = tune.take ("time");

		if (tune == Tune_)
			return;

		Tune_ = tune;
		if (!Tune_.isEmpty ())
			emit tuneInfoChanged (Tune_);
	}

	void MPRISSource::checkMPRISService (QString name,
			QString, QString newOwner)
	{
		if (!name.startsWith (MPRISPrefix))
			return;

		if (name.contains ("LMP_"))
			return;

		const int playerIdx = Players_.indexOf (name);
		if (playerIdx == -1)
		{
			if (!newOwner.isEmpty ())
			{
				Players_ << name;
				ConnectToBus (name);
			}
		}
		else if (newOwner.isEmpty ())
		{
			DisconnectFromBus (name);
			Players_.removeAt (playerIdx);
		}
	}
}
}
}
