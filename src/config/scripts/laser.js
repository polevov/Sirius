function check()
{
    return sirius.getCurrentFilePath()!=="";
}

function run()
{
    if(sirius.getCurrentFilePath()==="")
        return;
    sirius.execute('postproc\\laser.exe',[sirius.getCurrentFilePath()],true); //Запуск программы
    sirius.switchToTab(sirius.tabResult); //После завершения программы переключиться на вкладку с заданием
}
