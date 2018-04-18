function run()
{
    if(job.getCurrentFilePath()==="")
        return;
    job.execute('WSRS.exe',',,'+job.getCurrentFilePath(),false); //Запуск программы
}
