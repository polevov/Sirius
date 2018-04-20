function run()
{
    if(sirius.getCurrentFilePath()==="")
        return;
    sirius.execute('postproc\\trum3050.exe',[sirius.getCurrentFilePath()],true); //Запуск программы
    sirius.switchToTab(sirius.tabResult); //После завершения программы переключиться на вкладку с заданием
}
