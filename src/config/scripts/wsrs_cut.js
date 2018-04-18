function run()
{
    if(job.getCurrentFilePath()==="")
        return;
    job.execute('WSRS.EXE',',,'+job.getCurrentFilePath(),false); //Запуск программы
}
