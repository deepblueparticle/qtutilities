#include "resources.h"

#include <QString>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#if defined(GUI_QTWIDGETS)
# include <QApplication>
# include <QIcon>
# include <QFont>
# include <QStyleFactory>
#elif defined(GUI_QTQUICK)
# include <QGuiApplication>
# include <QIcon>
# include <QFont>
#else
# include <QCoreApplication>
#endif

#include <iostream>

using namespace std;

void qInitResources_qtutilsicons();
void qCleanupResources_qtutilsicons();

namespace QtUtilitiesResources {

/*!
 * \brief Initiates the resources used and provided by this library.
 */
void init()
{
    qInitResources_qtutilsicons();
}

/*!
 * \brief Frees the resources used and provided by this library.
 */
void cleanup()
{
    qCleanupResources_qtutilsicons();
}

}

namespace TranslationFiles {

bool hasEnglishTranslator = false;

/*!
 * \brief Loads and installs the appropriate Qt translation file for the current locale.
 */
void loadQtTranslationFile()
{
    QLocale locale;
    if(locale.language() != QLocale::English) {
        QTranslator *qtTranslator = new QTranslator;
        if(qtTranslator->load(QStringLiteral("qt_%1").arg(locale.name()),
                          QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
            QCoreApplication::installTranslator(qtTranslator);
        } else if(qtTranslator->load(QStringLiteral("qt_%1").arg(locale.name()), QStringLiteral("../share/qt/translations"))) {
            // used in Windows
            QCoreApplication::installTranslator(qtTranslator);
        } else {
            delete qtTranslator;
            cout << "Unable to load Qt translation file for the language " << locale.name().toStdString() << "." << endl;
        }
    }
}

/*!
 * \brief Loads and installs the appropriate application translation file for the current locale.
 * \param applicationName Specifies the name of the application.
 * \remarks Translation files have to be placed in one of the following
 *          locations:
 *           - ./translations
 *           - /usr/share/$application/translations (used in UNIX)
 *           - ../share/$application/translations (used in Windows)
 *           - ./projects/%1/translations (used during developement with subdirs project)
 *          Translation files must be named using the following scheme:
 *           - $application_$language.qm
 */
void loadApplicationTranslationFile(const QString &applicationName)
{
    // load English translation files as fallback
    loadApplicationTranslationFile(applicationName, QStringLiteral("en_US"));
    hasEnglishTranslator = true;
    // load translation files for current locale
    loadApplicationTranslationFile(applicationName, QLocale().name());
}

/*!
 * \brief Loads and installs the appropriate application translation file for the specified locale.
 * \param applicationName Specifies the name of the application.
 * \param localName Specifies the name of the locale.
 * \remarks Translation files have to be placed in one of the following
 *          locations:
 *           - ./translations
 *           - /usr/share/$application/translations (used in UNIX)
 *           - ../share/$application/translations (used in Windows)
 *           - ./projects/%1/translations (used during developement with subdirs project)
 *          Translation files must be named using the following scheme:
 *           - $application_$language.qm
 */
void loadApplicationTranslationFile(const QString &applicationName, const QString &localeName)
{
    QTranslator *appTranslator = new QTranslator;
    QString fileName = QStringLiteral("%1_%2").arg(applicationName, localeName);
    if(appTranslator->load(fileName, QStringLiteral("./translations"))) {
        QCoreApplication::installTranslator(appTranslator);
    } else if(appTranslator->load(fileName, QStringLiteral("./projects/%1/translations").arg(applicationName))) {
        QCoreApplication::installTranslator(appTranslator);
    } else if(appTranslator->load(fileName, QStringLiteral("/usr/share/%1/translations").arg(applicationName))) {
        QCoreApplication::installTranslator(appTranslator);
    } else if(appTranslator->load(fileName, QStringLiteral("../share/%1/translations").arg(applicationName))) {
        QCoreApplication::installTranslator(appTranslator);
    } else {
        delete appTranslator;
        if(localeName != QStringLiteral("en_US")) {
            cout << "Unable to load application translation file for the language \"" << localeName.toStdString() << "\", falling back to language \"en_US\"." << endl;
            if(!hasEnglishTranslator) {
                loadApplicationTranslationFile(applicationName, QStringLiteral("en_US"));
                hasEnglishTranslator = true;
            }
        } else {
            cout << "Unable to load application translation file for the language \"" << localeName.toStdString() << "\"." << endl;
        }
    }
}

}

namespace Theme {

/*!
 * \brief Sets the default icon theme.
 */
#if defined(GUI_QTWIDGETS) || defined(GUI_QTQUICK)
void setup()
{
    if(QIcon::themeName().isEmpty()) {
        QIcon::setThemeName(QStringLiteral("oxygen"));
    }
}
#endif

}

namespace ApplicationInstances {

#if defined(GUI_QTWIDGETS)
bool hasWidgetsApp()
{
    return qobject_cast<QApplication*>(QCoreApplication::instance()) != nullptr;
}
#endif

#if defined(GUI_QTWIDGETS) || defined(GUI_QTQUICK)
bool hasGuiApp()
{
    return qobject_cast<QGuiApplication*>(QCoreApplication::instance()) != nullptr;
}
#endif

bool hasCoreApp()
{
    return qobject_cast<QCoreApplication*>(QCoreApplication::instance()) != nullptr;
}

}
