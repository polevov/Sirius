function check()
{
    return sirius.getCurrentFilePath()!=="";
}

function check()
{
    return sirius.getCurrentFilePath()!=="";
}

function run()
{
    if(sirius.getCurrentFilePath()==="")
        return;
    sirius.execute('WSRS.EXE',',,'+sirius.getCurrentFilePath(),false); //Запуск программы
}
