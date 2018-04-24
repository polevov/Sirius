#include "scriptcontrol.h"
#include <QJSEngine>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QDebug>

ScriptControl::ScriptControl(QString script, Task *currentTask, QObject *parent):scriptExt(currentTask,parent),_script(script)
{
}

void ScriptControl::SetScript(QString script)
{
    _script=script;
}

void ScriptControl::TerminateProcess()
{
    if(hProcess!=INVALID_HANDLE_VALUE)
    {
        ::TerminateProcess(hProcess,100);
    }
}

QStringList errorList;
void myMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    errorList<<msg;
}

QVariant ScriptControl::Execute(QString function)
{
    QQmlEngine js;
 //   js.installExtensions(QJSEngine::ConsoleExtension);
    QJSValue jsExt = js.newQObject(this);
    QVariant result;
    js.globalObject().setProperty("sirius",jsExt);
    js.evaluate(QString("sirius.dirJob=%1;").arg(scriptExt::dirJob));
    js.evaluate(QString("sirius.dirApplication=%1;").arg(scriptExt::dirApplication));
    js.evaluate(QString("sirius.dirTask=%1;").arg(scriptExt::dirTask));
    js.evaluate(QString("sirius.dirDraws=%1;").arg(scriptExt::dirDraws));
    js.evaluate(QString("sirius.tabTask=%1;").arg(scriptExt::tabTask));
    js.evaluate(QString("sirius.tabResult=%1;").arg(scriptExt::tabResult));
    js.evaluate(QString("sirius.typeDetail=%1;").arg(DetailType::typeDetail));
    js.evaluate(QString("sirius.typeSheet=%1;").arg(DetailType::typeSheet));
    js.evaluate(QString("sirius.typeTool=%1;").arg(DetailType::typeTool));
    QQmlComponent com(&js);
    js.setImportPathList(QStringList()<<js.importPathList()<<(QApplication::applicationDirPath()+"/config/scripts"));
    QString source=QString("import QtQuick 2.0\n"
                           "import \"%1\" as ProgramScriptModule\n\n"
                           "Item \n"
                           "{ \n"
                           "function runScript()\n"
                               "{\n"
                                   "return ProgramScriptModule.%2();\n"
                               "}\n"
                           "}").arg(_script,function);
    com.setData(source.toLatin1(),QUrl::fromLocalFile(QApplication::applicationDirPath()+"/config/scripts/"+_script));
    if(com.errors().count()>0)
    {
        QString error_str;
        foreach (QQmlError str, com.errors())
        {
            error_str+=str.toString()+"\n";
        }
        emit error(error_str.trimmed());
    }
    else
    {
        QObject* ob=com.create();
        errorList.clear();
        qInstallMessageHandler(myMessage);
        QMetaObject::invokeMethod(ob,"runScript",Q_RETURN_ARG(QVariant, result));
        qInstallMessageHandler(0);
        if(errorList.size()>0)
        {
            QString error_str;
            foreach (QString str, errorList)
            {
                error_str+=str+"\n";
            }
            emit error(error_str.trimmed());
        }
        delete ob;
    }
    emit finished();
    return result;
}
