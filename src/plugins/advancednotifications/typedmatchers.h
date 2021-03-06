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

#ifndef PLUGINS_ADVANCEDNOTIFICATIONS_TYPEDMATCHERS_H
#define PLUGINS_ADVANCEDNOTIFICATIONS_TYPEDMATCHERS_H
#include <memory>
#include <QString>
#include <QRegExp>
#include <QVariant>

namespace Ui
{
	class IntMatcherConfigWidget;
	class StringLikeMatcherConfigWidget;
}

namespace LeechCraft
{
namespace AdvancedNotifications
{
	class TypedMatcherBase;

	typedef std::shared_ptr<TypedMatcherBase> TypedMatcherBase_ptr;

	class TypedMatcherBase
	{
	protected:
		QWidget *CW_;
	public:
		static TypedMatcherBase_ptr Create (QVariant::Type);

		virtual QVariantMap Save () const = 0;
		virtual void Load (const QVariantMap&) = 0;

		virtual bool Match (const QVariant&) const = 0;

		virtual QString GetHRDescription () const = 0;
		virtual QWidget* GetConfigWidget () = 0;
		virtual void SyncToWidget () = 0;
	};

	class StringLikeMatcher : public TypedMatcherBase
	{
	protected:
		QRegExp Rx_;
		bool Contains_;

		std::shared_ptr<Ui::StringLikeMatcherConfigWidget> Ui_;
	public:
		StringLikeMatcher ();

		QVariantMap Save () const;
		void Load (const QVariantMap&);

		QWidget* GetConfigWidget ();
		void SyncToWidget ();
	};

	class StringMatcher : public StringLikeMatcher
	{
	public:
		bool Match (const QVariant&) const;

		QString GetHRDescription () const;
	};

	class StringListMatcher : public StringLikeMatcher
	{
	public:
		bool Match (const QVariant&) const;

		QString GetHRDescription () const;
	};

	class IntMatcher : public TypedMatcherBase
	{
		int Boundary_;

		enum Operation
		{
			OGreater = 0x01,
			OLess = 0x02,
			OEqual = 0x04
		};

		Q_DECLARE_FLAGS (Operations, Operation)

		Operations Ops_;

		std::shared_ptr<Ui::IntMatcherConfigWidget> Ui_;
		QMap<Operations, int> Ops2pos_;
	public:
		IntMatcher ();

		QVariantMap Save () const;
		void Load (const QVariantMap&);

		bool Match (const QVariant&) const;

		QString GetHRDescription () const;
		QWidget* GetConfigWidget ();
		void SyncToWidget ();
	};
}
}

#endif
