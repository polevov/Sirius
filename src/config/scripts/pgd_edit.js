function check()
{
    return sirius.getCurrentFilePath()!=="";
}

function run()
{
    if(sirius.getCurrentFilePath()==="")
        return;
    sirius.execute('Pgd.exe',[sirius.getCurrentFilePath()],false); //Запуск программы
}
