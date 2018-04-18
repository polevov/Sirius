function run()
{
    if(job.getCurrentFilePath()==="")
        return;
    job.execute('pdbs.exe',[job.getCurrentFilePath()],false); //Запуск программы
}
