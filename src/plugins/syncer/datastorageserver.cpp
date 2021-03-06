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

#include "datastorageserver.h"
#include <QtDebug>
#include "serverchainhandler.h"

namespace LeechCraft
{
namespace Syncer
{
	DataStorageServer::DataStorageServer (QObject *parent)
	: DataStorageBase (parent)
	{
	}

	void DataStorageServer::sync (const QByteArray& chain)
	{
		// TODO correctly throw a correct exception
		if (ChainHandlers_.contains (chain))
			return;

		ServerChainHandler *handler = new ServerChainHandler (chain, this);

		connect (handler,
				SIGNAL (gotNewDeltas (const Sync::Deltas_t&, const QByteArray&)),
				this,
				SIGNAL (gotNewDeltas (const Sync::Deltas_t&, const QByteArray&)));
		connect (handler,
				SIGNAL (deltasRequired (Sync::Deltas_t*, const QByteArray&)),
				this,
				SIGNAL (deltasRequired (Sync::Deltas_t*, const QByteArray&)));
		connect (handler,
				SIGNAL (successfullySentDeltas (quint32, const QByteArray&)),
				this,
				SIGNAL (successfullySentDeltas (quint32, const QByteArray&)));

		connect (handler,
				SIGNAL (loginError ()),
				this,
				SLOT (handleLoginError ()));
		connect (handler,
				SIGNAL (connectionError ()),
				this,
				SLOT (handleConnectionError ()));
		connect (handler,
				SIGNAL (finishedSuccessfully (quint32, quint32)),
				this,
				SLOT (handleFinishedSuccessfully (quint32, quint32)));

		ChainHandlers_ [chain] = handler;
		handler->Sync ();
	}

	void DataStorageServer::handleLoginError ()
	{
		QByteArray chain = GetChainForSender (sender ());
		if (chain.isEmpty ())
		{
			qWarning () << Q_FUNC_INFO
					<< "^^^^^^^^^^^^";
			return;
		}

		emit loginError (chain);
		ChainHandlers_.take (chain)->deleteLater ();
	}

	void DataStorageServer::handleConnectionError ()
	{
		QByteArray chain = GetChainForSender (sender ());
		if (chain.isEmpty ())
		{
			qWarning () << Q_FUNC_INFO
					<< "^^^^^^^^^^^^";
			return;
		}

		emit connectionError (chain);
		ChainHandlers_.take (chain)->deleteLater ();
	}

	void DataStorageServer::handleFinishedSuccessfully (quint32 sent, quint32 received)
	{
		QByteArray chain = GetChainForSender (sender ());
		if (chain.isEmpty ())
		{
			qWarning () << Q_FUNC_INFO
					<< "^^^^^^^^^^^^";
			return;
		}

		emit finishedSuccessfully (sent, received, chain);
		ChainHandlers_.take (chain)->deleteLater ();
	}

	QByteArray DataStorageServer::GetChainForSender (QObject *sender)
	{
		ServerChainHandler *handler = qobject_cast<ServerChainHandler*> (sender);
		if (!handler)
		{
			qWarning () << "sender is not a ServerChainHandler"
					<< sender;
			return QByteArray ();
		}

		QByteArray chain = ChainHandlers_.key (handler);
		if (chain.isEmpty ())
		{
			qWarning () << "no chain for handler"
					<< handler;
			handler->deleteLater ();
		}
		return chain;
	}
}
}
