.import "common/savetask.js" as Ext
function run()
{
    var a=Ext.saveJOB(); //Сохраняем задание в формате KOL. Функция возвращает имя созданного файла.
    var result=sirius.execute('WSRS.exe',','+a+',',true);
    if(result) //Запуск программы
    {
        sirius.switchToTab(sirius.tabResult); //После завершения программы переключиться на вкладку с результатом
    }
}
