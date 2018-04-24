.import "common/savetask.js" as Ext
function check()
{
    return sirius.getCurrentTaskName()!=="";
}

function run()
{
    sirius.setMessage("Раскрой NCL");
    if(sirius.getCurrentTaskName()==="")
        return;
    //формируем INI файл с настройками NCL
    var nclIni='[Settings]\r\n';
    nclIni+='Dist='+sirius.getProperty('DistDetToDet')+'\r\n';
    nclIni+='S_Dist='+sirius.getProperty('DistDetToSheet')+'\r\n';
    nclIni+='Algoritm='+sirius.getProperty('SelectAlgorithm')+'\r\n';
    nclIni+='Build block='+sirius.getProperty('BuildBlock')?"1":"0"+'\r\n';
    nclIni+='Block step='+sirius.getProperty('BuildBlockStep')+'\r\n';
    nclIni+='Align block='+sirius.getProperty('RectOptimisation')+'\r\n';
    nclIni+='[Nest]\r\n';
    nclIni+='Step='+sirius.getProperty('StepCount')+'\r\n';
    nclIni+='Step_angle='+sirius.getProperty('Rotate')+'\r\n';
    nclIni+='Mirror='+sirius.getProperty('Mirror')?"1":"0"+'\r\n';
    nclIni+='Sort='+sirius.getProperty('Sorting')+'\r\n';
    nclIni+='Optimization='+sirius.getProperty('RectOptimisation')+'\r\n';
    nclIni+='Move='+sirius.getProperty('MoveOptimisation')+'\r\n';
    nclIni+='Box iteration='+sirius.getProperty('BoxIteration')+'\r\n';
    nclIni+='Box strong='+sirius.getProperty('BoxComplexity')+'\r\n';
    nclIni+='Box rotate='+sirius.getProperty('BoxRotate')?"1":"0"+'\r\n';
    nclIni+='Guillotine='+sirius.getProperty('BoxGilotine')?"1":"0"+'\r\n';
    //сохраняем INI файл
    sirius.saveToFile(sirius.getPath(sirius.dirApplication)+'\\ncl.ini',nclIni);

    var a=Ext.saveKOL(); //Сохраняем задание в формате KOL. Функция возвращает имя созданного файла.
    var result=sirius.execute('ncl.exe',['/c',a],true);
    if(result) //Запуск программы
    {
        sirius.switchToTab(sirius.tabResult); //После завершения программы переключиться на вкладку с результатом
    }
}
