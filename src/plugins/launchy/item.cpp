/**********************************************************************
 * LeechCraft - modular cross-platform feature rich internet client.
 * Copyright (C) 2006-2012  Georg Rudoy
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

#include "item.h"
#include <stdexcept>
#include <QFile>
#include <boost/graph/graph_concepts.hpp>
#include "fdodesktopparser.h"

namespace LeechCraft
{
namespace Launchy
{
	bool Item::IsValid () const
	{
		return !Name_.isEmpty ();
	}

	namespace
	{
		QString ByLang (const QHash<QString, QString>& cont, const QString& lang)
		{
			return cont.value (cont.contains (lang) ? lang : QString ());
		}
	}

	QString Item::GetName (const QString& lang) const
	{
		return ByLang (Name_, lang);
	}

	QString Item::GetGenericName (const QString& lang) const
	{
		return ByLang (GenericName_, lang);
	}

	QString Item::GetComment (const QString& lang) const
	{
		return ByLang (Comments_, lang);
	}

	QString Item::GetIconName () const
	{
		return IconName_;
	}

	QStringList Item::GetCategories () const
	{
		return Categories_;
	}

	void Item::SetIcon (const QIcon& icon)
	{
		Icon_ = icon;
	}

	QIcon Item::GetIcon () const
	{
		return Icon_;
	}

	namespace
	{
		QHash<QString, QString> FirstValues (const QHash<QString, QStringList>& hash)
		{
			QHash<QString, QString> result;
			for (auto i = hash.begin (), end = hash.end (); i != end; ++i)
				result [i.key ()] = i->value (0);
			return result;
		}
	}

	Item_ptr Item::FromDesktopFile (const QString& filename)
	{
		QFile file (filename);
		if (!file.open (QIODevice::ReadOnly))
			throw std::runtime_error ("Unable to open file");

		const auto& result = FDODesktopParser () (file.readAll ());
		const auto& group = result ["Desktop Entry"];

		Item_ptr item (new Item);
		item->Name_ = FirstValues (group ["Name"]);
		item->GenericName_ = FirstValues (group ["GenericName"]);
		item->Comments_ = FirstValues (group ["Comment"]);

		item->Categories_ = group ["Categories"] [QString ()];

		auto getSingle = [&group] (const QString& name) { return group [name] [QString ()].value (0); };

		item->Command_ = getSingle ("Exec");
		item->IconName_ = getSingle ("Icon");
		item->Type_ = getSingle ("Type");

		return item;
	}
}
}