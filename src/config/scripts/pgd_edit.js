function run()
{
    if(job.getCurrentFilePath()==="")
        return;
    job.execute('Pgd.exe',[job.getCurrentFilePath()],false); //Запуск программы
    job.switchToTab(0); //После завершения программы переключиться на вкладку с заданием
}
