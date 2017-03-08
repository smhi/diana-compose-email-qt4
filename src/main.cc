
#include "compose_email.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QProcess>
#include <QTranslator>

static void installTranslator(QApplication* app, const QString& file, const QString& path)
{
  const QLocale& locale = QLocale::system();
  QTranslator* translator = new QTranslator(app);
  if (translator->load(locale, file, "_", path)) {
    app->installTranslator(translator);
  } else if (translator->load(path + "/" + file + "_" + locale.name())) {
    app->installTranslator(translator);
  } else {
    delete translator;
  }
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    installTranslator(&app, "compose_email", ":/translations");
    installTranslator(&app, "qt", QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    QString tmpdir, from;

    int a = 1;
    for (; a < argc; ++a) {
        QString arg(argv[a]);
        if (arg == "--")
            break;
        if (arg == "-tmpdir") {
            if (a+1 < argc) {
                tmpdir = argv[a+1];
                a += 1;
            }
        }
        if (arg == "-from") {
            if (a+1 < argc) {
                from = argv[a+1];
                a += 1;
            }
        }
    }
    a += 1; // skip "--"

    MailDialog dialog;
    if (!from.isEmpty())
        dialog.setFrom(from);
    for (; a<argc; ++a)
        dialog.attach(argv[a]);
    dialog.exec();

    if (!tmpdir.isEmpty()) {
        QProcess::execute("rm", QStringList() << "-rvf" << tmpdir);
    }

    return 0;
}
