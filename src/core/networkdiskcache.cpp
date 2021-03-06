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

#include "networkdiskcache.h"
#include <QtDebug>
#include <QDateTime>
#include <QDir>
#include <QtConcurrentRun>
#include <QFutureWatcher>
#include <QTimer>
#include <QDirIterator>
#include <QMutexLocker>
#include "xmlsettingsmanager.h"

namespace LeechCraft
{
	NetworkDiskCache::NetworkDiskCache (QObject *parent)
	: QNetworkDiskCache (parent)
	, IsCollectingGarbage_ (false)
	, PreviousSize_ (-1)
	{
		setCacheDirectory (QDir::homePath () + "/.leechcraft/core/cache");

		XmlSettingsManager::Instance ()->RegisterObject ("CacheSize",
				this, "handleCacheSize");
		handleCacheSize ();
	}

	QIODevice* NetworkDiskCache::prepare (const QNetworkCacheMetaData& metadata)
	{
		QMutexLocker lock (&InsertRemoveMutex_);
		return QNetworkDiskCache::prepare (metadata);
	}

	void NetworkDiskCache::insert (QIODevice *device)
	{
		QMutexLocker lock (&InsertRemoveMutex_);
		QNetworkDiskCache::insert (device);
	}

	bool NetworkDiskCache::remove (const QUrl &url)
	{
		QMutexLocker lock (&InsertRemoveMutex_);
		return QNetworkDiskCache::remove (url);
	}

	qint64 NetworkDiskCache::expire ()
	{
		collectGarbage ();
		if (PreviousSize_ >= 0)
			return PreviousSize_;
		else
			return maximumCacheSize () / 10 * 8;
	}

	void NetworkDiskCache::handleCacheSize ()
	{
		setMaximumCacheSize (XmlSettingsManager::Instance ()->
				property ("CacheSize").toInt () * 1048576);
		QTimer::singleShot (60000,
				this,
				SLOT (collectGarbage ()));
	}

	namespace
	{
		qint64 Collector (QString& cacheDirectory, qint64 goal)
		{
			if (cacheDirectory.isEmpty ())
				return 0;

			QDir::Filters filters = QDir::AllDirs | QDir:: Files | QDir::NoDotAndDotDot;
			QDirIterator it (cacheDirectory, filters, QDirIterator::Subdirectories);

			QMultiMap<QDateTime, QString> cacheItems;
			qint64 totalSize = 0;
			while (it.hasNext ())
			{
				QString path = it.next ();
				QFileInfo info = it.fileInfo ();
				QString fileName = info.fileName ();
				if (fileName.endsWith(".cache") &&
						fileName.startsWith("cache_"))
				{
					cacheItems.insert(info.created (), path);
					totalSize += info.size ();
				}
			}

			QMultiMap<QDateTime, QString>::const_iterator i = cacheItems.constBegin();
			while (i != cacheItems.constEnd())
			{
				if (totalSize < goal)
					break;
				QString name = i.value ();
				QFile file (name);
				qint64 size = file.size ();
				file.remove ();
				totalSize -= size;
				++i;
			}

			return totalSize;
		}
	};

	void NetworkDiskCache::collectGarbage ()
	{
		if (IsCollectingGarbage_)
			return;

		if (cacheDirectory ().isEmpty ())
			return;

		IsCollectingGarbage_ = true;

		QFutureWatcher<qint64> *watcher = new QFutureWatcher<qint64> (this);
		connect (watcher,
				SIGNAL (finished ()),
				this,
				SLOT (handleCollectorFinished ()));

		QFuture<qint64> future = QtConcurrent::run (Collector,
				cacheDirectory (), maximumCacheSize () * 9 / 10);
		watcher->setFuture (future);
	}

	void NetworkDiskCache::handleCollectorFinished ()
	{
		QFutureWatcher<qint64> *watcher = dynamic_cast<QFutureWatcher<qint64>*> (sender ());

		PreviousSize_ = watcher->result ();

		IsCollectingGarbage_ = false;
	}
};

