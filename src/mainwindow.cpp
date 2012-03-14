/*
 * This file is part of ktp-ssh-contact
 *
 * Copyright (C) 2011 David Edmundson <kde@davidedmundson.co.uk>
 * Copyright (C) 2012 Daniele E. Domenichelli <daniele.domenichelli@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

// #include <KApplication>
// #include <KMessageBox>
#include <KDE/KLineEdit>
#include <KDE/KDebug>
#include <KDE/KProcess>
#include <KDE/KUser>

#include <QtGui/QPushButton>

#include <TelepathyQt/AccountManager>
#include <TelepathyQt/PendingChannelRequest>
#include <TelepathyQt/PendingReady>

#include <KTp/debug.h>
#include <KTp/Models/accounts-model.h>
#include <KTp/Models/accounts-filter-model.h>
#include <KTp/Widgets/contact-grid-widget.h>


MainWindow::MainWindow(const QString &account,
                       const QString &contact,
                       const QString &login,
                       const QString &sshContactPath,
                       const QString &terminalPath,
                       const QString &terminalArgs,
                       QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    m_accountsModel(0),
    m_contactGridWidget(0),
    m_sshContactPath(sshContactPath),
    m_terminalPath(terminalPath),
    m_terminalArgs(terminalArgs)
{
    // TODO select account/contact
    Q_UNUSED(account)
    Q_UNUSED(contact)

    ui->setupUi(this);
    setWindowTitle(i18n("KTp SSH Contact"));

    if (!login.isEmpty()) {
        ui->loginLineEdit->setText(login);
    } else {
        ui->loginLineEdit->setText(KUser().loginName());
    }

    Tp::registerTypes();
    KTp::Debug::installCallback(true);

    Tp::AccountFactoryPtr  accountFactory = Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                       Tp::Features() << Tp::Account::FeatureCore
                                                                                      << Tp::Account::FeatureAvatar
                                                                                      << Tp::Account::FeatureProtocolInfo
                                                                                      << Tp::Account::FeatureProfile);

    Tp::ConnectionFactoryPtr connectionFactory = Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                               Tp::Features() << Tp::Connection::FeatureCore
                                                                                              << Tp::Connection::FeatureRosterGroups
                                                                                              << Tp::Connection::FeatureRoster
                                                                                              << Tp::Connection::FeatureSelfContact);

    Tp::ContactFactoryPtr contactFactory = Tp::ContactFactory::create(Tp::Features()  << Tp::Contact::FeatureAlias
                                                                                      << Tp::Contact::FeatureAvatarData
                                                                                      << Tp::Contact::FeatureSimplePresence
                                                                                      << Tp::Contact::FeatureCapabilities);

    Tp::ChannelFactoryPtr channelFactory = Tp::ChannelFactory::create(QDBusConnection::sessionBus());

    m_accountManager = Tp::AccountManager::create(QDBusConnection::sessionBus(),
                                                  accountFactory,
                                                  connectionFactory,
                                                  channelFactory,
                                                  contactFactory);

    m_accountsModel = new AccountsModel(this);
    connect(m_accountManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation*)), SLOT(onAccountManagerReady()));


    m_contactGridWidget = new KTp::ContactGridWidget(m_accountsModel, this);
    m_contactGridWidget->contactFilterLineEdit()->setClickMessage(i18n("Search in Contacts..."));
    m_contactGridWidget->filter()->setPresenceTypeFilterFlags(AccountsFilterModel::ShowOnlyConnected);
    m_contactGridWidget->filter()->setCapabilityFilterFlags(AccountsFilterModel::FilterBySSHContactCapability);
    ui->contactVLayout->addWidget(m_contactGridWidget);

    connect(m_contactGridWidget,
            SIGNAL(selectionChanged(Tp::AccountPtr,Tp::ContactPtr)),
            SLOT(onChanged()));
    connect(ui->loginLineEdit,
            SIGNAL(textChanged(QString)),
            SLOT(onChanged()));

    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onDialogAccepted()));
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAccountManagerReady()
{
    m_accountsModel->setAccountManager(m_accountManager);
}

void MainWindow::onDialogAccepted()
{
    // don't do anytghing if no contact has been selected
    if (!m_contactGridWidget->hasSelection()) {
        // show message box?
        return;
    }

    Tp::ContactPtr contact = m_contactGridWidget->selectedContact();
    Tp::AccountPtr sendingAccount = m_contactGridWidget->selectedAccount();

    if (sendingAccount.isNull()) {
        kDebug() << "sending account: NULL";
    } else {
        kDebug() << "Account is: " << sendingAccount->displayName();
        kDebug() << "sending to: " << contact->alias();
    }

    MainWindow::startProcess(sendingAccount->objectPath(),
                             contact->id(),
                             ui->loginLineEdit->text(),
                             m_sshContactPath,
                             m_terminalPath,
                             m_terminalArgs);

}

void MainWindow::onChanged()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_contactGridWidget->hasSelection() &&
                                                            !ui->loginLineEdit->text().isEmpty());
}

void MainWindow::startProcess(const QString &account,
                        const QString &contact,
                        const QString &login,
                        const QString &sshContactPath,
                        const QString &terminalPath,
                        const QString &terminalArgs)
{
    QStringList cmdargs = terminalArgs.split(QLatin1Char(' '));
    cmdargs << sshContactPath
            << QLatin1String("--account") << account
            << QLatin1String("--contact") << contact
            << QLatin1String("--login") << login;

    KProcess termProcess;
    termProcess.setProgram(terminalPath, cmdargs);
    kDebug() << "Running:" << terminalPath << "with args" << cmdargs.join(QLatin1String(" "));
    termProcess.startDetached();
    QApplication::quit();

}
