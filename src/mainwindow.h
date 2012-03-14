/*
 * This file is part of ktp-ssh-contact
 *
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

#ifndef KTP_SSH_CONTACT_MAINWINDOW_H
#define KTP_SSH_CONTACT_MAINWINDOW_H

#include <QWidget>

#include <TelepathyQt/AccountManager>


namespace Ui { class MainWindow; }
namespace KTp { class ContactGridWidget; }
class AccountsModel;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &account,
                        const QString &contact,
                        const QString &login,
                        const QString &sshContactPath,
                        const QString &terminalPath,
                        const QString &terminalArgs,
                        QWidget *parent = 0);
    ~MainWindow();

    static void startProcess(const QString &account,
                             const QString &contact,
                             const QString &login,
                             const QString &sshContactPath,
                             const QString &terminalPath,
                             const QString &terminalArgs);

private Q_SLOTS:
    void onAccountManagerReady();
    void onDialogAccepted();
    void onChanged();

private:
    Ui::MainWindow *ui;
    Tp::AccountManagerPtr m_accountManager;
    AccountsModel *m_accountsModel;
    KTp::ContactGridWidget *m_contactGridWidget;
    const QString m_sshContactPath;
    const QString m_terminalPath;
    const QString m_terminalArgs;
};

#endif // KTP_SSH_CONTACT_MAINWINDOW_H
