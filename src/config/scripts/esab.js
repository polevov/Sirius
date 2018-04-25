function check()
{
    return sirius.getCurrentFilePath()!=="" && sirius.getCurrentTab()===sirius.tabResult;
}

function run()
{
    if(sirius.getCurrentFilePath()==="")
        return;
    sirius.execute('postproc\\esab.exe',[sirius.getCurrentFilePath()],true); //Запуск программы
    sirius.switchToTab(sirius.tabResult); //После завершения программы переключиться на вкладку с заданием
}
