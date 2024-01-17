#include "qmllanguage.h"

QmlLanguage::QmlLanguage(QGuiApplication &app, QQmlApplicationEngine& engine)
{
    m_app = &app;
    m_engine = &engine;
}
void QmlLanguage::setLanguage(int nLanguage)
{
    QTranslator translator;
    if (nLanguage == 0){
           translator.load(":/language_zh.qm");
    }else if (nLanguage == 1){
        translator.load(":/language_en.qm");
    }else if (nLanguage == 2){
        translator.load(":/language_de.qm");
    }
    m_app->installTranslator(&translator);
    m_engine->retranslate();
}
