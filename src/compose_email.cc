/*
  Diana - A Free Meteorological Visualisation Tool

  Copyright (C) 2006-2017 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
  NORWAY
  email: diana@met.no

  This file is part of Diana

  Diana is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Diana is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Diana; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "compose_email.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QImage>
#include <QListView>
#include <QMessageBox>
#include <QTabWidget>
#include <QTemporaryFile>

// #define WRITE_TO_FILE

//******************************************************************************
//* MailDialog::MailDialog( QWidget* parent, Controller* llctrl )
//*  : QDialog(parent), m_ctrl(llctrl)
//*
//*   Purpose : MailDialog constructor
//*   To call :
//*   Returns :
//*      Note :
//******************************************************************************
MailDialog::MailDialog(QWidget* parent)
    : QDialog(parent)
    , from_("diana_noreply@met.no")
{
  //--- Create dialog elements ---
    createGridGroupBox();
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    //--- Connect buttons to actions ---
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    //--- Create dialog ---
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(gridGroupBox);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    //--- Set dialog title ---
    setWindowTitle(tr("E-Mail Picture"));
    setWindowIcon(QIcon(":/icons/compose-email.svg"));
}

//******************************************************************************
//* Maildialog::createGridGroupBox()
//*
//*   Purpose : Creates dialog layout
//*   To call : Nothing
//*   Returns : Nothing
//*      Note :
//******************************************************************************
void MailDialog::createGridGroupBox()
{
    //--- Group box containg TO:, CC: & Subject elements ---
    gridGroupBox = new QGroupBox(tr("E-Mail Details"));
    QGridLayout *layout = new QGridLayout;
    //--- Create text & input box for TO: ---
    eToLabel = new QLabel(tr("To:"));
    eToEdit = new QLineEdit;
    layout->addWidget(eToLabel, 1, 0, Qt::AlignRight);
    layout->addWidget(eToEdit, 1, 1);
    //--- Create text & input box for CC: ---
    eCcLabel = new QLabel(tr("Cc:"));
    eCcEdit = new QLineEdit;
    layout->addWidget(eCcLabel, 2, 0, Qt::AlignRight);
    layout->addWidget(eCcEdit, 2, 1);
    //--- Create text & input box for Subject: ---
    eSubjectLabel = new QLabel(tr("Subject:"));
    eSubjectEdit = new QLineEdit;
    layout->addWidget(eSubjectLabel, 3, 0, Qt::AlignRight);
    layout->addWidget(eSubjectEdit, 3, 1);
    //---
    QTabWidget* tabs = new QTabWidget(this);
    eTextEdit = new QTextEdit;
    tabs->addTab(eTextEdit, tr("Message"));
    attachments_ = new AttachedFilesModel(this);
    QListView *listView = new QListView(this);
    listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    listView->setDragEnabled(true);
    listView->setModel(attachments_);
    tabs->addTab(listView, tr("Attachments"));
    layout->addWidget(tabs, 4, 0, 1, 2);
    //--- Set layout manager for widget ---
    gridGroupBox->setLayout(layout);
}

void MailDialog::attach(const QString& filename)
{
  attachments_->attach(filename);
}

//******************************************************************************
//* Maildialog::accept()
//*
//*   Purpose : Sends mail
//*   To call : Nothing
//*   Returns : Nothing
//*      Note :
//******************************************************************************
void MailDialog::accept()
{
#ifdef WRITE_TO_FILE
    FILE* sendmail = fopen("/tmp/mail.txt", "w");
#else
    FILE* sendmail = popen("/usr/lib/sendmail -t", "w");
#endif

    if (!sendmail) {
      QMessageBox::critical(this, tr("Cannot send mail"), tr("Unable to start sendmail program."));
      return;
    }

    //--- Mail header ---
    fprintf(sendmail,"MIME-Version: 1.0\n");
    fprintf(sendmail,"From: %s\n", from_.toStdString().c_str());
    fprintf(sendmail,"To: %s\n", eToEdit->text().toStdString().c_str());
    fprintf(sendmail,"Cc: %s\n", eCcEdit->text().toStdString().c_str());
    fprintf(sendmail,"Subject: %s\n", eSubjectEdit->text().toStdString().c_str());
    fprintf(sendmail,"Content-Type: multipart/mixed; boundary=\"diana_auto_generated\"\n\n");
    fprintf(sendmail,"This is a multi-part, MIME format, message.\n");
    //--- Mail body ---
    fprintf(sendmail,"--diana_auto_generated\n");
    fprintf(sendmail,"Content-type: text/plain\n\n");
    fprintf(sendmail,"%s\n\n", eTextEdit->toPlainText().toStdString().c_str());

    //--- Mail attachments ---
    const QStringList& filenames = attachments_->attachments();
    Q_FOREACH (const QString& filename, filenames) {
        //--- read the image and convert to Base64 byte array ---
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)){
            return;
        }
        QDataStream in(&file);
        QByteArray rawarray;
        QDataStream datastream(&rawarray, QIODevice::WriteOnly);
        char a;
        // copy between the two data streams
        while (in.readRawData(&a,1) != 0){
            datastream.writeRawData(&a,1);
        }
        QByteArray base64array = rawarray.toBase64();

        QFileInfo fi(filename);
        const std::string basename = fi.fileName().toStdString();

        const QString suffix = fi.suffix();
        const char* mime;
        if (suffix == "png")
            mime = "image/png";
        else if (suffix == "gif")
            mime = "image/gif";
        else if (suffix == "pdf")
            mime = "application/pdf";
        else
            mime = "application/octet-stream";

        fprintf(sendmail,"--diana_auto_generated\n");
        fprintf(sendmail,"Content-type: %s; name=\"%s\"\n", mime, basename.c_str());
        fprintf(sendmail,"Content-Transfer-Encoding: base64\n");
        fprintf(sendmail,"Content-Disposition: inline; filename=\"%s\"\n\n", basename.c_str());
        //--- the image data ---
        const char *data = base64array.constData();
        fprintf(sendmail,"%s",data);
        fprintf(sendmail,"\n\n");
    }
    fprintf(sendmail,"--diana_auto_generated--\n\n");

    //--- Finished with sendmail ---
#ifdef WRITE_TO_FILE
    fclose(sendmail);
#else
    pclose(sendmail);
#endif
    QDialog::accept();
}
