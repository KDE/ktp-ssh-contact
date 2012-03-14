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

#include <KDE/KCmdLineArgs>
#include <KDE/KApplication>
#include <KDE/KAboutData>
#include <KDE/KLocale>
#include <KDE/KDebug>
#include <KDE/KConfigDialog>

#include "mainwindow.h"
#include "version.h"


int main(int argc, char *argv[])
{
    KAboutData aboutData("ktp-ssh-contact", 0,
                         ki18n("KTp SSH Contact"),
                         KTP_SSH_CONTACT_VERSION,
                         ki18n("KTp SSH Contact"),
                         KAboutData::License_GPL,
                         ki18n("(C) 2012, Daniele E. Domenichelli"));
    aboutData.addAuthor(ki18n("Daniele E. Domenichelli"),
                        ki18n("Author"),
                        "daniele.domenichelli@gmail.com");
    aboutData.setProductName("telepathy/ssh-contact");
    aboutData.setProgramIconName(QLatin1String("telepathy-kde"));

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("a");
    options.add("account <argument>", ki18n("The account ID"));
    options.add("c");
    options.add("contact <argument>", ki18n("The contact ID"));
    options.add("l");
    options.add("login <argument>", ki18n("Specifies the user to log in as on the remote machine"));
    options.add("ssh-contact-path <argument>", ki18n("Path to the ssh-contact executable"));
    options.add("terminal-path <argument>", ki18n("Path to the chosen terminal"));
    options.add("terminal-args <argument>", ki18n("Arguments to pass to the terminal"));
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    KApplication app;

    // Read previous config
    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup configGroup(config, "SSH Contact");

    QString sshContactPath = configGroup.readEntry("sshContactPath", "/usr/bin/ssh-contact");
    if (args->isSet("ssh-contact-path")) {
        sshContactPath = args->getOption("ssh-contact-path");
    }

    // Check if ssh-contact is installed and is executable
    QFileInfo sshContactFileInfo(sshContactPath);
    if (!sshContactFileInfo.exists() || !sshContactFileInfo.isExecutable()) {
        kWarning() << "ssh-contact executable" << sshContactPath << "not found";
        return 1;
    }

    QString terminalPath = configGroup.readEntry("terminalPath", "/usr/bin/x-terminal-emulator");
    if (args->isSet("terminal-path")) {
        terminalPath = args->getOption("terminal-path");
    }

    // Check if terminal exists and is executable
    QFileInfo terminalFileInfo(terminalPath);
    if (!terminalFileInfo.exists() || !terminalFileInfo.isExecutable()) {
        kWarning() << "Cannot find" << terminalPath;
        return 2;
    }

    QString terminalArgs = configGroup.readEntry("terminalArgs", "--noclose -e");
    if (args->isSet("terminal-args")) {
        terminalArgs = args->getOption("terminal-args");
    }

    // Save settings for next time
    configGroup.writeEntry("sshContactPath", sshContactPath);
    configGroup.writeEntry("terminalPath", terminalPath);
    configGroup.writeEntry("terminalArgs", terminalArgs);
    configGroup.config()->sync();


    QString account;
    QString contact;
    QString login;

    if (args->isSet("account")) {
        account = args->getOption("account");
    }

    if (args->isSet("contact")) {
        contact = args->getOption("contact");
    }

    if (args->isSet("login")) {
        login = args->getOption("login");
    }

    if (account.isEmpty() || contact.isEmpty() || contact.isEmpty()) {
        MainWindow *window = new MainWindow(account,
                                            contact,
                                            login,
                                            sshContactPath,
                                            terminalPath,
                                            terminalArgs);
        window->show();
        return app.exec();
    } else {
        MainWindow::startProcess(account,
                                 contact,
                                 login,
                                 sshContactPath,
                                 terminalPath,
                                 terminalArgs);
        return 0;
    }

}

