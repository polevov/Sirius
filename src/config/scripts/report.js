function run()
{
    if(sirius.getCurrentFilePath()==="")
        return;
    sirius.execute('report.exe',[sirius.getCurrentFilePath()],false); //Запуск программы
}
