function run()
{
    if(sirius.getCurrentFilePath()==="")
        return;
    sirius.execute('editp.exe',[sirius.getCurrentFilePath()],true); //Запуск программы
    sirius.switchToTab(sirius.tabResult); //После завершения программы переключиться на вкладку с заданием
}
