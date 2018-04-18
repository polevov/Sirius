function run()
{
    if(job.getCurrentFilePath()==="")
        return;
    job.execute('editp.exe',[job.getCurrentFilePath()],true); //Запуск программы
    job.switchToTab(1); //После завершения программы переключиться на вкладку с заданием
}
