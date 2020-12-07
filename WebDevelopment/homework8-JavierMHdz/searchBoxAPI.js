const data = (query, callbackFunc) =>{
    const url = "https://api.cognitive.microsoft.com/bing/v7.0/suggestions?q=" + query;
    const xmlhttp = new XMLHttpRequest();
    
    xmlhttp.open("GET", url);
    xmlhttp.setRequestHeader("Ocp-Apim-Subscription-Key","da8e7aaeb6194112bfd346fb5cc74219");

    xmlhttp.onreadystatechange = function(){
        if(xmlhttp.readyState == 4){
            if(xmlhttp.status == 200){
                const obj = JSON.parse(xmlhttp.responseText);
                callbackFunc(obj);
            }
        }
    }
    xmlhttp.send();
}

export default data;