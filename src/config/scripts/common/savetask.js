//Сохраняем заадние в формате KOL.
//Функция возвращает имя созданного файла.
function saveKOL()
{
    var json_task=sirius.getCurrentTask();
    var kol_file='';
    var task=JSON.parse(json_task)['task'];
    var draw_dir=sirius.getPath(sirius.dirDraws);
    for(var index=0;index<task.length;index++)
    {
        if(task[index]['itemType']===sirius.typeSheet || task[index]['itemType']===sirius.typeDetail)
        {
            kol_file=kol_file+draw_dir+'\\'+task[index]['fileName']+' '+task[index]['count']+' '+task[index]['itemType']+'\r\n';
        }
    }
    var fileName=sirius.getPath(sirius.dirJob)+'\\'+sirius.getCurrentTaskName()+'.kol';
    sirius.saveToFile(fileName,kol_file);
    return fileName;
}

//Сохраняем заадние в формате J??.
//Функция возвращает имя созданного файла.
function saveJOB()
{
    var json_task=sirius.getCurrentTask();
    var job_file_first='';
    var job_file_current='';
    var task=JSON.parse(json_task)['task'];
    var draw_dir=sirius.getPath(sirius.dirDraws);
    var current_index=0;
    var find_selected=false;
    for(var index=0;index<task.length;index++)
    {
        var currnet_file='';
        switch(task[index]['itemType'])
        {
        case sirius.typeSheet:
            if(find_selected)
                break;
            currnet_file+="ACC/"+draw_dir+'\\'+task[index]['fileName']+'\r\n';
            currnet_file+="LOAD/"+'\r\n';
            current_index++;
            job_file_current='';
            break;
        case sirius.typeDetail:
            currnet_file+="ACC/"+draw_dir+'\\'+task[index]['fileName']+'\r\n';
            currnet_file+='LOAD/0'+'\r\n';
            if(task[index]['count']>1)
                currnet_file+='QUAN/:'+task[index]['fileName'].substr(0,task[index]['fileName'].length-4)+','+(task[index]['count']-1)+'\r\n';
            break;
        }
        if(current_index<=1)
            job_file_first+=currnet_file;
        job_file_current+=currnet_file;
        find_selected=find_selected || task[index]['selected'];
    }
    var job_file='';
    if(find_selected)
    {
        job_file=job_file_current;
    }
    else
    {
        job_file=job_file_first;
        current_index=1;
    }
    var current_index_str=(current_index<=9?('0'+current_index):current_index);
    job_file+='ACC/'+sirius.getCurrentTaskName().toUpperCase()+'_'+current_index_str+'\r\n';
    job_file+='CUR CEN DRA/0'+'\r\n';
    job_file+='UNSAVE/-999 -999'+'\r\n';
    job_file+='SAVE'+'\r\n';
    job_file+='EMPTY/-9999'+'\r\n';
    job_file+='LOAD CUR CEN DRA/0'+'\r\n';
    job_file+='UNSAVE/-999 -999'+'\r\n';
    job_file+='ALLOW/5'+'\r\n';
    job_file+='NEST/1'+'\r\n';
    job_file+='EOF/EOF'+'\r\n';
    var fileName=sirius.getPath(sirius.dirJob)+'\\'+sirius.getCurrentTaskName()+'.j'+current_index_str;
    sirius.saveToFile(fileName,job_file);
    if(current_index===0)
        return "";
    return fileName;
}
