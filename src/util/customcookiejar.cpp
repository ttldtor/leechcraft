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

#include "customcookiejar.h"
#include <QNetworkCookie>
#include <QtDebug>

using namespace LeechCraft::Util;

CustomCookieJar::CustomCookieJar (QObject *parent)
: QNetworkCookieJar (parent)
, FilterTrackingCookies_ (false)
{
}

CustomCookieJar::~CustomCookieJar ()
{
}

void CustomCookieJar::SetFilterTrackingCookies (bool filter)
{
	FilterTrackingCookies_ = filter;
}

QByteArray CustomCookieJar::Save () const
{
	QList<QNetworkCookie> cookies = allCookies ();
	QByteArray result;
	for (QList<QNetworkCookie>::const_iterator i = cookies.begin (),
			end = cookies.end (); i != end; ++i)
	{
		result += i->toRawForm ();
		result += "\n";
	}
	return result;
}

void CustomCookieJar::Load (const QByteArray& data)
{
	QList<QByteArray> spcookies = data.split ('\n');
	QList<QNetworkCookie> cookies, filteredCookies;
	for (QList<QByteArray>::const_iterator i = spcookies.begin (),
			end = spcookies.end (); i != end; ++i)
		cookies += QNetworkCookie::parseCookies (*i);
	Q_FOREACH (QNetworkCookie cookie, cookies)
		if (!(FilterTrackingCookies_ &&
					cookie.name ().startsWith ("__utm")))
			filteredCookies << cookie;
	setAllCookies (filteredCookies);
}

void CustomCookieJar::CollectGarbage ()
{
	QList<QNetworkCookie> cookies = allCookies ();
	QList<QNetworkCookie> result;
	Q_FOREACH (QNetworkCookie cookie, cookies)
		if (!result.contains (cookie))
			result << cookie;
	qDebug () << Q_FUNC_INFO << cookies.size () << result.size ();
	setAllCookies (result);
}

QList<QNetworkCookie> CustomCookieJar::cookiesForUrl (const QUrl& url) const
{
	QList<QNetworkCookie> result = QNetworkCookieJar::cookiesForUrl (url);
	QList<QNetworkCookie> filtered;
	Q_FOREACH (QNetworkCookie cookie, result)
		if (!filtered.contains (cookie))
			filtered << cookie;
	return filtered;
}

