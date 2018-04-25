function check()
{
    return sirius.getCurrentFilePath()!=="" && sirius.getCurrentTab()===sirius.tabResult;
}

function run()
{
    if(sirius.getCurrentFilePath()==="")
        return;
    sirius.execute('WSRS.exe',',,'+sirius.getCurrentFilePath(),false); //Запуск программы
}
