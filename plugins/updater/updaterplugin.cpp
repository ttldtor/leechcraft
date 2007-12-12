#include <QtGui>
#include <settingsdialog/settingsdialog.h>
#include <plugininterface/proxy.h>
#include "updaterplugin.h"
#include "settingsmanager.h"
#include "globals.h"
#include "core.h"

void UpdaterPlugin::Init ()
{
	Core_ = new Core;
	connect (Core_, SIGNAL (gotFile (const QString&, const QString&, ulong, const QString&)), this, SLOT (addFile (const QString&, const QString&, ulong, const QString)));
	connect (Core_, SIGNAL (error (const QString&)), this, SLOT (handleError (const QString&)));
	connect (Core_, SIGNAL (finishedLoop ()), this, SLOT (setActionsEnabled ()));
	connect (Core_, SIGNAL (finishedCheck ()), this, SLOT (handleFinishedCheck ()));
	Core_->start (QThread::LowestPriority);
	IsShown_ = false;
	SaveChangesScheduled_ = false;

	SettingsDialog_ = new SettingsDialog ();
	SettingsDialog_->RegisterObject (SettingsManager::Instance ());

	SetupInterface ();
	ReadSettings ();
	setActionsEnabled ();
	statusBar ()->showMessage (tr ("Idle"));
	connect (Updates_, SIGNAL (itemClicked (QTreeWidgetItem*, int)), this, SLOT (setActionsEnabled ()));
	connect (Updates_, SIGNAL (itemClicked (QTreeWidgetItem*, int)), this, SLOT (updateStatusbar ()));
}

UpdaterPlugin::~UpdaterPlugin ()
{
}

QString UpdaterPlugin::GetName () const
{
	return Globals::Name;
}

QString UpdaterPlugin::GetInfo () const
{
	return tr ("Simple updater");
}

QString UpdaterPlugin::GetStatusbarMessage () const
{
	return tr ("");
}

IInfo& UpdaterPlugin::SetID (IInfo::ID_t id)
{
	ID_ = id;
	return *this;
}

IInfo::ID_t UpdaterPlugin::GetID () const
{
	return ID_;
}

QStringList UpdaterPlugin::Provides () const
{
	return QStringList ("update");
}

QStringList UpdaterPlugin::Needs () const
{
	return (QStringList ("http") << "ftp");
}

QStringList UpdaterPlugin::Uses () const
{
	return QStringList ();
}

void UpdaterPlugin::SetProvider (QObject* provider, const QString& feature)
{
	Core_->SetProvider (provider, feature);
}

void UpdaterPlugin::Release ()
{
	saveSettings ();
	Core_->Release ();
	delete SettingsDialog_;
	SettingsDialog_ = 0;

	while (!Core_->wait (25))
		qApp->processEvents ();

	delete Core_;
	Core_ = 0;
}

QIcon UpdaterPlugin::GetIcon () const
{
	return QIcon (":/resources/images/updater.png");
}

void UpdaterPlugin::SetParent (QWidget *parent)
{
	setParent (parent);
}

void UpdaterPlugin::ShowWindow ()
{
	IsShown_ = 1 - IsShown_;
	IsShown_ ? show () : hide ();
}

void UpdaterPlugin::ShowBalloonTip ()
{
}

uint UpdaterPlugin::GetVersion () const
{
	return QDateTime (QDate (2007, 11, 30), QTime (11, 11)).toTime_t ();
}

void UpdaterPlugin::SetupInterface ()
{
	setWindowTitle (tr ("Updater"));
	setWindowIcon (GetIcon ());

	SetupMainWidget ();
	SetupToolbars ();
	SetupActions ();
	SetupMenus ();

	SizeLabel_ = new QLabel (this);
	SizeLabel_->setMinimumWidth (SizeLabel_->fontMetrics ().width ("9999 bytes"));
	statusBar ()->addPermanentWidget (SizeLabel_);
	updateStatusbar ();
}

void UpdaterPlugin::SetupMainWidget ()
{
	Updates_ = new QTreeWidget (this);
	setCentralWidget (Updates_);
	Updates_->setHeaderLabels (QStringList (tr ("Plugin")) << tr ("Size") << tr ("Location"));
	Updates_->setEditTriggers (QAbstractItemView::NoEditTriggers);
	Updates_->setSelectionMode (QAbstractItemView::NoSelection);
	Updates_->setAlternatingRowColors (true);
	Updates_->header ()->setStretchLastSection (true);
	Updates_->header ()->setHighlightSections (false);
	Updates_->header ()->setDefaultAlignment (Qt::AlignLeft);
}

void UpdaterPlugin::SetupToolbars ()
{
	MainToolbar_ = addToolBar (tr ("Main"));
	MainToolbar_->setIconSize (QSize (16, 16));
}

void UpdaterPlugin::SetupActions ()
{
	CheckForUpdates_ = MainToolbar_->addAction (QIcon (":/resources/images/check.png"), tr ("Check for updates"), this, SLOT (initCheckForUpdates ()));
	DownloadUpdates_ = MainToolbar_->addAction (QIcon (":/resources/images/download.png"), tr ("Download updates"), this, SLOT (initDownloadUpdates ()));
	MainToolbar_->addSeparator ();
	Settings_ = MainToolbar_->addAction (QIcon (":/resources/images/preferences.png"), tr ("Settings..."), this, SLOT (showSettings ()));
}

void UpdaterPlugin::SetupMenus ()
{
	QMenu *tools = menuBar ()->addMenu (tr ("&Tools"));
	tools->addAction (CheckForUpdates_);
	tools->addSeparator ();
	tools->addAction (Settings_);
}

void UpdaterPlugin::ReadSettings ()
{
    QSettings settings (Proxy::Instance ()->GetOrganizationName (), Proxy::Instance ()->GetApplicationName ());
    settings.beginGroup (GetName ());
    settings.beginGroup ("geometry");
    resize (settings.value ("size", QSize (400, 300)).toSize ());
    move   (settings.value ("pos",  QPoint (10, 10)).toPoint ());
	settings.endGroup ();
	settings.endGroup ();
}

void UpdaterPlugin::saveSettings ()
{
	SaveChangesScheduled_ = false;
    QSettings settings (Proxy::Instance ()->GetOrganizationName (), Proxy::Instance ()->GetApplicationName ());
    settings.beginGroup (GetName ());
    settings.beginGroup ("geometry");
    settings.setValue ("size", size ());
    settings.setValue ("pos", pos ());
	settings.endGroup ();
	settings.endGroup ();
}

void UpdaterPlugin::showSettings ()
{
	SettingsDialog_->show ();
	SettingsDialog_->setWindowTitle (windowTitle () + tr (": Preferences"));
}

void UpdaterPlugin::initCheckForUpdates ()
{
	statusBar ()->showMessage ("Checking for updates...");
	Updates_->clear ();
	Core_->checkForUpdates ();
	QTimer::singleShot (2, this, SLOT (setActionsEnabled ()));
}

void UpdaterPlugin::initDownloadUpdates ()
{
	statusBar ()->showMessage ("Downloading updates...");
	Core_->downloadUpdates ();
	QTimer::singleShot (2, this, SLOT (setActionsEnabled ()));
}

void UpdaterPlugin::addFile (const QString& name, const QString& loc, ulong size, const QString& descr)
{
	QTreeWidgetItem *item = new QTreeWidgetItem (Updates_);
	item->setCheckState (ColumnName, Qt::Unchecked);
	item->setText (ColumnName, name);
	item->setText (ColumnLocation, loc);
	item->setText (ColumnSize, Proxy::Instance ()->MakePrettySize (size));
	item->setData (ColumnSize, RoleSize, static_cast<qulonglong> (size));

	QTreeWidgetItem *descItem = new QTreeWidgetItem (item);
	descItem->setFirstColumnSpanned (true);
	descItem->setText (0, descr);
}

void UpdaterPlugin::handleError (const QString& error)
{
	qDebug () << error;
	QMessageBox::warning (this, tr ("Error!"), error);
}

void UpdaterPlugin::setActionsEnabled ()
{
	DownloadUpdates_->setEnabled (false);
	if (!Core_->IsDownloading ())
	{
		for (int i = 0; i < Updates_->topLevelItemCount (); ++i)
			if (Updates_->topLevelItem (i)->checkState (ColumnName) == Qt::Checked)
			{
				DownloadUpdates_->setEnabled (true);
				break;
			}
	}

	CheckForUpdates_->setEnabled (!Core_->IsChecking ());
}

void UpdaterPlugin::handleFinishedCheck ()
{
	Updates_->sortByColumn (ColumnName, Qt::AscendingOrder);
	statusBar ()->showMessage (tr ("Checked successfully"));
}

void UpdaterPlugin::updateStatusbar ()
{
	ulong result = 0;
	for (int i = 0; i < Updates_->topLevelItemCount (); ++i)
		if (Updates_->topLevelItem (i)->checkState (ColumnName) == Qt::Checked)
			result += Updates_->topLevelItem (i)->data (ColumnSize, RoleSize).toULongLong ();

	SizeLabel_->setText (Proxy::Instance ()->MakePrettySize (result));
}

Q_EXPORT_PLUGIN2 (leechcraft_updater, UpdaterPlugin);

