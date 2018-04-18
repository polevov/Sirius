.import "common/savetask.js" as Ext
function run()
{
    var a=Ext.saveJOB(); //Сохраняем заадние в формате KOL. Функция возвращает имя созданного файла.
    var result=job.execute('WSRS.exe',','+a+',',true);
    if(result) //Запуск программы
    {
        job.switchToTab(1); //После завершения программы переключиться на вкладку с результатом
    }
}
