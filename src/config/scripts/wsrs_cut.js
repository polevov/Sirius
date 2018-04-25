function check()
{
    return sirius.getCurrentFilePath()!=="" && sirius.getCurrentTab()===sirius.tabResult;
}

function run()
{
    if(sirius.getCurrentFilePath()==="")
        return;
    sirius.execute('WSRS.EXE',',,'+sirius.getCurrentFilePath(),false); //Запуск программы
}
