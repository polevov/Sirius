.import "common/savetask.js" as Ext
function run()
{
    job.setMessage("Раскрой NCL");
    if(job.getCurrentTaskName()==="")
        return;
    //формируем INI файл с настройками NCL
    var nclIni='[Settings]\r\n';
    nclIni+='Dist='+job.getProperty('DistDetToDet')+'\r\n';
    nclIni+='S_Dist='+job.getProperty('DistDetToSheet')+'\r\n';
    nclIni+='Algoritm='+job.getProperty('SelectAlgorithm')+'\r\n';
    nclIni+='Build block='+job.getProperty('BuildBlock')?"1":"0"+'\r\n';
    nclIni+='Block step='+job.getProperty('BuildBlockStep')+'\r\n';
    nclIni+='Align block='+job.getProperty('RectOptimisation')+'\r\n';
    nclIni+='[Nest]\r\n';
    nclIni+='Step='+job.getProperty('StepCount')+'\r\n';
    nclIni+='Step_angle='+job.getProperty('Rotate')+'\r\n';
    nclIni+='Mirror='+job.getProperty('Mirror')?"1":"0"+'\r\n';
    nclIni+='Sort='+job.getProperty('Sorting')+'\r\n';
    nclIni+='Optimization='+job.getProperty('RectOptimisation')+'\r\n';
    nclIni+='Move='+job.getProperty('MoveOptimisation')+'\r\n';
    nclIni+='Box iteration='+job.getProperty('BoxIteration')+'\r\n';
    nclIni+='Box strong='+job.getProperty('BoxComplexity')+'\r\n';
    nclIni+='Box rotate='+job.getProperty('BoxRotate')?"1":"0"+'\r\n';
    nclIni+='Guillotine='+job.getProperty('BoxGilotine')?"1":"0"+'\r\n';
    //сохраняем INI файл
    job.saveToFile(job.getPath(0)+'\\utils\\ncl.ini',nclIni);

    var a=Ext.saveKOL(); //Сохраняем заадние в формате KOL. Функция возвращает имя созданного файла.
    var result=job.execute('ncl.exe',['/c',a],true);
    if(result) //Запуск программы
    {
        job.switchToTab(1); //После завершения программы переключиться на вкладку с результатом
    }
}
