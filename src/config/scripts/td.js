function run()
{
    job.execute('c:\\Program files\\Microsoft Office\\msaccess.exe',[job.getPath(0)+'\\taskdb.mde'],true); //Запуск программы
}
