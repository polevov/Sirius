function run()
{
    if(job.getCurrentFilePath()==="")
        return;
    job.execute('report.exe',[job.getCurrentFilePath()],false); //Запуск программы
}
