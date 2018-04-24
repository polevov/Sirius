function check()
{
    return sirius.getCurrentFilePath()!=="";
}

function run()
{
    if(sirius.getCurrentFilePath()==="")
        return;
    sirius.execute('pdbs.exe',[sirius.getCurrentFilePath()],false); //Запуск программы
}
